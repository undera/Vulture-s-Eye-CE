/* NetHack may be freely redistributed.  See license for details. */

#ifndef _statusbars_h_
#define _statusbars_h_

#include <SDL.h>
#include "window.h"


class statusbars : public window
{
public:
	statusbars(window *p);
	virtual ~statusbars();
	virtual bool draw();
	virtual eventresult handle_timer_event(window* target, void* result, int time);
	virtual eventresult handle_mousemotion_event(window* target, void* result, 
	                                             int xrel, int yrel, int state);
	virtual eventresult handle_resize_event(window* target, void* result, int res_w, int res_h);

private:
	void draw_bar(int index, std::string label, float lvlMain, float lvlSecondary, Uint32 color);
};

extern statusbars *stbars;

#endif
