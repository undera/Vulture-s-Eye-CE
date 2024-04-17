/* NetHack may be freely redistributed.  See license for details. */

#ifndef _statusinv_h_
#define _statusinv_h_

#include <SDL.h>
#include "window.h"

class statusinv : public window
{
public:
	statusinv(window *p);
	virtual ~statusinv();
	virtual bool draw();
	virtual eventresult handle_timer_event(window *target, void *result, int time);
	virtual eventresult handle_mousemotion_event(window *target, void *result, int xrel, int yrel, int state);
	virtual eventresult handle_resize_event(window *target, void *result, int res_w, int res_h);

private:
	void draw_obj(obj *obj, int col, int row);
	int tileSize;
};

extern statusinv *stinv;

#endif
