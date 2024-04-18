/* NetHack may be freely redistributed.  See license for details. */

#include "vulture_sdl.h" /* XXX this must be the first include,
                             no idea why but it won't compile otherwise */

#include "config.h"
#include "global.h"

extern "C"
{
#ifndef VULTURE_NETHACK_3_6_7
	boolean can_advance(int skill, int speedy);
#endif
}

#include "vulture_win.h"
#include "vulture_gra.h"
#include "vulture_gfl.h"
#include "vulture_mou.h"
#include "vulture_tile.h"
#include "vulture_txt.h"

#include "statusinv.h"

#define META(c) (0x80 | (c))

statusinv *stinv;

statusinv::statusinv(window *p) : window(p)
{
	tileSize = V_LISTITEM_HEIGHT + 2;
	h = tileSize * 4;
	w = tileSize * 4;

	stinv = this;
	autobg = true;
	visible = true;
}

statusinv::~statusinv()
{
}

bool statusinv::draw()
{
	vulture_set_draw_region(abs_x, abs_y, abs_x + w - 1, abs_y + h - 1);

	this->draw_obj(uamul, 0, 0);	// Amulet
	this->draw_obj(uarmh, 1, 0);  // Helmet
	this->draw_obj(ublindf, 2, 0);	// Blindfold
	this->draw_obj(uwep, 3, 0);		// Weapon

	this->draw_obj(uarmc, 0, 1);  // Cloak
	this->draw_obj(uarm, 1, 1);	  // Armour
	this->draw_obj(uarmg, 2, 1);  // Gloves
	this->draw_obj(uarms, 3, 1);  // Shield


	this->draw_obj(uleft, 0, 2);  // RingL
	this->draw_obj(uarmu, 1, 2);  // Undershirt
	this->draw_obj(uright, 2, 2); // RingR
	this->draw_obj(uquiver, 3, 2);	// Quiver

	this->draw_obj(uchain, 0, 3);	// chain
	this->draw_obj(uarmf, 1, 3);  // Shoes (feet)
	this->draw_obj(uball, 2, 3);	// ball
	this->draw_obj(uswapwep, 3, 3); // Secondary weapon

	// the only thing we skip is 		*uskin  // dragon armor, if a dragon

	vulture_set_draw_region(0, 0, vulture_screen->w - 1, vulture_screen->h - 1);

	vulture_invalidate_region(abs_x, abs_y, w, h);

	return true;
}

void statusinv::draw_obj(obj *obj, int col, int row)
{
	// most code is copied from objitemwin.cpp
	int cx, cy, cw, ch;
	cx = x + col * (tileSize + 1);
	cy = y + row * (tileSize + 1);
	cw = tileSize;
	ch = tileSize;

	char tmpstr[32];

	/* draw the tile itself */
	/* constrain the drawing region to the box for the object tile, so that large
	 * tiles don't overlap */
	vulture_set_draw_region(cx, cy, cx + ch, cy + ch);

	/* darken the background */
	vulture_fill_rect(cx + 2, cy + 2, cx + ch - 3, cy + ch - 3, CLR32_BLACK_A30); 

	if (obj)
	{
		/* indicate blessed/cursed visually */
		if (obj->bknown && obj->blessed)
			vulture_fill_rect(cx + 2, cy + 2, cx + ch - 3, cy + ch - 3, CLR32_BLESS_BLUE);

		if (obj->bknown && obj->cursed)
			vulture_fill_rect(cx + 2, cy + 2, cx + ch - 3, cy + ch - 3, CLR32_CURSE_RED);

		/* draw the object tile */
		int tile_x, tile_y;
		int tile = 0;

		tile = vulture_object_to_tile(obj->otyp, -1, -1, obj);

		tile_x = cx + ch / 2;
		tile_y = cy + ch * 3 / 4;

		if (TILE_IS_OBJECT(tile))
		{
			tile = tile - OBJTILEOFFSET + ICOTILEOFFSET;
			tile_x = cx + 2;
			tile_y = cy + 2;
		}

		vulture_put_tile(tile_x, tile_y, tile);

		/* draw the quantity on the tile */
		if (obj->quan > 1)
		{
			snprintf(tmpstr, 11, "%ld", obj->quan);
			int txt_height = vulture_text_height(V_FONT_MENU, tmpstr);
			int text_start_x = cx + ch - vulture_text_length(V_FONT_MENU, tmpstr) - 2;
			vulture_put_text_shadow(V_FONT_MENU, tmpstr, vulture_screen, text_start_x, cy + ch - txt_height, CLR32_WHITE, CLR32_BLACK);
		}
	}

	// outline
	vulture_rect(cx + 2, cy + 2, cx + ch - 3, cy + ch - 3, CLR32_GRAY70);

	/* restore the drawing region */
	vulture_set_draw_region(0, 0, vulture_screen->w - 1, vulture_screen->h - 1);
}

eventresult statusinv::handle_timer_event(window *target, void *result, int time)
{
	return V_EVENT_HANDLED_NOREDRAW;
}

eventresult statusinv::handle_mousemotion_event(window *target, void *result, int xrel,
												int yrel, int state)
{
	vulture_set_mcursor(V_CURSOR_NORMAL);
	return V_EVENT_HANDLED_NOREDRAW;
}

eventresult statusinv::handle_resize_event(window *target, void *result, int res_w, int res_h)
{
	/* this relies on the fact that the enhance window is created
	 * immediately after the status window */
	y = sib_prev->y - h;
	return V_EVENT_HANDLED_NOREDRAW;
}
