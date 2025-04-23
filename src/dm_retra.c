
#include "demos.h"
#include "poly.h"

MERROR_RETVAL setup_retroani( struct DEMO_RETROANI_DATA* data ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETRO3D_PROJ_ARGS args;

   /* Create the fire animation texture. */

   debug_printf( 1, "initializing fire cube..." );

   data->base.translate_z = mfix_from_f( -5.0f );

   data->bmp_fire = calloc( 1, sizeof( retroflat_blit_t ) );
   assert( NULL != data->bmp_fire );
   retval = retroflat_2d_create_bitmap(
      RETROANI_TILE_W, RETROANI_TILE_H, data->bmp_fire, 0 );
   maug_cleanup_if_not_ok();

   /*
   assert( RETROANI_TILE_W == retro3d_texture_w( data->bmp_fire ) );
   assert( RETROANI_TILE_H == retro3d_texture_h( data->bmp_fire ) );
   assert( NULL != data->bmp_fire->bytes_h );
   */

   retval = retroflat_2d_lock_bitmap( data->bmp_fire );
   maug_cleanup_if_not_ok();

   /* assert( NULL != data->bmp_fire->bytes ); */

   retroflat_2d_rect(
      data->bmp_fire, RETROFLAT_COLOR_BLACK, 0, 0,
      RETROANI_TILE_W, RETROANI_TILE_H,
      RETROFLAT_FLAGS_FILL );
   retroflat_2d_release_bitmap( data->bmp_fire );

   /* assert( NULL != data->bmp_fire->bytes_h ); */

   data->idx_fire = retroani_create(
      &(data->animations[0]), ANIMATIONS_MAX,
      RETROANI_TYPE_FIRE, RETROANI_FLAG_CLEANUP,
      0, 0,
      retroflat_2d_bitmap_w( data->bmp_fire ),
      retroflat_2d_bitmap_h( data->bmp_fire ) );

   retroani_set_target( data->animations, data->idx_fire, data->bmp_fire );

   /* Create the snow animation texture. */

   debug_printf( 1, "initializing snow cube..." );

   data->bmp_snow = calloc( 1, sizeof( retroflat_blit_t ) );
   assert( NULL != data->bmp_snow );
   retval = retroflat_2d_create_bitmap(
      RETROANI_TILE_W, RETROANI_TILE_H, data->bmp_snow, 0 );
   maug_cleanup_if_not_ok();

   retval = retroflat_2d_lock_bitmap( data->bmp_snow );
   maug_cleanup_if_not_ok();

   retrosoft_rect(
      data->bmp_snow, RETROFLAT_COLOR_BLACK, 0, 0,
      RETROANI_TILE_W, RETROANI_TILE_H,
      RETROFLAT_FLAGS_FILL );
   retroflat_2d_release_bitmap( data->bmp_snow );

   data->idx_snow = retroani_create(
      &(data->animations[0]), ANIMATIONS_MAX,
      RETROANI_TYPE_SNOW, RETROANI_FLAG_CLEANUP,
      0, 0,
      retroflat_2d_bitmap_w( data->bmp_snow ),
      retroflat_2d_bitmap_h( data->bmp_snow ) );

   retroani_set_target( data->animations, data->idx_snow, data->bmp_snow );

   /* Create the cube. */

   /* assert( NULL != data->bmp_fire->bytes_h ); */

   maug_mzero( &args, sizeof( struct RETRO3D_PROJ_ARGS ) );
   args.proj = RETRO3D_PROJ_FRUSTUM;
   args.rzoom = 0.5f;
   args.near_plane = 0.5f;
   args.far_plane = 100.0f;
   retro3d_init_projection( &args );

   data->base.rotate_x = mfix_from_i( 10 );
   data->base.rotate_y = mfix_from_i( 10 );

cleanup:

   return retval;
}

void draw_retroani_iter( struct DEMO_RETROANI_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   MERROR_RETVAL retval = MERROR_OK;

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_RIGHT:
      break;

   case RETROFLAT_KEY_LEFT:
      break;

   case RETROFLAT_KEY_ESC:
      retroani_stop_all( &(data->animations[0]), ANIMATIONS_MAX );
      retroflat_2d_destroy_bitmap( data->bmp_fire );
      free( data->bmp_fire );
      retroflat_2d_destroy_bitmap( data->bmp_snow );
      free( data->bmp_snow );
      retroflat_quit( 0 );
      goto end_func;

   case RETROFLAT_KEY_SPACE:
      break;

   default:
      break;
   }

   /* assert( NULL != data->bmp_fire->bytes_h ); */

   /* Drawing */

   retroflat_2d_lock_bitmap( data->bmp_fire );
   retroflat_2d_lock_bitmap( data->bmp_snow );
   retroani_frame( &(data->animations[0]), ANIMATIONS_MAX, 0 );
   retroflat_2d_release_bitmap( data->bmp_fire );
   retroflat_2d_release_bitmap( data->bmp_snow );

   /* assert( NULL != data->bmp_fire->bytes_h ); */

   retroflat_draw_lock( NULL );
   retro3d_scene_init();

   retro3d_scene_translate( 0, 0, data->base.translate_z );
   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );

   /*
   retval = retro3d_texture_activate( &(data->tex_cube), 0 );
   maug_cleanup_if_not_ok();

   poly_ortho_skybox(
      data->bmp_snow, RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE );

   retval = retro3d_texture_activate(
      &(data->tex_cube), RETRO3D_TEX_FLAG_DEACTIVATE );
   maug_cleanup_if_not_ok();
   */

   retro3d_scene_translate( 0, 0, data->base.translate_z );
   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );

   retval = retro3d_texture_activate( data->bmp_fire, 0 );
   maug_cleanup_if_not_ok();

   poly_cube_tex(
      data->bmp_fire, mfix_from_i( 1 ),  mfix_from_i( 1 ),
      RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE,
      RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE );

   retval = retro3d_texture_activate(
      data->bmp_fire, RETRO3D_TEX_FLAG_DEACTIVATE );
   maug_cleanup_if_not_ok();

   demo_draw_fps();

   retro3d_scene_complete();
   retroflat_draw_release( NULL );

   if( data->next_rotate_ms < retroflat_get_ms() ) {
      data->base.rotate_y += mfix_from_i( 5 );
      data->next_rotate_ms = retroflat_get_ms() + 50;
   }

cleanup:

   if( retval ) {
      retroflat_quit( retval );
   }

end_func:
   return;
}

