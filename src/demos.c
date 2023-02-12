
#include <stdio.h>

#include <math.h>

#define DEMOS_C
#define RETROGLU_C
#include "demos.h"

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

      /* Note that the normals begin in the middle of the face and line
       * up with the face with the most similar vertexes.
       */

      /* White side - BACK */
      glBegin( GL_POLYGON );
      glNormal3f(  0, 0, 1.0f );
      glColor3f(   1.0,  1.0, 1.0 );
      glVertex3f(  0.5, -0.5, 0.5 );
      glVertex3f(  0.5,  0.5, 0.5 );
      glVertex3f( -0.5,  0.5, 0.5 );
      glVertex3f( -0.5, -0.5, 0.5 );
      glEnd();
      
      /* Purple side - RIGHT */
      glBegin( GL_POLYGON );
      glNormal3f(  1.0f, 0, 0 );
      glColor3f(  1.0,  0.0,  1.0 );
      glVertex3f( 0.5, -0.5, -0.5 );
      glVertex3f( 0.5,  0.5, -0.5 );
      glVertex3f( 0.5,  0.5,  0.5 );
      glVertex3f( 0.5, -0.5,  0.5 );
      glEnd();
      
      /* Green side - LEFT */
      glBegin( GL_POLYGON );
      glNormal3f(  -1.0f, 0, 0 );
      glColor3f(   0.0,  1.0,  0.0 );
      glVertex3f( -0.5, -0.5,  0.5 );
      glVertex3f( -0.5,  0.5,  0.5 );
      glVertex3f( -0.5,  0.5, -0.5 );
      glVertex3f( -0.5, -0.5, -0.5 );
      glEnd();

      /* Yellow side - FRONT */
      glBegin( GL_POLYGON );
      glNormal3f(  0, 0, -1.0f );
      glColor3f(   1.0,  1.0, 0.0 );
      glVertex3f( -0.5, -0.5, -0.5 );
      glVertex3f( -0.5,  0.5, -0.5 );
      glVertex3f(  0.5,  0.5, -0.5 );
      glVertex3f(  0.5, -0.5, -0.5 );
      glEnd();
      
      /* Blue side - TOP */
      glBegin( GL_POLYGON );
      glNormal3f(  0, 1.0f, 0 );
      glColor3f(   0.0,  0.0,  1.0 );
      glVertex3f(  0.5,  0.5,  0.5 );
      glVertex3f(  0.5,  0.5, -0.5 );
      glVertex3f( -0.5,  0.5, -0.5 );
      glVertex3f( -0.5,  0.5,  0.5 );
      glEnd();
      
      /* Red side - BOTTOM */
      glBegin( GL_POLYGON );
      glNormal3f(  0, -1.0f, 0 );
      glColor3f(   1.0,  0.0,  0.0 );
      glVertex3f(  0.5, -0.5, -0.5 );
      glVertex3f(  0.5, -0.5,  0.5 );
      glVertex3f( -0.5, -0.5,  0.5 );
      glVertex3f( -0.5, -0.5, -0.5 );
      glEnd();

      glEndList();

      retroglu_init_scene( RETROGLU_INIT_LIGHTS );
      args.proj = RETROGLU_PROJ_FRUSTUM;
      args.rzoom = 0.5f;
      retroglu_init_projection( &args );

      glEnable( GL_LIGHT0 );
      glEnable( GL_COLOR_MATERIAL );

      data->rotate_x = 10;
      data->rotate_y = 10;

      data->init = 1;
   }

   /* Input */

   input_evt.allow_repeat = 1;
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
   float ang_xy = 0;
   float ang_xz = 0;
   int even_col = 1;
   struct RETROGLU_PROJ_ARGS args;
   const float color_white[] = { 1.0f, 1.0f, 1.0f };
   const float light_pos[] = { 6.0f, 6.0f, 10.0f, 1.0f };

   if( !data->init ) {

      data->sphere_list = glGenLists( 1 );
      glNewList( data->sphere_list, GL_COMPILE );

      /* Generate a sphere using the cos() as X and sin() as Y of angles at 
       * DEMO_SPHERE_INC_XY and DEMO_SPHERE_INC_XZ increments around the
       * origin (0, 0).
       */
      for(
         ang_xz = 2 * RETROFLAT_PI ;
         0 <= ang_xz ;
         ang_xz -= DEMO_SPHERE_INC_XZ
      ) {
         for(
            ang_xy = 0 ;
            2 * RETROFLAT_PI > ang_xy ;
            ang_xy += DEMO_SPHERE_INC_XY
         ) {
            glBegin( GL_TRIANGLES );
            even_col = even_col ? 0 : 1;

            /* Checkerboard pattern. */
            if( even_col ) {
               glColor3f( 1.0, 0.0, 0.0 );
            } else {
               glColor3f( 1.0, 1.0, 1.0 );
            }

            /* Setup a normal for each face (2 triangles). */
            /* We're just copying the first vertex here... that should make
             * each face reflect in its own direction, which is what we want
             * for this demo.
             */
            glNormal3f( 
               sin( ang_xy ) * cos( ang_xz ),
               cos( ang_xy ),
               sin( ang_xy ) * sin( ang_xz ) );

            /* Quad panels at equal intervals around two circles intersecting
             * on orthogonal planes.
             */

            /* Each checkerboard square is 2 triangles: */

            /* Triangle 1 */
            glVertex3f( 
               sin( ang_xy ) * cos( ang_xz ),
               cos( ang_xy ),
               sin( ang_xy ) * sin( ang_xz ) );
            glVertex3f( 
               sin( ang_xy + DEMO_SPHERE_INC_XY ) * cos( ang_xz ),
               cos( ang_xy + DEMO_SPHERE_INC_XY ),
               sin( ang_xy + DEMO_SPHERE_INC_XY ) * sin( ang_xz ) );
            glVertex3f( 
               sin( ang_xy + DEMO_SPHERE_INC_XY ) 
                  * cos( ang_xz + DEMO_SPHERE_INC_XZ ),
               cos( ang_xy + DEMO_SPHERE_INC_XY ),
               sin( ang_xy + DEMO_SPHERE_INC_XY ) 
                  * sin( ang_xz + DEMO_SPHERE_INC_XZ ) );

            /* Triangle 2 */
            glVertex3f( 
               sin( ang_xy + DEMO_SPHERE_INC_XY ) 
                  * cos( ang_xz + DEMO_SPHERE_INC_XZ ),
               cos( ang_xy + DEMO_SPHERE_INC_XY ),
               sin( ang_xy + DEMO_SPHERE_INC_XY ) 
                  * sin( ang_xz + DEMO_SPHERE_INC_XZ ) );
            glVertex3f(
               sin( ang_xy ) * cos( ang_xz + DEMO_SPHERE_INC_XZ ),
               cos( ang_xy ),
               sin( ang_xy ) * sin( ang_xz + DEMO_SPHERE_INC_XZ ) );
            glVertex3f( 
               sin( ang_xy ) * cos( ang_xz ),
               cos( ang_xy ),
               sin( ang_xy ) * sin( ang_xz ) );

            glEnd();
         }
      }

      glEndList();

      data->skybox_list = glGenLists( 1 );
      glNewList( data->skybox_list, GL_COMPILE );

         glColor3fv( color_white );

         /* Create a skybox. Note the normals, crucial for making the sides
          * show up properly in lighting.
          * This is a frustum shape to enhance its "3D" appearance in ortho
          * rendering.
          */

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
      retroglu_init_projection( &args );

      glEnable( GL_LIGHT0 );
      glEnable( GL_COLOR_MATERIAL );

      /* Start spin. */
      data->rotate_y_inc = 5;

      /* Start bounce. */
      data->translate_x_inc = 0.2f;
      data->translate_y_inc = 0.2f;

      data->translate_z = -5.0f;

      data->init = 1;
   }

   /* Input */

   input_evt.allow_repeat = 1;
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

   if( 0 == data->vertices_sz ) {
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
      retroglu_init_projection( &args );

      glEnable( GL_LIGHT0 );
   }

   /* Input */

   input_evt.allow_repeat = 1;
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

void draw_fp_iter( struct DEMO_FP_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0;

   /* Input */

   input_evt.allow_repeat = 1;
   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );

   glFlush();
   retroflat_draw_release( NULL );
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
      retroglu_init_projection( &args );

      data->init = 1;
   }

   /* Input */

   input_evt.allow_repeat = 1;
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
   float x = 0,
      y = 0,
      z = 0,
      x_next = 0,
      y_next = 0;
   static int rotate_x = 0;
   static int rotate_y = 0;
   static float peak_offset = 0;
   struct RETROGLU_PROJ_ARGS args;

   if( 0 == data->init ) {

      retroglu_init_scene( 0 );
      args.proj = RETROGLU_PROJ_FRUSTUM;
      args.rzoom = 10.0f;
      retroglu_init_projection( &args );

      data->init = 1;
   }

   /* Input */

   input_evt.allow_repeat = 1;
   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_RIGHT:
      rotate_y += 10;
      break;

   case RETROFLAT_KEY_UP:
      rotate_x += 10;
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

   glScalef( 0.4f, 0.4f, 0.4f );

   glRotatef( rotate_x, 1.0f, 0, 0 );
   glRotatef( rotate_y, 0, 1.0f, 0 );

   for( x = -10.0f ; 10.0f > x ; x += 0.1f ) {
      x_next = x + 0.1f;
      y = sin( x + peak_offset );
      y_next = sin( x_next + peak_offset );

      glBegin( GL_QUADS );
      glNormal3f( 0,           y,      0 );
      glColor3f( 0, 0.75, 0.75 );
      glVertex3f( x,           y, -10.0f ); /* Far Left */
      glVertex3f( x,           y,  10.0f ); /* Near Left */
      glVertex3f( x_next, y_next,  10.0f ); /* Near Right */
      glVertex3f( x_next, y_next, -10.0f ); /* Far Right */
      glEnd();
   }

   glPopMatrix();

   glFlush();
   retroflat_draw_release( NULL );

   peak_offset += 0.1f;
}

