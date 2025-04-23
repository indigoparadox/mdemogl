
#include "demos.h"

void draw_water_iter( struct DEMO_WATER_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   struct RETRO3D_PROJ_ARGS args;
   MERROR_RETVAL retval = MERROR_OK;

   if( 0 == data->base.init ) {

      retval = retroflat_2d_load_bitmap(
         DEMO_TEX_WATER, &(data->tex_water), 0 );
      maug_cleanup_if_not_ok();

      maug_mzero( &args, sizeof( struct RETRO3D_PROJ_ARGS ) );
      args.proj = RETRO3D_PROJ_FRUSTUM;
      args.rzoom = 1.0f;
      /* args.near_plane = 0.5f; */
      args.near_plane = 1.0f;
      args.far_plane = 100.0f;
      retro3d_init_projection( &args );

      /* TODO: Add CLI option to select pattern. */
      data->pattern = 0;
      data->freq_mod = 7.0f;
      data->amp_mod = 0.1f;
      if( 1 == data->pattern ) {
         data->base.translate_y = mfix_from_f( -4.0f );
         data->base.translate_z = mfix_from_f( -4.0f );

      } else {
         data->base.translate_y = mfix_from_f( -8.0f );
         data->base.translate_z = mfix_from_f( -4.0f );
         data->base.rotate_x = mfix_from_i( 20 );
      }

      /*
      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }
      */

      data->base.init = 1;
   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_UP:
      data->base.translate_y += mfix_from_f( 0.1f );
      debug_printf( 3, "ty: %d", data->base.translate_y );
      break;

   case RETROFLAT_KEY_DOWN:
      data->base.translate_y -= mfix_from_f( 0.1f );
      debug_printf( 3, "ty: %d", data->base.translate_y );
      break;

#if 0
   case RETROFLAT_KEY_PGUP:
      data->rotate_x += 5;
      debug_printf( 3, "rx: %d", data->rotate_x );
      break;

   case RETROFLAT_KEY_PGDN:
      data->rotate_x -= 5;
      debug_printf( 3, "rx: %d", data->rotate_x );
      break;

   case RETROFLAT_KEY_S:
      data->translate_z += 0.1f;
      debug_printf( 3, "tz: %f", data->translate_z );
      break;

   case RETROFLAT_KEY_W:
      data->translate_z -= 0.1f;
      debug_printf( 3, "tz: %f", data->translate_z );
      break;
#endif

   case RETROFLAT_KEY_RIGHT:
      data->freq_mod += 0.5f;
      debug_printf( 3, "freq_mod: %f", data->freq_mod );
      break;

   case RETROFLAT_KEY_LEFT:
      data->freq_mod -= 0.5f;
      debug_printf( 3, "freq_mod: %f", data->freq_mod );
      break;

   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );
   retro3d_scene_init();

   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );
   retro3d_scene_translate(
      data->base.translate_x, data->base.translate_y, data->base.translate_z );

   poly_water_skybox();

   if( 0 == data->pattern ) {

      retval = retro3d_texture_activate( &(data->tex_water), 0 );
      maug_cleanup_if_not_ok();

      poly_water_sheet(
         &(data->tex_water),
         RETROFLAT_COLOR_WHITE, DEMO_WATER_SHEET_WIDTH, DEMO_WATER_SHEET_DEPTH,
         DEMO_WATER_SHEET_X_ITER,
         data->freq_mod, data->amp_mod, data->peak_offset );

      retval = retro3d_texture_activate(
         &(data->tex_water), RETRO3D_TEX_FLAG_DEACTIVATE );
      maug_cleanup_if_not_ok();

   } else {

      /*
      poly_well( RETROGLU_COLOR_GRAY, DEMO_WATER_RING_RADIUS + 0.75f,
         DEMO_WATER_RING_RADIUS, 2.0f, DEMO_WATER_RING_A_ITER );

      poly_water_ring(
         RETROGLU_COLOR_CYAN, 1.0f,
         DEMO_WATER_RING_RADIUS, DEMO_WATER_RING_R_ITER,
         DEMO_WATER_RING_A_ITER, 0.3f, data->freq_mod, data->amp_mod,
         data->peak_offset );
      */
   }

   demo_draw_fps();

   retro3d_scene_complete();
   retroflat_draw_release( NULL );

   data->peak_offset -= 0.1f;

cleanup:

   assert( MERROR_OK == retval );

   return;
}


