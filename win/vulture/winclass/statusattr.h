/* NetHack may be freely redistributed.  See license for details. */

#ifndef _statusattr_h_
#define _statusattr_h_

#include <SDL.h>
#include "window.h"


#define vulture_free_line(x1,y1,x2,y2,cindex) vulture_free_line_surface(vulture_screen,x1,y1,x2,y2,cindex)
#define vulture_triangle(x1,y1,x2,y2,x3,y3,cindex,fill) vulture_triangle_surface(vulture_screen,x1,y1,x2,y2,x3,y3,cindex,fill)

extern void vulture_free_line_surface(SDL_Surface *,int, int, int, int, Uint32);
extern void vulture_triangle_surface(SDL_Surface *,int, int, int, int, int, int, Uint32, bool);
extern void vulture_fill_triangle_surface(SDL_Surface *,int, int, int, int, int, int, Uint32);

class statusattr : public window
{
public:
	statusattr(window *p);
	virtual ~statusattr();
	virtual bool draw();
    virtual eventresult handle_timer_event(window *target, void *result, int time);
    virtual eventresult handle_mousemotion_event(window *target, void *result, int xrel, int yrel, int state);
	virtual eventresult handle_resize_event(window *target, void *result, int res_w, int res_h);

private:
    //virtual void calculatePoints(float points[][2], float centerX, float centerY, float radius, float *values, int numValues);
};

extern statusattr *stattr;

#endif
