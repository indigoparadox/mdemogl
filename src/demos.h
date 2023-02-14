
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

#define DEMO_SPHERE_INC_XY (RETROFLAT_PI / 10)
#define DEMO_SPHERE_INC_XZ (RETROFLAT_PI / 10)

#define DEMO_WATER_SHEET_DEPTH 20.0f
#define DEMO_WATER_SHEET_WIDTH 40.0f
#define DEMO_WATER_SHEET_X_ITER 0.1f
#define DEMO_WATER_RING_R_ITER 0.2f
#define DEMO_WATER_RING_A_ITER 0.2f
#define DEMO_WATER_RING_RADIUS 3.0f

#define SPRITE_X 0
#define SPRITE_Y 1

#define SPRITE_LL 0
#define SPRITE_LR 1
#define SPRITE_UR 2
#define SPRITE_UL 3

#define SPRITE_W 64
#define SPRITE_H 64

#define DEMO_MAP_TILES_SZ_MAX 10
#define DEMO_MAP_W 40
#define DEMO_MAP_H 40

#define hash_mat_r( m ) (fmod( m[0] * 0.01f, 1.0f ) * 2)
#define hash_mat_g( m ) (fmod( m[1] * 0.01f, 1.0f ) * 2)
#define hash_mat_b( m ) (fmod( m[2] * 0.01f, 1.0f ) * 2)

struct DEMO_CUBE_DATA {
   int init;
   GLint cube_list;
   int rotate_x;
   int rotate_y;
};

struct DEMO_SPHERE_DATA {
   int init;
   GLint sphere_list;
   GLint skybox_list;
   int rotate_x;
   int rotate_x_inc;
   int rotate_y;
   int rotate_y_inc;
   float translate_x;
   float translate_x_inc;
   float translate_y;
   float translate_y_inc;
   float translate_z;
};

struct DEMO_OBJ_DATA {
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
   GLint obj_list;
};

struct DEMO_SPRITE_DATA {
   int init;
   struct RETROGLU_SPRITE sprite;
   struct RETROGLU_TILE tiles[DEMO_MAP_TILES_SZ_MAX];
   uint8_t map[DEMO_MAP_H][DEMO_MAP_W];
   GLint sprite_list[2];
   int tex_countdown;
   int tex_frame_idx;
};

struct DEMO_FP_DATA {
   int init;
};

struct DEMO_WATER_DATA {
   int init;
   int pattern;
   float freq_mod;
   float amp_mod;
   float translate_x;
   float translate_y;
   float translate_z;
   int rotate_x;
   int rotate_y;
   float peak_offset;
   GLint well_list;
   GLint skybox_list;
};

void demo_init_scene( uint8_t flags );
void demo_init_projection( uint8_t demo_proj, float zoom );
int demo_load_obj(
   const char* filename, struct RETROGLU_PARSER* parser,
   struct DEMO_OBJ_DATA* data
);
void demo_dump_obj( const char* filename, struct DEMO_OBJ_DATA* data );
MERROR_RETVAL demo_load_sprite( const char* filename, struct RETROGLU_SPRITE* sprite );

#define DEMOS_LIST( f ) \
   f( cube, struct DEMO_CUBE_DATA ) \
   f( sphere, struct DEMO_SPHERE_DATA ) \
   f( obj, struct DEMO_OBJ_DATA ) \
   f( sprite, struct DEMO_SPRITE_DATA ) \
   f( water, struct DEMO_WATER_DATA )

#define DEMOS_LIST_PROTOS( name, data_struct ) \
   void draw_ ## name ## _iter( data_struct* data );

DEMOS_LIST( DEMOS_LIST_PROTOS )

#ifdef DEMOS_C

#define DEMOS_LIST_NAMES( name, data_struct ) #name,

const char* gc_demo_names[] = {
   DEMOS_LIST( DEMOS_LIST_NAMES )
   ""
};

#define DEMOS_LIST_LOOPS( name, data_struct ) \
   (retroflat_loop_iter)draw_ ## name ## _iter,

retroflat_loop_iter gc_demo_loops[] = {
   DEMOS_LIST( DEMOS_LIST_LOOPS )
   NULL
};

#define DEMOS_LIST_DATA_SZ( name, data_struct ) sizeof( data_struct ),

size_t gc_demo_data_sz[] = {
   DEMOS_LIST( DEMOS_LIST_DATA_SZ )
   0
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
extern size_t gc_demo_data_sz[];

#endif /* DEMOS_C */

#endif /* !DEMOS_H */

