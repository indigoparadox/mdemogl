
#include <stdio.h>
#include <math.h>

#define DEMOS_C
#include "demos.h"

#include "poly.h"

int32_t g_frames_per_sec = 0;
int32_t g_frames_since_last = 0;
uint32_t g_last_frame_ms = 0;

void demo_draw_fps() {
   float aspect_ratio = 0;
   char overlay_str[DEMOS_OVERLAY_SZ_MAX + 1];
   uint32_t now_ms = 0;

   /* Switch to projection setup. */
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();

#ifndef RETROGLU_NO_LIGHTING
   /* Lighting makes overlay text hard to see. */
   glDisable( GL_LIGHTING );
#endif /* !RETROGLU_NO_LIGHTING */

   /* Use ortho for overlay. */
   glLoadIdentity();
   aspect_ratio = (float)retroflat_screen_w() / (float)retroflat_screen_h();
   glOrtho( -1.0f * aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0, 10.0f );

   /* Generate FPS string. */
   now_ms = retroflat_get_ms();

   g_frames_since_last++;
   if( 0 == g_last_frame_ms || g_last_frame_ms + 1000 <= now_ms ) {
      g_last_frame_ms = now_ms;
      g_frames_per_sec = g_frames_since_last;
      g_frames_since_last = 0;
   }
   memset( overlay_str, '\0', DEMOS_OVERLAY_SZ_MAX + 1 );
   maug_snprintf( overlay_str, DEMOS_OVERLAY_SZ_MAX,
      "FPS: %d", g_frames_per_sec );
   /* TODO: Display FPS using new font system. */
   /*
   retroglu_string(
      -1.0 * aspect_ratio, -1.0, 0, RETROGLU_COLOR_WHITE,
      overlay_str, DEMOS_OVERLAY_SZ_MAX, "", 0 );
   */
   
   /* Restore modelview. */
   glPopMatrix();
   glMatrixMode( GL_MODELVIEW );
}

void demo_dump_obj( const char* filename, struct DEMO_OBJ_DATA* data ) {
   FILE* obj_file = NULL;
   uint32_t i = 0, /* Index in file buffer, so long. */
      j = 0;

   /* TODO: Fix me to work with new obj parser! */
#if 0
   /* Dump */
   obj_file = fopen( filename, "w" );
   assert( NULL != obj_file );
   for( i = 0 ; data->obj.vertices_sz > i ; i++ ) {
      fprintf( obj_file, "v %f %f %f\n",
         data->obj.vertices[i].x,
         data->obj.vertices[i].y,
         data->obj.vertices[i].z );
   }

   fprintf( obj_file, "\n" );
   
   for( i = 0 ; data->obj.vnormals_sz > i ; i++ ) {
      fprintf( obj_file, "vn %f %f %f\n",
         data->obj.vnormals[i].x,
         data->obj.vnormals[i].y,
         data->obj.vnormals[i].z );
   }

   for( i = 0 ; data->obj.faces_sz > i ; i++ ) {
      if(
         0 == i ||
         data->obj.faces[i].material_idx != 
            data->obj.faces[i - 1].material_idx
      ) {
         fprintf( obj_file, "\nusemtl %s\n\n",
            data->obj.materials[data->obj.faces[i].material_idx].name );
      }
      fprintf( obj_file, "f " );
      for( j = 0 ; data->obj.faces[i].vertex_idxs_sz > j ; j++ ) {
         fprintf( obj_file, "%d/%d/%d",
            data->obj.faces[i].vertex_idxs[j],
            data->obj.faces[i].vtexture_idxs[j],
            data->obj.faces[i].vnormal_idxs[j] );
         if( j + 1 < data->obj.faces[i].vertex_idxs_sz ) {
            fprintf( obj_file, " " );
         }
      }
      fprintf( obj_file, "\n" );
   }

   fclose( obj_file );
   obj_file = NULL;
#endif
}

void demo_dump_tex( struct RETROFLAT_BITMAP* tex_bmp ) {
   size_t x, y;

   printf( "\n" );
   for( y = 0 ; tex_bmp->tex.h > y ; y++ ) {
      for( x = 0 ; tex_bmp->tex.w > x ; x++ ) {
         printf( "%02x|",
            tex_bmp->tex.bytes[
               ((y * tex_bmp->tex.w) * 4) + (x * 4)] );
         printf( "%02x|",
            tex_bmp->tex.bytes[
               ((y * tex_bmp->tex.w) * 4) + (x * 4) + 1] );
         printf( "%02x|",
            tex_bmp->tex.bytes[
               ((y * tex_bmp->tex.w) * 4) + (x * 4) + 2] );
         printf( "%02x ",
            tex_bmp->tex.bytes[
               ((y * tex_bmp->tex.w) * 4) + (x * 4) + 3] );
      }
      printf( "\n" );
   }
   printf( "\n" );
}

MERROR_RETVAL demo_load_sprite(
   const char* filename, struct RETROGLU_SPRITE* sprite
) {
   MERROR_RETVAL retval = 0;

   retval = retroflat_load_bitmap( filename, &(sprite->texture), 0 );
   maug_cleanup_if_not_ok();

   retroglu_set_sprite_color( sprite, RETROGLU_COLOR_WHITE );

cleanup:
   return retval;
}

/* === */

void draw_cube_iter( struct DEMO_CUBE_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   struct RETRO3D_PROJ_ARGS args;

   if( !data->base.init ) {

      args.proj = RETRO3D_PROJ_FRUSTUM;
      args.rzoom = 0.5f;
      args.near_plane = 0.5f;
      args.far_plane = 10.0f;
      retro3d_init_projection( &args );

      data->base.rotate_x = 10;
      data->base.rotate_y = 10;
      data->base.translate_z = mfix( -5.0f );

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->base.init = 1;
   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );
   retro3d_scene_init();

   retro3d_scene_translate( 0, 0, data->base.translate_z );
   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );

   poly_cube(
      100,
      RETROFLAT_COLOR_RED, RETROFLAT_COLOR_GREEN, RETROFLAT_COLOR_BLUE,
      RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_CYAN, RETROFLAT_COLOR_MAGENTA );

   demo_draw_fps();

   retro3d_scene_complete();
   retroflat_draw_release( NULL );

   data->base.rotate_y += 5;
}

/* === */

void draw_sphere_iter( struct DEMO_SPHERE_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   struct RETRO3D_PROJ_ARGS args;
   const float light_pos[] = { 6.0f, 6.0f, 10.0f, 1.0f };

   if( !data->base.init ) {

#ifndef DEMOS_NO_LISTS
      /* Create sphere display list. */
      data->sphere_list = glGenLists( 1 );
      glNewList( data->sphere_list, GL_COMPILE );
      poly_sphere_checker( RETROFLAT_COLOR_RED, RETROFLAT_COLOR_WHITE, 1.0f );
      glEndList();

      /* Create Skybox display list. */
      data->skybox_list = glGenLists( 1 );
      glNewList( data->skybox_list, GL_COMPILE );
      poly_ortho_skybox( RETROGLU_COLOR_WHITE, NULL );
      glEndList();
#endif /* DEMOS_NO_LISTS */

      args.proj = RETRO3D_PROJ_ORTHO;
      args.rzoom = 10.0f;
      args.near_plane = -100.0f;
      args.far_plane = 100.0f;
      retro3d_init_projection( &args );

#ifndef DEMOS_NO_LIGHTS
      glEnable( GL_LIGHT0 );
      glEnable( GL_COLOR_MATERIAL );
#endif /* !DEMOS_NO_LIGHTS */

      /* Start spin. */
      data->rotate_y_inc = 5;

      /* Start bounce. */
      data->translate_x_inc = 0.2f;
      data->translate_y_inc = 0.2f;

      data->base.translate_z = -5.0f;

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->base.init = 1;
   }

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
      data->base.rotate_x += 10;
      debug_printf( 1, "rotate_x: %d", data->base.rotate_x );
      break;

   case RETROFLAT_KEY_DOWN:
      data->base.rotate_x -= 10;
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

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

#ifndef DEMOS_NO_LIGHTS
   glEnable( GL_NORMALIZE );
   glEnable( GL_LIGHTING );
   glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
#endif /* !DEMOS_NO_LIGHTS */

   glPushMatrix();

   glScalef( 8.0f, 6.0f, 1.0f );

#ifdef DEMOS_NO_LISTS
   poly_ortho_skybox( RETROGLU_COLOR_WHITE, NULL );
#else
   glCallList( data->skybox_list );
#endif /* DEMOS_NO_LISTS */

   glPopMatrix();

   /* Draw sphere from display list. */

   glPushMatrix();

   retro3d_scene_translate(
      data->base.translate_x, data->base.translate_y, data->base.translate_z );
   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );

#ifdef DEMOS_NO_LISTS
   poly_sphere_checker( RETROGLU_COLOR_RED, RETROGLU_COLOR_WHITE, 1.0f );
#else
   glCallList( data->sphere_list );
#endif /* DEMOS_NO_LISTS */

   glPopMatrix();

   demo_draw_fps();

   retroflat_draw_release( NULL );

   data->base.rotate_y += data->rotate_y_inc;

   /* X bounce. */
   data->base.translate_x += data->translate_x_inc;
   if( 9 <= data->base.translate_x || -9 >= data->base.translate_x ) {
      data->translate_x_inc *= -1;
      data->base.translate_x += data->translate_x_inc;
   }

   /* Y bounce. */
   data->base.translate_y += data->translate_y_inc;
   if( 6 <= data->base.translate_y || -6 >= data->base.translate_y ) {
      data->translate_y_inc *= -1;
      data->base.translate_y += data->translate_y_inc;
   }
}

#ifndef DEMOS_NO_FILES

void draw_obj_iter( struct DEMO_OBJ_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   struct RETRO3D_PROJ_ARGS args;
   MERROR_RETVAL retval = MERROR_OK;

   if( 0 == data->base.init ) {
      retval = retro3dp_parse_obj_file( g_demo_obj_name, NULL, &(data->obj) );
      maug_cleanup_if_not_ok();

      data->base.rotate_y = 10;
      data->base.rotate_z = 0;
      data->base.translate_z = mfix( -5.0f );

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

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

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

   /* retro3d_scene_scale( mfix( 0.5f ), mfix( 0.5f ), mfix( 0.5f ) ); */
   retro3d_scene_translate(
      data->base.translate_x, data->base.translate_y, data->base.translate_z );

   retro3d_scene_rotate( 0, data->base.rotate_y, data->base.rotate_z );

   retro3d_draw_model( &(data->obj) );

   demo_draw_fps();

   retro3d_scene_complete();
   retroflat_draw_release( NULL );

   data->base.rotate_y += 5;

cleanup:

   if( retval ) {
      retro3dp_destroy_obj( &(data->obj) );
      retroflat_quit( retval );
   }

}

void demo_fp_poly_well( struct DEMO_FP_DATA* data ) {
   const float ang_iter = 0.2f;

   /* Draw floor under well. */
   glBegin( GL_QUADS );
   glColor3fv( RETROGLU_COLOR_GRAY );
   glNormal3f( 0, 1.0f, 0 );
   glVertex3f( -0.5f, 0, -0.5f );
   glVertex3f( -0.5f, 0,  0.5f );
   glVertex3f(  0.5f, 0,  0.5f );
   glVertex3f(  0.5f, 0, -0.5f );
   glEnd();

   /* Draw well. */
   poly_well( RETROGLU_COLOR_GRAY, 0.5f, 0.4f, 0.25f, ang_iter );

   /* Draw water. */
   poly_water_ring(
      RETROGLU_COLOR_CYAN, 0.15f, 0.4f, 0.05f,
      ang_iter, 0.3f, 20.0f, 0.01f,
      data->water_peak_offset );
}

#define DEMO_RING_HR 0.3f
#define DEMO_RING_BR 0.1f
#define DEMO_RING_BA_INC 0.4f
#define DEMO_RING_HA_INC 0.4f

void demo_fp_poly_ring( struct DEMO_FP_DATA* data ) {
   float 
      x_hole = 0,
      y_hole = 0,
      x_hole_next = 0,
      y_hole_next = 0,
      y_bar = 0,
      z_bar = 0,
      y_bar_next = 0,
      z_bar_next = 0,
      ang_hole = 0,
      ang_bar = 0;

   /* Red floor. */
   glBegin( GL_QUADS );
   glColor3fv( RETROGLU_COLOR_DARKRED );
   glNormal3f( 0, 1.0f, 0 );
   glVertex3f( -0.5f, 0, -0.5f );
   glVertex3f( -0.5f, 0,  0.5f );
   glVertex3f(  0.5f, 0,  0.5f );
   glVertex3f(  0.5f, 0, -0.5f );
   glEnd();

   for(
      ang_hole = 0 ; 2 * RETROFLAT_PI > ang_hole ; ang_hole += DEMO_RING_HA_INC
   ) {
      for(
         ang_bar = RETROFLAT_PI ; 2 * RETROFLAT_PI > ang_bar ; ang_bar += DEMO_RING_BA_INC
      ) {

         x_hole = cos( ang_hole ) * DEMO_RING_HR;
         y_hole = sin( ang_hole ) * DEMO_RING_HR + 0.5f;
         x_hole_next = cos( ang_hole + DEMO_RING_HA_INC ) * DEMO_RING_HR;
         y_hole_next = sin( ang_hole + DEMO_RING_HA_INC ) * DEMO_RING_HR + 0.5f;

         z_bar = cos( ang_bar ) * DEMO_RING_BR;
         y_bar = sin( ang_bar ) * DEMO_RING_BR;
         z_bar_next = cos( ang_bar + DEMO_RING_BA_INC ) * DEMO_RING_BR;
         y_bar_next = sin( ang_bar + DEMO_RING_BA_INC ) * DEMO_RING_BR;

         glBegin( GL_QUADS );
         glColor3fv( RETROGLU_COLOR_YELLOW );
         glNormal3f( x_hole, y_hole + y_bar, z_bar );

         glVertex3f( x_hole, y_hole + y_bar, z_bar );
         glVertex3f( x_hole_next, y_hole_next + y_bar, z_bar );
         glVertex3f( x_hole_next, y_hole_next + y_bar_next, z_bar_next );
         glVertex3f( x_hole, y_hole + y_bar_next, z_bar_next );

         glVertex3f( x_hole, y_hole + y_bar_next, z_bar_next );
         glVertex3f( x_hole_next, y_hole_next + y_bar_next, z_bar_next );
         glVertex3f( x_hole_next, y_hole_next + y_bar, z_bar );
         glVertex3f( x_hole, y_hole + y_bar, z_bar );

#if 0
         glVertex3f(
            x_hole + z_bar, y_hole + y_bar, z_bar );
         glVertex3f(
            x_hole_next + z_bar, y_hole_next + y_bar, z_bar );
         glVertex3f(
            x_hole_next + z_bar_next, y_hole_next + y_bar_next, z_bar_next );
         glVertex3f(
            x_hole + z_bar_next, y_hole + y_bar_next, z_bar_next );
#endif

         glEnd();
      }
   }
}

void draw_fp_iter( struct DEMO_FP_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   struct RETRO3D_PROJ_ARGS args;
   int x = 0,
      z = 0;
   float z_diff = 0,
      x_diff = 0;
   const float light_pos[] = {
      0,
      0.5,
      0,
      0 };

   if( !data->base.init ) {

      /* Init scene. */
      args.proj = RETRO3D_PROJ_FRUSTUM;
      args.rzoom = 0.3f;
      args.near_plane = 0.5f;
      args.far_plane = 100.0f;
      retro3d_init_projection( &args );

      /* Setup map tiles: */

      /* Floor 0 */
      data->tiles[0].list = glGenLists( 1 );
      glNewList( data->tiles[0].list, GL_COMPILE );
      glBegin( GL_QUADS );
      glColor3fv( RETROGLU_COLOR_DARKGREEN );
      glNormal3f( 0, 1.0f, 0 );
      glVertex3f( -0.5f, 0, -0.5f );
      glVertex3f( -0.5f, 0,  0.5f );
      glVertex3f(  0.5f, 0,  0.5f );
      glVertex3f(  0.5f, 0, -0.5f );
      glEnd();
      glEndList();

      /* Wall 1 */
      data->tiles[1].list = glGenLists( 1 );
      glNewList( data->tiles[1].list, GL_COMPILE );
      /* Compensate for the cube being Y-centered on zero. */
      glTranslatef( 0, 0.5f, 0 );
      poly_cube(
         1.0f,
         RETROFLAT_COLOR_RED, RETROFLAT_COLOR_GREEN, RETROFLAT_COLOR_BLUE,
         RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_CYAN, RETROFLAT_COLOR_MAGENTA );
      glTranslatef( 0, -0.5f, 0 );
      glEndList();

      /* Floor 2 */
      data->tiles[2].list = glGenLists( 1 );
      glNewList( data->tiles[2].list, GL_COMPILE );
      glBegin( GL_QUADS );
      glColor3fv( RETROGLU_COLOR_DARKBLUE );
      glNormal3f( 0, 1.0f, 0 );
      glVertex3f( -0.5f, 0, -0.5f );
      glVertex3f( -0.5f, 0,  0.5f );
      glVertex3f(  0.5f, 0,  0.5f );
      glVertex3f(  0.5f, 0, -0.5f );
      glEnd();
      glEndList();

      /* Sphere */
      data->tiles[3].list = glGenLists( 1 );
      glNewList( data->tiles[3].list, GL_COMPILE );

      /* Red floor. */
      glBegin( GL_QUADS );
      glColor3fv( RETROGLU_COLOR_DARKRED );
      glNormal3f( 0, 1.0f, 0 );
      glVertex3f( -0.5f, 0, -0.5f );
      glVertex3f( -0.5f, 0,  0.5f );
      glVertex3f(  0.5f, 0,  0.5f );
      glVertex3f(  0.5f, 0, -0.5f );
      glEnd();

      /* Compensate for the sphere being Y-centered on zero. */
      glTranslatef( 0, 0.5f, 0 );
      poly_sphere_checker( RETROFLAT_COLOR_RED, RETROFLAT_COLOR_WHITE, 0.5f );
      glTranslatef( 0, -0.5f, 0 );
      glEndList();

      /* Well */
      data->tiles[4].anim = demo_fp_poly_well;

      /* Ring */
      data->tiles[5].anim = demo_fp_poly_ring;

      data->base.translate_y = -0.5f;
      data->base.translate_x = (DEMO_FP_MAP_W / 2) - 2;
      data->base.translate_z = (DEMO_FP_MAP_H / 2) - 2;

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

#if 0
      glFogf( GL_FOG_MODE, GL_EXP );
      glFogfv( GL_FOG_COLOR, RETROGLU_COLOR_WHITE );
      glFogf( GL_FOG_DENSITY, 0.25f );
      glFogf( GL_FOG_START, 5.0f );
      glFogf( GL_FOG_END, 8.0f );
      glEnable( GL_FOG );
#endif

      data->base.init = 1;
   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_W:
      z_diff = cos( (data->base.rotate_y * (2 * RETROFLAT_PI)) / 360 ) * 0.1f;
      x_diff = sin( (data->base.rotate_y * (2 * RETROFLAT_PI)) / 360 ) * 0.1f;
      if( 
         0 > ceil( data->base.translate_z + z_diff ) ||
         0 > ceil( data->base.translate_x - x_diff ) ||
         DEMO_FP_MAP_H <= ceil( data->base.translate_z + z_diff )  ||
         DEMO_FP_MAP_W <= ceil( data->base.translate_x - x_diff ) 
      ) {
         break;
      }
      data->base.translate_z += z_diff;
      data->base.translate_x -= x_diff;
      debug_printf( 3, "tx: %d, tz: %d",
            data->base.translate_x, data->base.translate_z );
      debug_printf( 3, "map x: %d, y: %d",
         DEMO_FP_MAP_W - (int)(ceil( data->base.translate_x )),
         DEMO_FP_MAP_H - (int)(ceil( data->base.translate_z )) - 1 );
      break;

   case RETROFLAT_KEY_S:
      data->base.translate_z -=
         cos( (data->base.rotate_y * (2 * RETROFLAT_PI)) / 360 ) * 0.1f;
      data->base.translate_x +=
         sin( (data->base.rotate_y * (2 * RETROFLAT_PI)) / 360 ) * 0.1f;
      debug_printf( 3, "tx: %d, tz: %d",
         data->base.translate_x, data->base.translate_z );
      break;

   case RETROFLAT_KEY_A:
      if( 5 > data->base.rotate_y ) {
         data->base.rotate_y = 355;
      } else {
         data->base.rotate_y -= 5;
      }
      debug_printf( 3, "ry: %d", data->base.rotate_y );
      break;

   case RETROFLAT_KEY_D:
      if( 355 <= data->base.rotate_y ) {
         data->base.rotate_y = 0;
      } else {
         data->base.rotate_y += 5;
      }
      debug_printf( 3, "ry: %d", data->base.rotate_y );
      break;

   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glLoadIdentity();

   /* Create a new matrix to apply transformations for this frame. */
   glPushMatrix();

#ifndef DEMOS_NO_LIGHTS
   glEnable( GL_LIGHTING );
   glEnable( GL_NORMALIZE );
   glEnable( GL_LIGHT0 );
   glEnable( GL_COLOR_MATERIAL );
#endif /* !DEMOS_NO_LIGHTS */

   /* Translate/rotate the whole scene. */
   glRotatef( data->base.rotate_y, 0, 1.0f, 0 );
   glTranslatef( data->base.translate_x, data->base.translate_y, data->base.translate_z );

   /* Draw the map tiles around us. */
   glTranslatef( 1.0f, 0, 1.0f );
   glTranslatef(
      -1.0f * ((float)DEMO_FP_MAP_W), 0, -1.0f * ((float)DEMO_FP_MAP_H) );
   for( z = 0 ; DEMO_FP_MAP_H > z ; z++ ) {
      glTranslatef( (float)(DEMO_FP_MAP_W), 0, 1.0f );
      for( x = DEMO_FP_MAP_W - 1 ; 0 <= x ; x-- ) {
         glTranslatef( -1.0f, 0, 0 );
         if(
            NULL == data->tiles[g_demo_fp_map[(z * DEMO_FP_MAP_W) + x]].anim
         ) {
            glCallList(
               data->tiles[g_demo_fp_map[(z * DEMO_FP_MAP_W) + x]].list );
         } else {
            data->tiles[g_demo_fp_map[(z * DEMO_FP_MAP_W) + x]].anim( data );
         }
      }
   }

#ifndef DEMOS_NO_LIGHTS
   glEnable( GL_NORMALIZE );
   glEnable( GL_LIGHTING );
   glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
#endif /* !DEMOS_NO_LIGHTS */

   glPopMatrix();

   demo_draw_fps();

   retroflat_draw_release( NULL );

   data->water_peak_offset -= 0.01f;
}

void draw_sprite_iter( struct DEMO_SPRITE_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   struct RETRO3D_PROJ_ARGS args;
   MERROR_RETVAL retval = MERROR_OK;

   if( 0 == data->base.init ) {
      /* Load sprite. */
      retval = demo_load_sprite( "test", &(data->sprite) );
      assert( MERROR_OK == retval );

      retroglu_set_sprite_pos( &(data->sprite), 120, 120 );
      retroglu_init_sprite_vertices_scale( &(data->sprite), 0.05f );

      /* Generate texture frame 1 list. */
      retroglu_prerender_sprite(
         &(data->sprite), 0,
         0, 48, 0, 32, 16, 16, 0 );

      /* Generate texture frame 2 list. */
      retroglu_prerender_sprite(
         &(data->sprite), 1,
         16, 48, 16, 32, 16, 16, 0 );

      /* Load tiles. */
      /* XXX */

      /* Setup projection. */

      args.proj = RETRO3D_PROJ_ORTHO;
      args.rzoom = 1.0f;
      args.near_plane = -100.0f;
      args.far_plane = 100.0f;
      retro3d_init_projection( &args );

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->base.init = 1;
   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_A:
      if( 
         0 == data->sprite.rotate_y ||
         180 == data->sprite.rotate_y ||
         -180 == data->sprite.rotate_y
      ) {
         data->sprite.rotate_y += 10;
      }
      break;

   case RETROFLAT_KEY_D:
      if(
         0 == data->sprite.rotate_y ||
         180 == data->sprite.rotate_y ||
         -180 == data->sprite.rotate_y
      ) {
         data->sprite.rotate_y -= 10;
      }
      break;

   case RETROFLAT_KEY_RIGHT:
      data->sprite.translate_x += DEMO_TRANSLATE_INC;
      break;

   case RETROFLAT_KEY_LEFT:
      data->sprite.translate_x -= DEMO_TRANSLATE_INC;
      break;

   case RETROFLAT_KEY_DOWN:
      data->sprite.translate_y -= DEMO_TRANSLATE_INC;
      break;

   case RETROFLAT_KEY_UP:
      data->sprite.translate_y += DEMO_TRANSLATE_INC;
      break;

   case RETROFLAT_KEY_ESC:
      retroglu_free_sprite( &(data->sprite) );
      retroflat_quit( 0 );
      goto end_func;
   }

   /* Draw */

   retroflat_draw_lock( NULL );

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   /* Create a new matrix to apply transformations for this frame. */
   glPushMatrix();

   /* Scale sprite down to manageable size insite this matrix. */
   retroglu_tsrot_sprite( &(data->sprite) );

   retroglu_jitrender_sprite( &(data->sprite), data->tex_frame_idx );

   glPopMatrix();

   demo_draw_fps();

   retroflat_draw_release( NULL );

   /* Set the walking frame. */
   data->tex_countdown--;
   if( 0 >= data->tex_countdown ) {
      data->tex_frame_idx = (0 == data->tex_frame_idx ? 1 : 0);
      data->tex_countdown = 30;
   }

   /* Rotate a little bit with each frame if we're rotating. */
   if( 0 < data->sprite.rotate_y && 180 > data->sprite.rotate_y ) {
      data->sprite.rotate_y += DEMO_ROTATE_INC;
   } else if( 180 < data->sprite.rotate_y && 360 > data->sprite.rotate_y ) {
      data->sprite.rotate_y += DEMO_ROTATE_INC;
   } else if( 0 > data->sprite.rotate_y && -180 < data->sprite.rotate_y ) {
      data->sprite.rotate_y -= DEMO_ROTATE_INC;
   } else if( -180 > data->sprite.rotate_y && -360 < data->sprite.rotate_y ) {
      data->sprite.rotate_y -= DEMO_ROTATE_INC;
   } else if( 360 == data->sprite.rotate_y || -360 == data->sprite.rotate_y ) {
      data->sprite.rotate_y = 0;
   }

end_func:
   return;
}

#endif /* DEMOS_NO_FILES */

void draw_water_iter( struct DEMO_WATER_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   struct RETRO3D_PROJ_ARGS args;
   const float light_pos[] = { 6.0f, 6.0f, 10.0f, 1.0f };

   if( 0 == data->base.init ) {

      args.proj = RETRO3D_PROJ_FRUSTUM;
      args.rzoom = 1.0f;
      /* args.near_plane = 0.5f; */
      args.near_plane = 1.0f;
      args.far_plane = 100.0f;
      retro3d_init_projection( &args );

#ifndef DEMOS_NO_LIGHTS
      glEnable( GL_LIGHT0 );
      glEnable( GL_COLOR_MATERIAL );
#endif /* !DEMOS_NO_LIGHTS */

#ifndef DEMOS_NO_LISTS
      /* Setup the skybox (such as it is). */
      data->skybox_list = glGenLists( 1 );
      glNewList( data->skybox_list, GL_COMPILE );
      poly_water_skybox();
      glEndList();
#endif /* !DEMOS_NO_LISTS */

      /* TODO: Add CLI option to select pattern. */
      data->pattern = 1;
      data->freq_mod = 7.0f;
      data->amp_mod = 0.1f;
      if( 1 == data->pattern ) {
         data->base.translate_y = mfix( -4.0f );
         data->base.translate_z = mfix( -4.0f );

#ifndef DEMOS_NO_LISTS
         /* Setup well list. */
         data->well_list = glGenLists( 1 );
         glNewList( data->well_list, GL_COMPILE );
         poly_well( RETROGLU_COLOR_GRAY, DEMO_WATER_RING_RADIUS + 0.75f,
            DEMO_WATER_RING_RADIUS, 2.0f, DEMO_WATER_RING_A_ITER );
         glEndList();
#endif /* !DEMOS_NO_LISTS */

      } else {
         data->base.translate_y = -8.0f;
         data->base.translate_z = -4.0f;
         data->base.rotate_x = 20;
      }

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->base.init = 1;
   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_UP:
      data->base.translate_y += mfix( 0.1f );
      debug_printf( 3, "ty: %d", data->base.translate_y );
      break;

   case RETROFLAT_KEY_DOWN:
      data->base.translate_y -= mfix( 0.1f );
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

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

#ifndef DEMOS_NO_LIGHTS
   glEnable( GL_NORMALIZE );
   glEnable( GL_LIGHTING );
   glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
#endif /* !DEMOS_NO_LIGHTS */

   /* Create a new matrix to apply transformations for this frame. */
   glPushMatrix();

   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );
   retro3d_scene_translate(
      data->base.translate_x, data->base.translate_y, data->base.translate_z );

#ifdef DEMOS_NO_LISTS
   poly_water_skybox();
#else
   /* Use the same skybox for either mode. */
   glCallList( data->skybox_list );
#endif /* DEMOS_NO_LISTS */

   if( 0 == data->pattern ) {

      poly_water_sheet(
         RETROGLU_COLOR_CYAN, DEMO_WATER_SHEET_WIDTH, DEMO_WATER_SHEET_DEPTH,
         DEMO_WATER_SHEET_X_ITER,
         data->freq_mod, data->amp_mod, data->peak_offset );

   } else {

#ifdef DEMOS_NO_LISTS
   poly_well( RETROGLU_COLOR_GRAY, DEMO_WATER_RING_RADIUS + 0.75f,
      DEMO_WATER_RING_RADIUS, 2.0f, DEMO_WATER_RING_A_ITER );
#else
      /* Only the well gets drawn from a list since it's static. */
      glCallList( data->well_list );
#endif /* DEMOS_NO_LISTS */

      poly_water_ring(
         RETROGLU_COLOR_CYAN, 1.0f,
         DEMO_WATER_RING_RADIUS, DEMO_WATER_RING_R_ITER,
         DEMO_WATER_RING_A_ITER, 0.3f, data->freq_mod, data->amp_mod,
         data->peak_offset );

   }

   glPopMatrix();

   demo_draw_fps();

   retroflat_draw_release( NULL );

   data->peak_offset -= 0.1f;
}

void draw_retroani_iter( struct DEMO_RETROANI_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;
   struct RETRO3D_PROJ_ARGS args;
   MERROR_RETVAL retval = MERROR_OK;

   if( !data->base.init ) {
      data->base.init = 1;

      /* Create the fire animation texture. */

      debug_printf( 1, "initializing fire cube..." );

      data->base.translate_z = mfix( -5.0f );

      data->bmp_fire = calloc( 1, sizeof( struct RETROFLAT_BITMAP ) );
      assert( NULL != data->bmp_fire );
      retval = retroflat_create_bitmap(
         RETROANI_TILE_W, RETROANI_TILE_H, data->bmp_fire, 0 );
      maug_cleanup_if_not_ok();

      assert( RETROANI_TILE_W == retroflat_bitmap_w( data->bmp_fire ) );
      assert( RETROANI_TILE_H == retroflat_bitmap_h( data->bmp_fire ) );
      assert( NULL != data->bmp_fire->tex.bytes_h );

      retval = retroflat_draw_lock( data->bmp_fire );
      maug_cleanup_if_not_ok();

      assert( NULL != data->bmp_fire->tex.bytes );

      retroflat_rect(
         data->bmp_fire, RETROFLAT_COLOR_BLACK, 0, 0,
         RETROANI_TILE_W, RETROANI_TILE_H,
         RETROFLAT_FLAGS_FILL );
      retroflat_draw_release( data->bmp_fire );

      assert( NULL != data->bmp_fire->tex.bytes_h );

      data->idx_fire = retroani_create(
         &(data->animations[0]), ANIMATIONS_MAX,
         RETROANI_TYPE_FIRE, RETROANI_FLAG_CLEANUP,
         0, 0,
         retroflat_bitmap_w( data->bmp_fire ),
         retroflat_bitmap_h( data->bmp_fire ) );

      retroani_set_target( data->animations, data->idx_fire, data->bmp_fire );

      /* Create the snow animation texture. */

      debug_printf( 1, "initializing snow cube..." );

      data->bmp_snow = calloc( 1, sizeof( struct RETROFLAT_BITMAP ) );
      assert( NULL != data->bmp_snow );
      retval = retroflat_create_bitmap(
         RETROANI_TILE_W, RETROANI_TILE_H, data->bmp_snow, 0 );
      maug_cleanup_if_not_ok();

      retval = retroflat_draw_lock( data->bmp_snow );
      maug_cleanup_if_not_ok();

      retroflat_rect(
         data->bmp_snow, RETROFLAT_COLOR_BLACK, 0, 0,
         RETROANI_TILE_W, RETROANI_TILE_H,
         RETROFLAT_FLAGS_FILL );
      retroflat_draw_release( data->bmp_snow );

      data->idx_snow = retroani_create(
         &(data->animations[0]), ANIMATIONS_MAX,
         RETROANI_TYPE_SNOW, RETROANI_FLAG_CLEANUP,
         0, 0,
         retroflat_bitmap_w( data->bmp_snow ),
         retroflat_bitmap_h( data->bmp_snow ) );

      retroani_set_target( data->animations, data->idx_snow, data->bmp_snow );

      /* Create the cube. */

#if 0
#ifndef DEMOS_NO_LISTS
      data->cube_list = glGenLists( 1 );
      glNewList( data->cube_list, GL_COMPILE );
      poly_cube(
         RETROGLU_COLOR_RED, RETROFLAT_COLOR_GREEN, RETROFLAT_COLOR_BLUE,
         RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_CYAN, RETROFLAT_COLOR_MAGENTA );
      glEndList();
#endif /* DEMOS_NO_LISTS */
#endif

      assert( NULL != data->bmp_fire->tex.bytes_h );

      args.proj = RETRO3D_PROJ_FRUSTUM;
      args.rzoom = 0.5f;
      args.near_plane = 0.5f;
      args.far_plane = 100.0f;
      retro3d_init_projection( &args );

      data->base.rotate_x = 10;
      data->base.rotate_y = 10;

   }

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_RIGHT:
      break;

   case RETROFLAT_KEY_LEFT:
      break;

   case RETROFLAT_KEY_ESC:
      retroani_stop_all( &(data->animations[0]), ANIMATIONS_MAX );
      retroflat_destroy_bitmap( data->bmp_fire );
      free( data->bmp_fire );
      retroflat_destroy_bitmap( data->bmp_snow );
      free( data->bmp_snow );
      retroflat_quit( 0 );
      goto end_func;

   case RETROFLAT_KEY_SPACE:
      break;

   default:
      break;
   }

   assert( NULL != data->bmp_fire->tex.bytes_h );

   /* Drawing */

   retroflat_draw_lock( NULL );

   retroflat_draw_lock( data->bmp_fire );
   retroflat_draw_lock( data->bmp_snow );
   retroani_frame( &(data->animations[0]), ANIMATIONS_MAX, 0 );
   retroflat_draw_release( data->bmp_fire );
   retroflat_draw_release( data->bmp_snow );

   assert( NULL != data->bmp_fire->tex.bytes_h );

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glPushMatrix();

#ifndef DEMOS_NO_LIGHTS
   glEnable( GL_NORMALIZE );
#endif /* DEMOS_NO_LIGHTS */

   glPushMatrix();
   glScalef( 8.0f, 6.0f, 1.0f );
   poly_ortho_skybox( RETROGLU_COLOR_WHITE, data->bmp_snow );
   glPopMatrix();

   retro3d_scene_translate( 0, 0, data->base.translate_z );
   retro3d_scene_rotate( data->base.rotate_x, data->base.rotate_y, 0 );

/* #ifdef DEMOS_NO_LISTS */
   poly_cube_tex(
      data->bmp_fire, 1.0f,
      RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE,
      RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE, RETROFLAT_COLOR_WHITE );
#if 0
/* #else */
   glCallList( data->cube_list );
/* #endif */ /* DEMOS_NO_LISTS */
#endif

   glPopMatrix();

   demo_draw_fps();

   retroflat_draw_release( NULL );

   if( data->next_rotate_ms < retroflat_get_ms() ) {
      data->base.rotate_y += 5;
      data->next_rotate_ms = retroflat_get_ms() + 50;
   }

cleanup:

   if( retval ) {
      retroflat_quit( retval );
   }

end_func:
   return;
}

