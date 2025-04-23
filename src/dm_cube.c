
#include "demos.h"
#include "poly.h"

MERROR_RETVAL setup_cube( struct DEMO_CUBE_DATA* data ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETRO3D_PROJ_ARGS args;

   /* Create the fire animation texture. */

   debug_printf( 1, "initializing fire cube..." );

   maug_mzero( &args, sizeof( struct RETRO3D_PROJ_ARGS ) );
   args.proj = RETRO3D_PROJ_FRUSTUM;
   args.rzoom = 0.5f;
   args.near_plane = 0.5f;
   args.far_plane = 10.0f;
   retro3d_init_projection( &args );

   retro3d_check_errors( "dump" );
   retval = retroflat_2d_load_bitmap(
      DEMO_TEX_WATER, &(data->tex_cube), 0 );
   maug_cleanup_if_not_ok();

   data->base.rotate_x = mfix_from_i( 10 );
   data->base.rotate_y = mfix_from_i( 10 );
   data->base.translate_z = mfix_from_f( -5.0f );

   data->tx_scale = mfix_from_i( 1 );

   retval = demo_setup_win( &(data->base) );
   maug_cleanup_if_not_ok();

   /*
   if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
   }
   */

cleanup:

   return retval;
}

/* === */

void draw_cube_iter( struct DEMO_CUBE_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   MERROR_RETVAL retval = MERROR_OK;

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;

   case RETROFLAT_KEY_UP:
      data->base.translate_z += mfix_from_i( 1 );
      break;

   case RETROFLAT_KEY_DOWN:
      data->base.translate_z -= mfix_from_i( 1 );
      break;

   case RETROFLAT_KEY_RIGHT:
      data->tx_scale += 100;
      debug_printf( 1, "tx_scale: %d", data->tx_scale );
      break;

   case RETROFLAT_KEY_LEFT:
      data->tx_scale -= 100;
      debug_printf( 1, "tx_scale: %d", data->tx_scale );
      break;

   case RETROFLAT_KEY_SPACE:
      data->pause = ~data->pause;
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );
   retro3d_scene_init();

   retro3d_scene_translate( 0, 0, data->base.translate_z );
   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );

   poly_cube_tex(
      &(data->tex_cube), mfix_from_i( 1 ), data->tx_scale,
      RETROFLAT_COLOR_RED, RETROFLAT_COLOR_GREEN, RETROFLAT_COLOR_BLUE,
      RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_CYAN, RETROFLAT_COLOR_MAGENTA );

   demo_draw_fps();

   retrowin_redraw_win_stack( &(data->base.win) );

   retro3d_scene_complete();
   retroflat_draw_release( NULL );

   if( !(data->pause) ) {
      data->base.rotate_y += mfix_from_i( 5 );
   }

cleanup:

   assert( MERROR_OK == retval );

   return;
}


