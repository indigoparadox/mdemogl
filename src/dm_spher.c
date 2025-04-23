
#include "demos.h"
#include "poly.h"

MERROR_RETVAL setup_sphere( struct DEMO_SPHERE_DATA* data ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETRO3D_PROJ_ARGS args;

   maug_mzero( &args, sizeof( struct RETRO3D_PROJ_ARGS ) );
   args.proj = RETRO3D_PROJ_ORTHO;
   args.rzoom = 10.0f;
   args.near_plane = -100.0f;
   args.far_plane = 100.0f;
   retro3d_init_projection( &args );

   retval = retroflat_2d_load_bitmap( DEMO_TEX_TILE, &(data->tex_walls), 0 );
   maug_cleanup_if_not_ok();

   /* Start spin. */
   data->rotate_y_inc = mfix_from_i( 5 );

   /* Start bounce. */
   data->translate_x_inc = mfix_from_f( 0.2f );
   data->translate_y_inc = mfix_from_f( 0.2f );

   data->base.translate_z = mfix_from_f( -5.0f );

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

void draw_sphere_iter( struct DEMO_SPHERE_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   MERROR_RETVAL retval = MERROR_OK;

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_RIGHT:
      data->base.rotate_y += 10;
      debug_printf( 1, "rotate_y: %d", data->base.rotate_y );
      break;

   case RETROFLAT_KEY_LEFT:
      data->base.rotate_y -= 10;
      debug_printf( 1, "rotate_y: %d", data->base.rotate_y );
      break;

   case RETROFLAT_KEY_UP:
      data->base.rotate_x += mfix_from_i( 10 );
      debug_printf( 1, "rotate_x: %d", data->base.rotate_x );
      break;

   case RETROFLAT_KEY_DOWN:
      data->base.rotate_x -= mfix_from_i( 10 );
      debug_printf( 1, "rotate_x: %d", data->base.rotate_x );
      break;

   case RETROFLAT_KEY_ENTER:
      data->translate_x_inc = 0;
      data->translate_y_inc = 0;
      break;

   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );
   retro3d_scene_init();

   /* retval = retro3d_texture_activate( &(data->tex_walls), 0 ); */
   poly_ortho_skybox( NULL, RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_GRAY );
   /* retval = retro3d_texture_activate(
      &(data->tex_walls), RETRO3D_TEX_FLAG_DEACTIVATE ); */

   /* retro3d_draw_window( &(data->tex_walls), 10, 10 ); */

   /* Draw sphere from display list. */

   retro3d_scene_translate(
      data->base.translate_x, data->base.translate_y, data->base.translate_z );
   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );
   poly_sphere_checker( RETROFLAT_COLOR_RED, RETROFLAT_COLOR_WHITE );

   demo_draw_fps();

   retrowin_redraw_win_stack( &(data->base.win) );

   retro3d_scene_complete();
   retroflat_draw_release( NULL );

   data->base.rotate_y += data->rotate_y_inc;

   /* X bounce. */
   data->base.translate_x += data->translate_x_inc;
   if(
      mfix_from_f( 9.0f ) <= data->base.translate_x ||
      mfix_from_f( -9.0f ) >= data->base.translate_x
   ) {
      data->translate_x_inc *= -1;
      data->base.translate_x += data->translate_x_inc;
   }

   /* Y bounce. */
   data->base.translate_y += data->translate_y_inc;
   if(
      mfix_from_f( 6.0f ) <= data->base.translate_y ||
      mfix_from_f( -6.0f ) >= data->base.translate_y
   ) {
      data->translate_y_inc *= -1;
      data->base.translate_y += data->translate_y_inc;
   }

cleanup:

   assert( MERROR_OK == retval );

   return;
}

