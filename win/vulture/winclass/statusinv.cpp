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
	statusbg = vulture_load_graphic(V_FILENAME_LOADOUT);
	this->w = statusbg->w;
	this->h = statusbg->h;

	tileSize = V_LISTITEM_HEIGHT + 2;

	stinv = this;
	autobg = true;
	visible = true;
}

statusinv::~statusinv()
{
	SDL_FreeSurface(statusbg);
}

bool statusinv::draw()
{
	vulture_set_draw_region(abs_x, abs_y, abs_x + w - 1, abs_y + h - 1);
	vulture_put_img(abs_x, abs_y, statusbg);

	this->draw_obj(uarmh, 0, 0);	  // Helmet
	this->draw_obj(ublindf, 1, 0);	  // Blindfold
	this->draw_obj(uamul, 0, 1);	  // Amulet
	this->draw_obj(uarm, 0, 2);		  // Armour

	this->draw_obj(uarmc, -1.5, 1.5); // Cloak
	this->draw_obj(uarms, 1.5, 1.5);  // Shield

	this->draw_obj(uarmu, 0, 3); // Undershirt
	this->draw_obj(uskin, -1, 3); // Dragon skin armor

	this->draw_obj(uleft, -2, 3.5);  // RingL
	this->draw_obj(uright, 2, 3.5); // RingR

	this->draw_obj(uwep, 2.5, 4.5);		// Weapon
	this->draw_obj(uswapwep, -2.5, 4.5); // Secondary weapon

	this->draw_obj(uarmg, -1, 5);  // Gloves
	this->draw_obj(uquiver, 1, 5);	// Quiver

	this->draw_obj(uarmf, 0, 7);  // Shoes (feet)
	this->draw_obj(uchain, 1, 7);	// chain
	this->draw_obj(uball, 2, 7);	// ball

	vulture_set_draw_region(0, 0, vulture_screen->w - 1, vulture_screen->h - 1);

	vulture_invalidate_region(abs_x, abs_y, w, h);

	return true;
}

void statusinv::draw_obj(obj *obj, float rx, float ry)
{
	// most code is copied from objitemwin.cpp
	int cx, cy, cw, ch;
	cw = tileSize;
	ch = tileSize;
	cx = x + w / 2 - tileSize / 2 + rx * (tileSize + 2) + 5;
	cy = y + ry * (tileSize + 2) + 30;

	char tmpstr[32];

	/* draw the tile itself */
	/* constrain the drawing region to the box for the object tile, so that large
	 * tiles don't overlap */
	vulture_set_draw_region(cx, cy, cx + ch, cy + ch);

	/* darken the background */
	vulture_fill_rect(cx + 2, cy + 2, cx + ch - 3, cy + ch - 3, CLR32_BLACK_A50);

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
	vulture_rect(cx + 2, cy + 2, cx + ch - 3, cy + ch - 3, CLR32_GRAY77);

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
