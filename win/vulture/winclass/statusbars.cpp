/* NetHack may be freely redistributed.  See license for details. */

#include "vulture_sdl.h" /* XXX this must be the first include,
                             no idea why but it won't compile otherwise */

extern "C"
{
	extern const char *hu_stat[];		 /* defined in eat.c */
	extern const char *const enc_stat[]; /* defined in botl.c */
}

#include "config.h"
#include "global.h"
#include <math.h>

#include "vulture_win.h"
#include "vulture_gra.h"
#include "vulture_gfl.h"
#include "vulture_mou.h"
#include "vulture_tile.h"
#include "vulture_txt.h"

#include "statusbars.h"

#define META(c) (0x80 | (c))

statusbars *stbars;
int barh;

statusbars::statusbars(window *p) : window(p)
{
	barh = vulture_get_lineheight(V_FONT_STATUS) + 2;

	w = 100;
	h = (barh + 5) * 5;

	stbars = this;
	autobg = true;
	visible = true;
}

statusbars::~statusbars()
{
}

bool statusbars::draw()
{
	float wMain = (inv_weight() + weight_cap()) / (float)weight_cap();
	float wSec = 0;
	if (wMain > 1)
	{
		wMain = 1;
		wSec = weight_cap() / (float)(inv_weight() + weight_cap());
	}

	float hMain, hSec;
	hMain = 1;
	if (u.uhunger > 1000)
	{
		hSec = 1000 / (float)u.uhunger;
	}
	else if (u.uhunger > 150)
	{
		hSec = 150 / (float)u.uhunger;
	}
	else if (u.uhunger > 50)
	{
		hSec = 50 / (float)u.uhunger;
	}
	else
	{
		hMain = u.uhunger / 50.0;
		hSec = 0;
	}

	vulture_set_draw_region(abs_x, abs_y, abs_x + w - 1, abs_y + h - 1);
	if (u.uhpmax > 0)
	{
		this->draw_bar(0, "HP", u.uhp / (float)u.uhpmax, 0, CLR32_RED, "");
	}
	if (u.uenmax > 0)
	{
		this->draw_bar(1, "Magic", u.uen / (float)u.uenmax, 0, CLR32_BLESS_BLUE, "");
	}
	this->draw_bar(2, "Food", hMain, hSec, CLR32_GOLD_SHADE, hu_stat[u.uhs]);
	int cap = near_capacity();
	this->draw_bar(3, "Weight", wMain, wSec, CLR32_BROWN, enc_stat[cap]);
	this->draw_bar(4, "XP", u.uexp / (float)newuexp(u.ulevel), 0, CLR32_LIGHTPINK, "");
	vulture_set_draw_region(0, 0, vulture_screen->w - 1, vulture_screen->h - 1);

	vulture_invalidate_region(abs_x, abs_y, w, h);

	return true;
}

void statusbars::draw_bar(int i, std::string label, float lvlMain, float lvlSecondary, Uint32 color, std::string hint)
{
	if (lvlMain < 0 || isinf(lvlMain) || std::isnan(lvlMain))
	{
		lvlMain = 0;
	}

	int by = y + (barh + 5) * i;
	vulture_fill_rect(x, by, x + w * lvlMain, by + barh, color);
	if (lvlSecondary > 0)
	{
		vulture_fill_rect(x + w * lvlSecondary, by, x + w * lvlSecondary, by + barh, CLR32_WHITE);
	}
	vulture_put_text(V_FONT_TOOLTIP, label, vulture_screen, x + 1, by + 1, CLR32_WHITE);
	vulture_put_text(V_FONT_TOOLTIP, hint, vulture_screen, x + w - vulture_text_length(V_FONT_TOOLTIP, label) - 1, by + 1, CLR32_YELLOW);
	vulture_rect(x, by, x + w, by + barh, CLR32_GRAY70);
}

eventresult statusbars::handle_timer_event(window *target, void *result, int time)
{
	return V_EVENT_HANDLED_NOREDRAW;
}

eventresult statusbars::handle_mousemotion_event(window *target, void *result, int xrel,
												 int yrel, int state)
{
	vulture_set_mcursor(V_CURSOR_NORMAL);
	return V_EVENT_HANDLED_NOREDRAW;
}

eventresult statusbars::handle_resize_event(window *target, void *result, int res_w, int res_h)
{
	/* this relies on the fact that the enhance window is created
	 * immediately after the status window */
	return V_EVENT_HANDLED_NOREDRAW;
}
