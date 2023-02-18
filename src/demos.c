
#include <stdio.h>

#include <math.h>

#define DEMOS_C
#define RETROGLU_C
#include "demos.h"

#include "poly.h"

int demo_load_obj(
   const char* filename, struct RETROGLU_PARSER* parser,
   struct DEMO_OBJ_DATA* data
) {
   FILE* obj_file = NULL;
   uint32_t i = 0; /* Index in file buffer, so long. */
   long int obj_read = 0;
   int auto_parser = 0; /* Did we provision parser? */
   uint8_t* obj_buf = NULL;
   uint32_t obj_buf_sz = 0;

   if( NULL == parser ) {
      parser = calloc( 1, sizeof( struct RETROGLU_PARSER ) );
      assert( NULL != parser );
      auto_parser = 1;
   }

   /* Open the file and allocate the buffer. */
   obj_file = fopen( filename, "r" );
   assert( NULL != obj_file );
   fseek( obj_file, 0, SEEK_END );
   obj_buf_sz = ftell( obj_file );
   fseek( obj_file, 0, SEEK_SET );
   debug_printf( 3, "opened %s, " UPRINTF_U32 " bytes", filename, obj_buf_sz );
   obj_buf = calloc( 1, obj_buf_sz );
   assert( NULL != obj_buf );
   obj_read = fread( obj_buf, 1, obj_buf_sz, obj_file );
   assert( obj_read == obj_buf_sz );
   fclose( obj_file );

   retroglu_parse_init(
      parser,
      data->vertices, data->vertices_sz, DEMO_VERTICES_SZ_MAX,
      data->vnormals, data->vnormals_sz, DEMO_VERTICES_SZ_MAX,
      data->vtextures, data->vtextures_sz, DEMO_VERTICES_SZ_MAX,
      data->faces, data->faces_sz, DEMO_FACES_SZ_MAX,
      data->materials, data->materials_sz, DEMO_MATERIALS_SZ_MAX,
      (retroglu_mtl_cb)demo_load_obj, (void*)data );

   /* Parse the obj, byte by byte. */
   for( i = 0 ; obj_buf_sz > i ; i++ ) {
      obj_read = retroglu_parse_obj_c( parser, obj_buf[i] );
      assert( 0 <= obj_read );
   }
   free( obj_buf );
   obj_buf = NULL;
   obj_buf_sz = 0;
   data->vertices_sz = parser->vertices_sz;
   data->vnormals_sz = parser->vnormals_sz;
   data->vtextures_sz = parser->vtextures_sz;
   data->faces_sz = parser->faces_sz;
   data->materials_sz = parser->materials_sz;

   if( auto_parser ) {
      free( parser );
      parser = NULL;
   }

   return RETROFLAT_OK;
}

void demo_dump_obj( const char* filename, struct DEMO_OBJ_DATA* data ) {
   FILE* obj_file = NULL;
   uint32_t i = 0, /* Index in file buffer, so long. */
      j = 0;

   /* Dump */
   obj_file = fopen( filename, "w" );
   assert( NULL != obj_file );
   for( i = 0 ; data->vertices_sz > i ; i++ ) {
      fprintf( obj_file, "v %f %f %f\n",
         data->vertices[i].x, data->vertices[i].y, data->vertices[i].z );
   }

   fprintf( obj_file, "\n" );
   
   for( i = 0 ; data->vnormals_sz > i ; i++ ) {
      fprintf( obj_file, "vn %f %f %f\n",
         data->vnormals[i].x, data->vnormals[i].y, data->vnormals[i].z );
   }

   for( i = 0 ; data->faces_sz > i ; i++ ) {
      if(
         0 == i ||
         data->faces[i].material_idx != data->faces[i - 1].material_idx
      ) {
         fprintf( obj_file, "\nusemtl %s\n\n",
            data->materials[data->faces[i].material_idx].name );
      }
      fprintf( obj_file, "f " );
      for( j = 0 ; data->faces[i].vertex_idxs_sz > j ; j++ ) {
         fprintf( obj_file, "%d/%d/%d",
            data->faces[i].vertex_idxs[j],
            data->faces[i].vtexture_idxs[j],
            data->faces[i].vnormal_idxs[j] );
         if( j + 1 < data->faces[i].vertex_idxs_sz ) {
            fprintf( obj_file, " " );
         }
      }
      fprintf( obj_file, "\n" );
   }

   fclose( obj_file );
   obj_file = NULL;
}

MERROR_RETVAL demo_load_sprite(
   const char* filename, struct RETROGLU_SPRITE* sprite
) {
   MERROR_RETVAL retval = 0;
   uint32_t bmp_w = 0,
      bmp_h = 0,
      texture_id = 0;

   retval = retroglu_load_tex_bmp( filename, &texture_id, &bmp_w, &bmp_h );

   if( MERROR_OK != retval ) {
      goto cleanup;
   }
   retroglu_set_sprite_tex( sprite, texture_id, bmp_w, bmp_h );

cleanup:
   return retval;
}

void draw_cube_iter( struct DEMO_CUBE_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0;
   float translate_z = -5.0f;
   struct RETROGLU_PROJ_ARGS args;

   if( !data->init ) {

      data->cube_list = glGenLists( 1 );
      glNewList( data->cube_list, GL_COMPILE );

      poly_cube(
         RETROGLU_COLOR_RED, RETROGLU_COLOR_GREEN, RETROGLU_COLOR_BLUE,
         RETROGLU_COLOR_WHITE, RETROGLU_COLOR_CYAN, RETROGLU_COLOR_MAGENTA );

      glEndList();

      retroglu_init_scene( RETROGLU_INIT_LIGHTS );
      args.proj = RETROGLU_PROJ_FRUSTUM;
      args.rzoom = 0.5f;
      args.near_plane = 0.5f;
      args.far_plane = 10.0f;
      retroglu_init_projection( &args );

      glEnable( GL_LIGHT0 );
      glEnable( GL_COLOR_MATERIAL );

      data->rotate_x = 10;
      data->rotate_y = 10;

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->init = 1;
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

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glPushMatrix();

   glTranslatef( 0.0f, 0.0f, translate_z );
   glRotatef( data->rotate_x, 1.0f, 0.0f, 0.0f );
   glRotatef( data->rotate_y, 0.0f, 1.0f, 0.0f );

   glCallList( data->cube_list );

   glPopMatrix();

   glFlush();

   retroflat_draw_release( NULL );

   data->rotate_y += 5;
}

void draw_sphere_iter( struct DEMO_SPHERE_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0;
   struct RETROGLU_PROJ_ARGS args;
   const float light_pos[] = { 6.0f, 6.0f, 10.0f, 1.0f };

   if( !data->init ) {

      data->sphere_list = glGenLists( 1 );
      glNewList( data->sphere_list, GL_COMPILE );

      poly_sphere_checker( RETROGLU_COLOR_RED, RETROGLU_COLOR_WHITE, 1.0f );

      glEndList();

      data->skybox_list = glGenLists( 1 );
      glNewList( data->skybox_list, GL_COMPILE );

         glColor3fv( RETROGLU_COLOR_WHITE );

         /* Create a skybox. Note the normals, crucial for making the sides
          * show up properly in lighting.
          * This is a frustum shape to enhance its "3D" appearance in ortho
          * rendering.
          */

         /* TODO: Break into triangles. */

         /* Back Face */
         glBegin( GL_QUADS );
         glNormal3f(  0, 0, 1.0f );
         glVertex3f(  1.0f,  1.0f, -10.0f ); /* Top Right */
         glVertex3f( -1.0f,  1.0f, -10.0f ); /* Top Left */
         glVertex3f( -1.0f, -1.0f, -10.0f ); /* Bottom Left */
         glVertex3f(  1.0f, -1.0f, -10.0f ); /* Bottom Right */
         glEnd();

         /* Bottom Face */
         glBegin( GL_QUADS );
         glNormal3f(  0, 1.0f, 0 );
         glVertex3f(  1.0f, -1.0f, -10.0f );
         glVertex3f( -1.0f, -1.0f, -10.0f );
         glVertex3f( -2.0f, -2.0f,  10.0f );
         glVertex3f(  2.0f, -2.0f,  10.0f );
         glEnd();

         /* Right Face */
         glBegin( GL_QUADS );
         glNormal3f(  -1.0f, 0, 0 );
         glVertex3f(  2.0f,  2.0f,  10.0f );
         glVertex3f(  1.0f,  1.0f, -10.0f );
         glVertex3f(  1.0f, -1.0f, -10.0f );
         glVertex3f(  2.0f, -2.0f,  10.0f );
         glEnd();

         /* Top Face */
         glBegin( GL_QUADS );
         glNormal3f(  0, -1.0f, 0 );
         glVertex3f(  2.0f,  2.0f,  10.0f );
         glVertex3f( -2.0f,  2.0f,  10.0f );
         glVertex3f( -1.0f,  1.0f, -10.0f );
         glVertex3f(  1.0f,  1.0f, -10.0f );
         glEnd();

         /* Left Face */
         glBegin( GL_QUADS );
         glNormal3f(  1.0f, 0, 0 );
         glVertex3f( -1.0f,  1.0f, -10.0f );
         glVertex3f( -2.0f,  2.0f,  10.0f );
         glVertex3f( -2.0f, -2.0f,  10.0f );
         glVertex3f( -1.0f, -1.0f, -10.0f );
         glEnd();

      glEndList();

      retroglu_init_scene( RETROGLU_INIT_LIGHTS );
      args.proj = RETROGLU_PROJ_ORTHO;
      args.rzoom = 10.0f;
      args.near_plane = -100.0f;
      args.far_plane = 100.0f;
      retroglu_init_projection( &args );

      glEnable( GL_LIGHT0 );
      glEnable( GL_COLOR_MATERIAL );

      /* Start spin. */
      data->rotate_y_inc = 5;

      /* Start bounce. */
      data->translate_x_inc = 0.2f;
      data->translate_y_inc = 0.2f;

      data->translate_z = -5.0f;

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->init = 1;
   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_RIGHT:
      data->rotate_y += 10;
      debug_printf( 1, "rotate_y: %d", data->rotate_y );
      break;

   case RETROFLAT_KEY_LEFT:
      data->rotate_y -= 10;
      debug_printf( 1, "rotate_y: %d", data->rotate_y );
      break;

   case RETROFLAT_KEY_UP:
      data->rotate_x += 10;
      debug_printf( 1, "rotate_x: %d", data->rotate_x );
      break;

   case RETROFLAT_KEY_DOWN:
      data->rotate_x -= 10;
      debug_printf( 1, "rotate_x: %d", data->rotate_x );
      break;

   case RETROFLAT_KEY_P:
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

   glLightfv( GL_LIGHT0, GL_POSITION, light_pos );

   glPushMatrix();

   glScalef( 8.0f, 6.0f, 1.0f );

   glCallList( data->skybox_list );

   glPopMatrix();

   /* Draw sphere from display list. */

   glPushMatrix();

   glTranslatef( data->translate_x, data->translate_y, data->translate_z );
   glRotatef( data->rotate_x, 1.0f, 0.0f, 0.0f );
   glRotatef( data->rotate_y, 0.0f, 1.0f, 0.0f );

   glCallList( data->sphere_list );

   glPopMatrix();

   glFlush();

   retroflat_draw_release( NULL );

   data->rotate_y += data->rotate_y_inc;

   /* X bounce. */
   data->translate_x += data->translate_x_inc;
   if(
      9 <= data->translate_x ||
      -9 >= data->translate_x 
   ) {
      data->translate_x_inc *= -1.0f;
      data->translate_x += data->translate_x_inc;
   }

   /* Y bounce. */
   data->translate_y += data->translate_y_inc;
   if(
      6 <= data->translate_y ||
      -6 >= data->translate_y 
   ) {
      data->translate_y_inc *= -1.0f;
      data->translate_y += data->translate_y_inc;
   }
}

void draw_obj_iter( struct DEMO_OBJ_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0;
   /*
   static int rotate_x = 10;
   */
   static int rotate_y = 10;
   static int rotate_z = 0;
   static float tx = 0.0f,
      ty = 0.0f,
      tz = -5.0f;
   const float l_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
   const float l_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
   /*
   const float l_position[] = {0.0f, 0.0f, 2.0f, 1.0f};
   */
   struct RETROGLU_PROJ_ARGS args;

   if( 0 == data->init ) {
      demo_load_obj( g_demo_obj_name, NULL, data );

      assert( 0 < data->vertices_sz );
      assert( 0 < data->faces_sz );
      assert( 0 < data->materials_sz );

      if( '\0' != g_demo_dump_name[0] ) {
         demo_dump_obj( g_demo_dump_name, data );
         debug_printf( 3, "demo data dumped to %s", g_demo_dump_name );
      }

      /* Setup display list. */
      data->obj_list = glGenLists( 1 );
      glNewList( data->obj_list, GL_COMPILE );

      retroglu_draw_poly(
         data->vertices, data->vertices_sz,
         data->vnormals, data->vnormals_sz,
         data->vtextures, data->vtextures_sz,
         data->faces, data->faces_sz,
         data->materials, data->materials_sz );
      
      glEndList();

      retroglu_init_scene( RETROGLU_INIT_LIGHTS );
      args.proj = RETROGLU_PROJ_FRUSTUM;
      args.rzoom = 1.0f;
      args.near_plane = 0.5f;
      args.far_plane = 10.0f;
      retroglu_init_projection( &args );

      glEnable( GL_LIGHT0 );

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->init = 1;
   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
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

   case RETROFLAT_KEY_E:
      tz += DEMO_ZOOM_INC;
      debug_printf( 3, "%f, %f, %f", tx, ty, tz );
      break;

   case RETROFLAT_KEY_D:
      tz -= DEMO_ZOOM_INC;
      debug_printf( 3, "%f, %f, %f", tx, ty, tz );
      break;

   case RETROFLAT_KEY_R:
      rotate_z += DEMO_ROTATE_INC;
      debug_printf( 3, "%d", rotate_z );
      break;

   case RETROFLAT_KEY_F:
      rotate_z -= DEMO_ROTATE_INC;
      debug_printf( 3, "%d", rotate_z );
      break;

   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );

   /* Create a new matrix to apply transformations for this frame. */
   glPushMatrix();

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glScalef( 0.5f, 0.5f, 0.5f );
   glTranslatef( tx, ty, tz );

   /* glRotatef( 1, 1.0f, 0.0f, 0.0f ); */
   glRotatef( rotate_y, 0.0f, 1.0f, 0.0f );
   glRotatef( rotate_z, 0.0f, 0.0f, 1.0f );

   /*
   glLightfv( GL_LIGHT0, GL_POSITION, l_position );
   */
   glLightfv( GL_LIGHT0, GL_DIFFUSE, l_diffuse );
   glLightfv( GL_LIGHT0, GL_AMBIENT, l_ambient );

   glCallList( data->obj_list );

   glPopMatrix();

   glFlush();

   retroflat_draw_release( NULL );

   rotate_y += 5;

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
   int input = 0;
   struct RETROGLU_PROJ_ARGS args;
   int x = 0,
      z = 0;
   float z_diff = 0,
      x_diff = 0;
   const float light_pos[] = {
      0,
      0.5,
      0,
      0 };

   if( !data->init ) {

      /* Init scene. */
      retroglu_init_scene( RETROGLU_INIT_LIGHTS );
      args.proj = RETROGLU_PROJ_FRUSTUM;
      args.rzoom = 0.3f;
      args.near_plane = 0.5f;
      args.far_plane = 100.0f;
      retroglu_init_projection( &args );

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
         RETROGLU_COLOR_RED, RETROGLU_COLOR_GREEN, RETROGLU_COLOR_BLUE,
         RETROGLU_COLOR_WHITE, RETROGLU_COLOR_CYAN, RETROGLU_COLOR_MAGENTA );
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
      /* Compensate for the sphere being Y-centered on zero. */
      glTranslatef( 0, 0.5f, 0 );
      poly_sphere_checker( RETROGLU_COLOR_RED, RETROGLU_COLOR_WHITE, 0.5f );
      glTranslatef( 0, -0.5f, 0 );
      glEndList();

      /* Well */
      data->tiles[4].anim = demo_fp_poly_well;

      /* Ring */
      data->tiles[5].anim = demo_fp_poly_ring;

      glEnable( GL_LIGHT0 );
      glEnable( GL_COLOR_MATERIAL );

      data->translate_y = -0.5f;
      data->translate_x = (DEMO_FP_MAP_W / 2) - 2;
      data->translate_z = (DEMO_FP_MAP_H / 2) - 2;

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->init = 1;
   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_W:
      z_diff = cos( (data->rotate_y * (2 * RETROFLAT_PI)) / 360 ) * 0.1f;
      x_diff = sin( (data->rotate_y * (2 * RETROFLAT_PI)) / 360 ) * 0.1f;
      if( 
         0 > ceil( data->translate_z + z_diff ) ||
         0 > ceil( data->translate_x - x_diff ) ||
         DEMO_FP_MAP_H <= ceil( data->translate_z + z_diff )  ||
         DEMO_FP_MAP_W <= ceil( data->translate_x - x_diff ) 
      ) {
         break;
      }
      data->translate_z += z_diff;
      data->translate_x -= x_diff;
      debug_printf( 3, "tx: %f, tz: %f", data->translate_x, data->translate_z );
      debug_printf( 3, "map x: %d, y: %d",
         DEMO_FP_MAP_W - (int)(ceil( data->translate_x )),
         DEMO_FP_MAP_H - (int)(ceil( data->translate_z )) - 1 );
      break;

   case RETROFLAT_KEY_S:
      data->translate_z -=
         cos( (data->rotate_y * (2 * RETROFLAT_PI)) / 360 ) * 0.1f;
      data->translate_x +=
         sin( (data->rotate_y * (2 * RETROFLAT_PI)) / 360 ) * 0.1f;
      debug_printf( 3, "tx: %f, tz: %f", data->translate_x, data->translate_z );
      break;

   case RETROFLAT_KEY_A:
      if( 5 > data->rotate_y ) {
         data->rotate_y = 355.0f;
      } else {
         data->rotate_y -= 5.0f;
      }
      debug_printf( 3, "ry: %f", data->rotate_y );
      break;

   case RETROFLAT_KEY_D:
      if( 355 <= data->rotate_y ) {
         data->rotate_y = 0;
      } else {
         data->rotate_y += 5.0f;
      }
      debug_printf( 3, "ry: %f", data->rotate_y );
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

   /* Translate/rotate the whole scene. */
   glRotatef( data->rotate_y, 0, 1.0f, 0 );
   glTranslatef( data->translate_x, data->translate_y, data->translate_z );

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

   glLightfv( GL_LIGHT0, GL_POSITION, light_pos );

   glPopMatrix();

   glFlush();
   retroflat_draw_release( NULL );

   data->water_peak_offset -= 0.01f;
}

void draw_sprite_iter( struct DEMO_SPRITE_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0;
   struct RETROGLU_PROJ_ARGS args;

   if( 0 == data->init ) {
      /* Load sprite. */
      demo_load_sprite( "test", &(data->sprite) );
      retroglu_set_sprite_clip(
         &(data->sprite), 0, 48, 0, 32, 16, 16, RETROGLU_FLAGS_INIT_VERTICES );
      retroglu_set_sprite_pos( &(data->sprite), 400, 300 );
      data->sprite.scale_x = 4.0f;
      data->sprite.scale_y = 4.0f;

      /* Generate texture frame 1 list. */
      data->sprite_list[0] = glGenLists( 1 );
      glNewList( data->sprite_list[0], GL_COMPILE );
      retroglu_draw_sprite( &(data->sprite) );
      glEndList();

      /* Generate texture frame 2 list. */
      data->sprite_list[1] = glGenLists( 1 );
      retroglu_set_sprite_clip(
         &(data->sprite), 16, 48, 16, 32, 16, 16, 0 );
      glNewList( data->sprite_list[1], GL_COMPILE );
      retroglu_draw_sprite( &(data->sprite) );
      glEndList();

      /* Load tiles. */
      /* XXX */

      data->sprite.scale_x = 0.05f;
      data->sprite.scale_y = 0.05f;

      /* Setup projection. */

      retroglu_init_scene( 0 );
      args.proj = RETROGLU_PROJ_ORTHO;
      args.rzoom = 1.0f;
      args.near_plane = -100.0f;
      args.far_plane = 100.0f;
      retroglu_init_projection( &args );

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->init = 1;
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
      retroflat_quit( 0 );
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   /* Create a new matrix to apply transformations for this frame. */
   glPushMatrix();

   /* Scale sprite down to manageable size insite this matrix. */
   retroglu_tsrot_sprite( &(data->sprite) );

   glCallList( data->sprite_list[data->tex_frame_idx] );

   glPopMatrix();

   glFlush();
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
}

void draw_water_iter( struct DEMO_WATER_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0;
   struct RETROGLU_PROJ_ARGS args;
   const float light_pos[] = { 6.0f, 6.0f, 10.0f, 1.0f };

   if( 0 == data->init ) {

      retroglu_init_scene( RETROGLU_INIT_LIGHTS );
      args.proj = RETROGLU_PROJ_FRUSTUM;
      args.rzoom = 1.0f;
      /* args.near_plane = 0.5f; */
      args.near_plane = 1.0f;
      args.far_plane = 100.0f;
      retroglu_init_projection( &args );

      glEnable( GL_LIGHT0 );
      glEnable( GL_COLOR_MATERIAL );

      /* Setup the skybox (such as it is). */
      data->skybox_list = glGenLists( 1 );
      glNewList( data->skybox_list, GL_COMPILE );

      glBegin( GL_QUADS );
      glColor3fv( RETROGLU_COLOR_GREEN );
      glNormal3f( 0, 1, 0 );
      glVertex3f( -200.0f, 0, -100.0f );
      glVertex3f( -200.0f, 0, 100.0f );
      glVertex3f( 200.0f, 0, 100.0f );
      glVertex3f( 200.0f, 0, -100.0f );
      glEnd();

      glBegin( GL_QUADS );
      glColor3fv( RETROGLU_COLOR_BLUE );
      glNormal3f( 0, 0, 1 );
      glVertex3f( 200.0f, 0, -50.0f );
      glVertex3f( 200.0f, 200.0f, -50.0f );
      glVertex3f( -200.0f, 200.0f, -50.0f );
      glVertex3f( -200.0f, 0, -50.0f );
      glEnd();

      glEndList();

      /* TODO: Add CLI option to select pattern. */
      data->pattern = 1;
      data->freq_mod = 7.0f;
      data->amp_mod = 0.1f;
      if( 1 == data->pattern ) {
         data->translate_y = -4.0f;
         data->translate_z = -4.0f;

         /* Setup well list. */
         data->well_list = glGenLists( 1 );
         glNewList( data->well_list, GL_COMPILE );

         poly_well( RETROGLU_COLOR_GRAY, DEMO_WATER_RING_RADIUS + 0.75f,
            DEMO_WATER_RING_RADIUS, 2.0f, DEMO_WATER_RING_A_ITER );

         glEndList();
      } else {
         data->translate_y = -8.0f;
         data->translate_z = -4.0f;
         data->rotate_x = 20;
      }

      if( DEMO_FLAG_WIRE == (DEMO_FLAG_WIRE & g_demo_flags) ) {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }

      data->init = 1;
   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_UP:
      data->translate_y += 0.1f;
      debug_printf( 3, "ty: %f", data->translate_y );
      break;

   case RETROFLAT_KEY_DOWN:
      data->translate_y -= 0.1f;
      debug_printf( 3, "ty: %f", data->translate_y );
      break;

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

   case RETROFLAT_KEY_R:
      data->freq_mod += 0.5f;
      debug_printf( 3, "freq_mod: %f", data->freq_mod );
      break;

   case RETROFLAT_KEY_F:
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

   glLightfv( GL_LIGHT0, GL_POSITION, light_pos );

   /* Create a new matrix to apply transformations for this frame. */
   glPushMatrix();

   glRotatef( data->rotate_x, 1.0f, 0, 0 );
   glRotatef( data->rotate_y, 0, 1.0f, 0 );
   glTranslatef( data->translate_x, data->translate_y, data->translate_z );

   /* Use the same skybox for either mode. */
   glCallList( data->skybox_list );

   if( 0 == data->pattern ) {

      poly_water_sheet(
         RETROGLU_COLOR_CYAN, DEMO_WATER_SHEET_WIDTH, DEMO_WATER_SHEET_DEPTH,
         DEMO_WATER_SHEET_X_ITER,
         data->freq_mod, data->amp_mod, data->peak_offset );

   } else {

      /* Only the well gets drawn from a list since it's static. */
      glCallList( data->well_list );

      poly_water_ring(
         RETROGLU_COLOR_CYAN, 1.0f,
         DEMO_WATER_RING_RADIUS, DEMO_WATER_RING_R_ITER,
         DEMO_WATER_RING_A_ITER, 0.3f, data->freq_mod, data->amp_mod,
         data->peak_offset );

   }

   glPopMatrix();

   glFlush();
   retroflat_draw_release( NULL );

   data->peak_offset -= 0.1f;
}

