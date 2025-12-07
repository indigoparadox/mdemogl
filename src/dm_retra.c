
#include "demos.h"
#include "poly.h"

MERROR_RETVAL setup_retroani( struct DEMO_RETROANI_DATA* data ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETRO3D_PROJ_ARGS args;

   retval = demo_setup_win( &(data->base), RETROFLAT_COLOR_WHITE );
   maug_cleanup_if_not_ok();

   /* Create the fire cube. */

   debug_printf( 1, "initializing fire cube..." );

   data->bmp_fire = calloc( 1, sizeof( retroflat_blit_t ) );
   assert( NULL != data->bmp_fire );
   retval = retroflat_2d_create_bitmap(
      RETROFLAT_TILE_W, RETROFLAT_TILE_H, data->bmp_fire,
      RETROFLAT_FLAGS_OPAQUE );
   maug_cleanup_if_not_ok();

   retval = retroflat_2d_lock_bitmap( data->bmp_fire );
   maug_cleanup_if_not_ok();

   retroflat_2d_rect(
      data->bmp_fire, RETROFLAT_COLOR_BLACK, 0, 0,
      RETROFLAT_TILE_W, RETROFLAT_TILE_H,
      RETROFLAT_FLAGS_FILL );
   retroflat_2d_release_bitmap( data->bmp_fire );

   /* Create the fire animation texture. */
   data->idx_fire = retroani_create(
      &(data->animations),
      RETROANI_TYPE_FIRE, RETROANI_FLAG_CLEANUP,
      0, 0,
      retroflat_2d_bitmap_w( data->bmp_fire ),
      retroflat_2d_bitmap_h( data->bmp_fire ) );
   if( 0 > data->idx_snow ) {
      error_printf( "invalid fire animation index!" );
   } else {
      debug_printf( 3, "fire animation index: " SSIZE_T_FMT, data->idx_fire );
   }

   retroani_set_target( &(data->animations), data->idx_fire, data->bmp_fire );

   /* Create the snow cube. */

   debug_printf( 1, "initializing snow cube..." );

   data->bmp_snow = calloc( 1, sizeof( retroflat_blit_t ) );
   assert( NULL != data->bmp_snow );
   retval = retroflat_2d_create_bitmap(
      RETROFLAT_TILE_W, RETROFLAT_TILE_H, data->bmp_snow,
      RETROFLAT_FLAGS_OPAQUE );
   maug_cleanup_if_not_ok();

   retval = retroflat_2d_lock_bitmap( data->bmp_snow );
   maug_cleanup_if_not_ok();

   retrosoft_rect(
      data->bmp_snow, RETROFLAT_COLOR_BLACK, 0, 0,
      RETROFLAT_TILE_W, RETROFLAT_TILE_H,
      RETROFLAT_FLAGS_FILL );
   retroflat_2d_release_bitmap( data->bmp_snow );

   /* Create the snow animation texture. */
   data->idx_snow = retroani_create(
      &(data->animations),
      RETROANI_TYPE_SNOW, RETROANI_FLAG_CLEANUP,
      0, 0,
      retroflat_2d_bitmap_w( data->bmp_snow ),
      retroflat_2d_bitmap_h( data->bmp_snow ) );
   if( 0 > data->idx_snow ) {
      error_printf( "invalid snow animation index!" );
   } else {
      debug_printf( 3, "snow animation index: " SSIZE_T_FMT, data->idx_snow );
   }

   retroani_set_target( &(data->animations), data->idx_snow, data->bmp_snow );

   /* Setup projection. */

   maug_mzero( &args, sizeof( struct RETRO3D_PROJ_ARGS ) );
   args.proj = RETRO3D_PROJ_FRUSTUM;
   args.rzoom = 0.5f;
   args.near_plane = 0.5f;
   args.far_plane = 100.0f;
   retro3d_init_projection( &args );

   data->base.translate_z = mfix_from_f( -8.0f );
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
      retroani_stop_all( &(data->animations) );
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
   retroani_frame( &(data->animations), 0 );
   retroflat_2d_release_bitmap( data->bmp_fire );
   retroflat_2d_release_bitmap( data->bmp_snow );

   retroflat_draw_lock( NULL );
   retro3d_scene_init();

   /* Draw the skybox. */
   /* The skybox has an open face towards the camera, so we're not rotating it.
    */

   poly_ortho_skybox(
      data->bmp_snow, RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE );

   /* Draw the cube. */

   /* Rotate the cube! */
   retro3d_scene_translate( 0, 0, data->base.translate_z );
   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );

   poly_cube_tex(
      data->bmp_fire, mfix_from_i( 1 ),  mfix_from_i( 1 ),
      RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE,
      RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE );
   maug_cleanup_if_not_ok();

   demo_draw_fps( &(data->base) );

   retrowin_redraw_win_stack( &(data->base.win) );

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

