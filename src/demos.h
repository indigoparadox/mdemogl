
#ifndef DEMOS_H
#define DEMOS_H

#include <retroflt.h>
#include <retroglu.h>

void draw_cube_iter( void* data );

#ifdef DEMOS_C

const char* gc_demo_names[] = {
   "cube",
   ""
};

retroflat_loop_iter gc_demo_loops[] = {
   draw_cube_iter,
   NULL
};

int g_timer = 0;

#else

extern const char* gc_demo_names[];
extern retroflat_loop_iter gc_demo_loops[];
extern int g_timer;

#endif /* DEMOS_C */

#endif /* !DEMOS_H */

