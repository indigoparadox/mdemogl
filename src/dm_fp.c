
#include "demos.h"

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
         1,
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
      poly_sphere_checker( RETROFLAT_COLOR_RED, RETROFLAT_COLOR_WHITE );
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

