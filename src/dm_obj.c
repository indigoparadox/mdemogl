
#include "demos.h"

void draw_obj_iter( struct DEMO_OBJ_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   struct RETRO3D_PROJ_ARGS args;
   MERROR_RETVAL retval = MERROR_OK;

   if( 0 == data->base.init ) {
      retval = retro3dp_parse_obj_file( g_demo_obj_name, NULL, &(data->obj) );
      maug_cleanup_if_not_ok();

      data->base.rotate_y = mfix_from_i( 10 );
      data->base.rotate_z = 0;
      data->base.translate_z = mfix_from_f( -5.0f );

      /*
      assert( 0 < data->obj.vertices_sz );
      assert( 0 < data->obj.faces_sz );
      assert( 0 < data->obj.materials_sz );
      */

      if( '\0' != g_demo_dump_name[0] ) {
         demo_dump_obj( g_demo_dump_name, data );
         debug_printf( 3, "demo data dumped to %s", g_demo_dump_name );
      }

      maug_mzero( &args, sizeof( struct RETRO3D_PROJ_ARGS ) );
      args.proj = RETRO3D_PROJ_FRUSTUM;
      args.rzoom = 1.0f;
      args.near_plane = 0.5f;
      args.far_plane = 10.0f;
      retro3d_init_projection( &args );

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
#if 0
   case RETROFLAT_KEY_Q:
      tx += DEMO_ZOOM_INC;
      debug_printf( 3, "%f, %f, %f", tx, ty, tz );
      break;

   case RETROFLAT_KEY_A:
      tx -= DEMO_ZOOM_INC;
      debug_printf( 3, "%f, %f, %f", tx, ty, tz );
      break;

   case RETROFLAT_KEY_W:
      ty += DEMO_ZOOM_INC;
      debug_printf( 3, "%f, %f, %f", tx, ty, tz );
      break;

   case RETROFLAT_KEY_S:
      ty -= DEMO_ZOOM_INC;
      debug_printf( 3, "%f, %f, %f", tx, ty, tz );
      break;
#endif

   case RETROFLAT_KEY_UP:
      data->base.translate_z += DEMO_ZOOM_INC;
      debug_printf( 3, "%d, %d, %d",
         data->base.translate_x, data->base.translate_y,
         data->base.translate_z );
      break;

   case RETROFLAT_KEY_DOWN:
      data->base.translate_z -= DEMO_ZOOM_INC;
      debug_printf( 3, "%d, %d, %d",
         data->base.translate_x, data->base.translate_y,
         data->base.translate_z );
      break;

   case RETROFLAT_KEY_RIGHT:
      data->base.rotate_z += DEMO_ROTATE_INC;
      debug_printf( 3, "%d, %d, %d",
         data->base.translate_x, data->base.translate_y,
         data->base.translate_z );
      break;

   case RETROFLAT_KEY_LEFT:
      data->base.rotate_z -= DEMO_ROTATE_INC;
      debug_printf( 3, "%d", data->base.rotate_z );
      break;

   case RETROFLAT_KEY_ESC:
      retro3dp_destroy_obj( &(data->obj) );
      retroflat_quit( 0 );
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );
   retro3d_scene_init();

   /* retro3d_scene_scale( mfix_from_f( 0.5f ), mfix_from_f( 0.5f ), mfix_from_f( 0.5f ) ); */
   retro3d_scene_translate(
      data->base.translate_x, data->base.translate_y, data->base.translate_z );

   retro3d_scene_rotate( 0, data->base.rotate_y, data->base.rotate_z );

   retro3d_draw_model( &(data->obj) );

   demo_draw_fps();

   retro3d_scene_complete();
   retroflat_draw_release( NULL );

   data->base.rotate_y += mfix_from_i( 5 );

cleanup:

   if( retval ) {
      retro3dp_destroy_obj( &(data->obj) );
      retroflat_quit( retval );
   }

}

