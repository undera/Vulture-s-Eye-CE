/* NetHack may be freely redistributed.  See license for details. */

#include "vulture_sdl.h" /* XXX this must be the first include,
                             no idea why but it won't compile otherwise */

#include "config.h"
#include "global.h"
#include <math.h>
#include <algorithm>

#include "vulture_win.h"
#include "vulture_gra.h"
#include "vulture_gfl.h"
#include "vulture_mou.h"
#include "vulture_tile.h"
#include "vulture_txt.h"

#include "statusattr.h"

#define META(c) (0x80 | (c))

statusattr *stattr;

statusattr::statusattr(window *p) : window(p)
{
	stattr = this;
	autobg = true;
	visible = true;
}

statusattr::~statusattr()
{
}

bool statusattr::draw()
{
	vulture_set_draw_region(abs_x, abs_y, abs_x + w - 1, abs_y + h - 1);

	// actual draw START
	float strength = acurr(A_STR) / 25.0;
	float dexterity = acurr(A_DEX) / 25.0;
	float consitution = acurr(A_CON) / 25.0;

	float intelligence = acurr(A_INT) / 25.0;
	float wisdom = acurr(A_WIS) / 25.0;
	float charisma = acurr(A_CHA) / 25.0;

	int MAX_AXES = 6;
	float dataValues[MAX_AXES] = {
		dexterity,
		strength,
		consitution,
		charisma,
		intelligence,
		wisdom,
	};

	char *dataLabels[MAX_AXES] = {
		"Dx",
		"St",
		"Co",
		"Ch",
		"In",
		"Wi",
	};

	int centerX = this->x + this->w / 2;
	int centerY = this->y + this->w / 2;
	int radius = this->w / 2 - (vulture_get_lineheight(V_FONT_SMALL) + 2);

	int edgePoints[MAX_AXES][2];
	int dataPoints[MAX_AXES][2];
	int labelPoints[MAX_AXES][2];

	// do math
	float angleIncrement = 2 * M_PI / MAX_AXES;

	for (int i = 0; i < MAX_AXES; ++i)
	{
		float angle = angleIncrement * i;
		float dataRadius = radius * dataValues[i];
		edgePoints[i][0] = centerX + radius * cos(angle);
		edgePoints[i][1] = centerY + radius * sin(angle);
		dataPoints[i][0] = centerX + dataRadius * cos(angle);
		dataPoints[i][1] = centerY + dataRadius * sin(angle);

		labelPoints[i][0] = (radius + vulture_get_lineheight(V_FONT_SMALL) / 2) * cos(angle);
		char *lbl = dataLabels[i];
		if (labelPoints[i][0] > centerX)
		{
			labelPoints[i][0] += vulture_text_length(V_FONT_SMALL, lbl) / 2;
		}
		else if (labelPoints[i][0] < centerX)
		{
			labelPoints[i][0] -= vulture_text_length(V_FONT_SMALL, lbl) / 2;
		}
		labelPoints[i][0] += centerX;

		labelPoints[i][1] = (radius + vulture_get_lineheight(V_FONT_SMALL) / 2) * sin(angle);
		if (labelPoints[i][1] > centerY)
		{
			labelPoints[i][1] += vulture_get_lineheight(V_FONT_SMALL) / 2;
		}
		else if (labelPoints[i][1] < centerY)
		{
			labelPoints[i][1] -= vulture_get_lineheight(V_FONT_SMALL) / 2;
		}
		labelPoints[i][1] += centerY;
	}

	// Drawing data
	for (int i = 0; i < MAX_AXES; ++i)
	{
		int nx = dataPoints[0][0];
		int ny = dataPoints[0][1];
		if (i < MAX_AXES - 1)
		{
			nx = dataPoints[i + 1][0];
			ny = dataPoints[i + 1][1];
		}

		vulture_triangle(centerX, centerY, dataPoints[i][0], dataPoints[i][1], nx, ny, CLR32_BROWN, true);
	}

	// Drawing perimeter (spiderweb)
	for (int i = 0; i < MAX_AXES; ++i)
	{
		vulture_free_line(centerX, centerY, edgePoints[i][0], edgePoints[i][1], CLR32_GRAY70);
		vulture_put_text(V_FONT_TOOLTIP, dataLabels[i], vulture_screen, labelPoints[i][0], labelPoints[i][1], CLR32_GRAY20);
	}

	// actual draw END

	vulture_set_draw_region(0, 0, vulture_screen->w - 1, vulture_screen->h - 1);

	vulture_invalidate_region(abs_x, abs_y, w, h);

	return true;
}

eventresult statusattr::handle_timer_event(window *target, void *result, int time)
{
	return V_EVENT_HANDLED_NOREDRAW;
}

eventresult statusattr::handle_mousemotion_event(window *target, void *result, int xrel,
												 int yrel, int state)
{
	vulture_set_mcursor(V_CURSOR_NORMAL);
	return V_EVENT_HANDLED_NOREDRAW;
}

eventresult statusattr::handle_resize_event(window *target, void *result, int res_w, int res_h)
{
	/* this relies on the fact that the enhance window is created
	 * immediately after the status window */
	return V_EVENT_HANDLED_NOREDRAW;
}

void vulture_free_line_surface(
	SDL_Surface *surface,
	int x1, int y1,
	int x2, int y2,
	Uint32 color)
{
	Uint32 *screen;

	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	screen = (Uint32 *)surface->pixels;

	int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
	int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */

	while (true)
	{ /* loop */
		screen[y1 * surface->w + x1] = color;
		if (x1 == x2 && y1 == y2)
			break;
		e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			x1 += sx;
		} /* e_xy+e_x > 0 */
		if (e2 <= dx)
		{
			err += dx;
			y1 += sy;
		} /* e_xy+e_y < 0 */
	}

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}

void vulture_triangle_surface(
	SDL_Surface *surface,
	int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	Uint32 color, bool fill)
{
	if (fill)
	{
		vulture_fill_triangle_surface(surface, x1, y1, x2, y2, x3, y3, color);
	}
	else
	{
		vulture_free_line_surface(surface, x1, y1, x2, y2, color);
		vulture_free_line_surface(surface, x2, y2, x3, y3, color);
		vulture_free_line_surface(surface, x3, y3, x1, y1, color);
	}
}

struct Point
{
	int x, y;
};

// Helper function to sort points vertically
bool comparePoints(const Point &a, const Point &b)
{
	return a.y < b.y;
}

void vulture_fill_triangle_surface(
	SDL_Surface *surface,
	int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	Uint32 color)
{
	Point p1 = Point{x : x1, y : y1};
	Point p2 = Point{x : x2, y : y2};
	Point p3 = Point{x : x3, y : y3};
	// GPT start
	// Sort the vertices by y-coordinate (top to bottom)
	Point vertices[] = {p1, p2, p3};
	std::sort(vertices, vertices + 3, comparePoints);

	float dx1 = vertices[1].y != vertices[0].y ? float(vertices[1].x - vertices[0].x) / (vertices[1].y - vertices[0].y) : 0;
	float dx2 = vertices[2].y != vertices[0].y ? float(vertices[2].x - vertices[0].x) / (vertices[2].y - vertices[0].y) : 0;
	float dx3 = vertices[2].y != vertices[1].y ? float(vertices[2].x - vertices[1].x) / (vertices[2].y - vertices[1].y) : 0;

	float startX = vertices[0].x;
	float endX = vertices[0].x;

	for (int y = vertices[0].y; y <= vertices[2].y; y++)
	{
		vulture_free_line_surface(surface, startX, y, endX, y, color);
		if (y < vertices[1].y)
		{
			if (vertices[1].y != vertices[0].y)
				startX += dx1;
			if (vertices[2].y != vertices[0].y)
				endX += dx2;
		}
		else
		{
			if (vertices[2].y != vertices[1].y)
				startX = vertices[1].x + (y - vertices[1].y) * dx3;
			if (vertices[2].y != vertices[0].y)
				endX += dx2;
		}
	}
	// GPT end
}
