
#ifndef DEMOS_H
#define DEMOS_H

#include <retroflt.h>
#include <retroglu.h>

#define DEMO_VERTICES_SZ_MAX 1024
#define DEMO_FACES_SZ_MAX 1024
#define DEMO_MATERIALS_SZ_MAX 1024

#define DEMO_ZOOM_INC 0.1f
#define DEMO_TRANSLATE_INC 0.1f
#define DEMO_ROTATE_INC 10

#define DEMO_DEFAULT_OBJ "metalbug.obj"

#define DEMO_OBJ_NAME_SZ_MAX 255

#define SPRITE_X 0
#define SPRITE_Y 1

#define SPRITE_LL 0
#define SPRITE_LR 1
#define SPRITE_UR 2
#define SPRITE_UL 3

#define SPRITE_W 64
#define SPRITE_H 64

#define hash_mat_r( m ) (fmod( m[0] * 0.01f, 1.0f ) * 2)
#define hash_mat_g( m ) (fmod( m[1] * 0.01f, 1.0f ) * 2)
#define hash_mat_b( m ) (fmod( m[2] * 0.01f, 1.0f ) * 2)

struct DEMO_DATA {
   struct RETROGLU_VERTEX vertices[DEMO_VERTICES_SZ_MAX];
   int vertices_sz;
   struct RETROGLU_VERTEX vnormals[DEMO_VERTICES_SZ_MAX];
   int vnormals_sz;
   struct RETROGLU_VERTEX vtextures[DEMO_VERTICES_SZ_MAX];
   int vtextures_sz;
   struct RETROGLU_FACE faces[DEMO_FACES_SZ_MAX];
   int faces_sz;
   struct RETROGLU_MATERIAL materials[DEMO_MATERIALS_SZ_MAX];
   int materials_sz;
};

void demo_init_scene();
int demo_load_obj(
   const char* filename, struct RETROGLU_PARSER* parser, struct DEMO_DATA* data
);
void demo_dump_obj( const char* filename, struct DEMO_DATA* data );
MERROR_RETVAL demo_load_sprite( const char* filename, struct RETROGLU_SPRITE* sprite );
void draw_cube_iter( struct DEMO_DATA* data );
void draw_obj_iter( struct DEMO_DATA* data );
void draw_fp_iter( struct DEMO_DATA* data );
void draw_bmp_iter( struct DEMO_DATA* data );

#ifdef DEMOS_C

const char* gc_demo_names[] = {
   "cube",
   "obj",
   "fp",
   "bmp",
   ""
};

retroflat_loop_iter gc_demo_loops[] = {
   (retroflat_loop_iter)draw_cube_iter,
   (retroflat_loop_iter)draw_obj_iter,
   (retroflat_loop_iter)draw_fp_iter,
   (retroflat_loop_iter)draw_bmp_iter,
   NULL
};

int g_timer = 0;

char g_demo_dump_name[DEMO_OBJ_NAME_SZ_MAX] = "";
char g_demo_obj_name[DEMO_OBJ_NAME_SZ_MAX] = DEMO_DEFAULT_OBJ;

#else

extern const char* gc_demo_names[];
extern retroflat_loop_iter gc_demo_loops[];
extern int g_timer;
extern char g_demo_dump_name[];
extern char g_demo_obj_name[];

#endif /* DEMOS_C */

#endif /* !DEMOS_H */

