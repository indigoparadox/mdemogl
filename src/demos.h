
#ifndef DEMOS_H
#define DEMOS_H

#include <retroflt.h>
#include <retroglu.h>

#define DEMO_VERTICES_SZ_MAX 1024
#define DEMO_FACES_SZ_MAX 1024
#define DEMO_ZOOM_INC 0.1f

#define hash_mat_r( m ) (fmod( m[0] * 0.01f, 1.0f ) * 2)
#define hash_mat_g( m ) (fmod( m[1] * 0.01f, 1.0f ) * 2)
#define hash_mat_b( m ) (fmod( m[2] * 0.01f, 1.0f ) * 2)

void draw_cube_iter( void* data );
void draw_tree_iter( void* data );

#ifdef DEMOS_C

const char* gc_demo_names[] = {
   "tree",
   ""
};

retroflat_loop_iter gc_demo_loops[] = {
   draw_tree_iter,
   NULL
};

int g_timer = 0;

#else

extern const char* gc_demo_names[];
extern retroflat_loop_iter gc_demo_loops[];
extern int g_timer;

#endif /* DEMOS_C */

#endif /* !DEMOS_H */

