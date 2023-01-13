
#include <stdio.h>

#include <math.h>

#define DEMOS_C
#define RETROGLU_C
#include "demos.h"

void draw_cube_iter( void* data ) {
   struct RETROFLAT_INPUT input_evt;
   long unsigned int next = 0;
   int input = 0;
   static int rotate_x = 10;
   static int rotate_y = 10;

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_Q:
      retroflat_quit( 0 );
      break;
   }

   if( retroflat_get_ms() < next ) {
      return;
   }

   /* Draw */

   retroflat_draw_lock( NULL );

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT );

   glLoadIdentity();
   glRotatef( rotate_x, 1.0f, 0.0f, 0.0f );
   glRotatef( rotate_y, 0.0f, 1.0f, 0.0f );

   /* White side - BACK */
   glBegin( GL_POLYGON );
   glColor3f(   1.0,  1.0, 1.0 );
   glVertex3f(  0.5, -0.5, 0.5 );
   glVertex3f(  0.5,  0.5, 0.5 );
   glVertex3f( -0.5,  0.5, 0.5 );
   glVertex3f( -0.5, -0.5, 0.5 );
   glEnd();
   
   /* Purple side - RIGHT */
   glBegin( GL_POLYGON );
   glColor3f(  1.0,  0.0,  1.0 );
   glVertex3f( 0.5, -0.5, -0.5 );
   glVertex3f( 0.5,  0.5, -0.5 );
   glVertex3f( 0.5,  0.5,  0.5 );
   glVertex3f( 0.5, -0.5,  0.5 );
   glEnd();
   
   /* Green side - LEFT */
   glBegin( GL_POLYGON );
   glColor3f(   0.0,  1.0,  0.0 );
   glVertex3f( -0.5, -0.5,  0.5 );
   glVertex3f( -0.5,  0.5,  0.5 );
   glVertex3f( -0.5,  0.5, -0.5 );
   glVertex3f( -0.5, -0.5, -0.5 );
   glEnd();
   
   /* Blue side - TOP */
   glBegin( GL_POLYGON );
   glColor3f(   0.0,  0.0,  1.0 );
   glVertex3f(  0.5,  0.5,  0.5 );
   glVertex3f(  0.5,  0.5, -0.5 );
   glVertex3f( -0.5,  0.5, -0.5 );
   glVertex3f( -0.5,  0.5,  0.5 );
   glEnd();
   
   /* Red side - BOTTOM */
   glBegin( GL_POLYGON );
   glColor3f(   1.0,  0.0,  0.0 );
   glVertex3f(  0.5, -0.5, -0.5 );
   glVertex3f(  0.5, -0.5,  0.5 );
   glVertex3f( -0.5, -0.5,  0.5 );
   glVertex3f( -0.5, -0.5, -0.5 );

   /* White side - BACK */
   glBegin( GL_POLYGON );
   glColor3f(   1.0,  1.0, 1.0 );
   glVertex3f(  0.5, -0.5, 0.5 );
   glVertex3f(  0.5,  0.5, 0.5 );
   glVertex3f( -0.5,  0.5, 0.5 );
   glVertex3f( -0.5, -0.5, 0.5 );
   glEnd();
   
   /* Purple side - RIGHT */
   glBegin( GL_POLYGON );
   glColor3f(  1.0,  0.0,  1.0 );
   glVertex3f( 0.5, -0.5, -0.5 );
   glVertex3f( 0.5,  0.5, -0.5 );
   glVertex3f( 0.5,  0.5,  0.5 );
   glVertex3f( 0.5, -0.5,  0.5 );
   glEnd();
   
   /* Green side - LEFT */
   glBegin( GL_POLYGON );
   glColor3f(   0.0,  1.0,  0.0 );
   glVertex3f( -0.5, -0.5,  0.5 );
   glVertex3f( -0.5,  0.5,  0.5 );
   glVertex3f( -0.5,  0.5, -0.5 );
   glVertex3f( -0.5, -0.5, -0.5 );
   glEnd();
   
   /* Blue side - TOP */
   glBegin( GL_POLYGON );
   glColor3f(   0.0,  0.0,  1.0 );
   glVertex3f(  0.5,  0.5,  0.5 );
   glVertex3f(  0.5,  0.5, -0.5 );
   glVertex3f( -0.5,  0.5, -0.5 );
   glVertex3f( -0.5,  0.5,  0.5 );
   glEnd();
   
   /* Red side - BOTTOM */
   glBegin( GL_POLYGON );
   glColor3f(   1.0,  0.0,  0.0 );
   glVertex3f(  0.5, -0.5, -0.5 );
   glVertex3f(  0.5, -0.5,  0.5 );
   glVertex3f( -0.5, -0.5,  0.5 );
   glVertex3f( -0.5, -0.5, -0.5 );

   glEnd();

   glFlush();

   retroflat_draw_release( NULL );

   rotate_y += 5;

   next = retroflat_get_ms() + retroflat_fps_next();
}

void draw_tree_iter( void* data ) {
   struct RETROFLAT_INPUT input_evt;
   long unsigned int next = 0;
   int input = 0;
   static int rotate_x = 10;
   static int rotate_y = 10;
   FILE* tree_file = NULL;
   static struct RETROGLU_VERTEX vertices[DEMO_VERTICES_SZ_MAX];
   static int vertices_sz = 0;
   static struct RETROGLU_FACE faces[DEMO_FACES_SZ_MAX];
   static int faces_sz = 0;
   struct RETROGLU_PARSER parser;
   long int tree_read = 0;
   unsigned long int i = 0; /* Also index in file buffer, so long. */
   unsigned int j = 0;
   static float tx = 0.0f,
      ty = 0.0f,
      tz = 0.0f;
   float mcolor = 0.0f;

   if( 0 == vertices_sz ) {
      tree_file = fopen( "treeHigh.obj", "r" );
      assert( NULL != tree_file );
      fseek( tree_file, 0, SEEK_END );
      parser.obj_buf_sz = ftell( tree_file );
      fseek( tree_file, 0, SEEK_SET );
      printf( "opened treeHigh.obj, %lu bytes\n", parser.obj_buf_sz );
      parser.obj_buf = calloc( 1, parser.obj_buf_sz );
      assert( NULL != parser.obj_buf );
      tree_read = fread( parser.obj_buf, 1, parser.obj_buf_sz, tree_file );
      assert( tree_read == parser.obj_buf_sz );
      fclose( tree_file );

      retroglu_parse_init(
         &parser,
         vertices, DEMO_VERTICES_SZ_MAX, faces, DEMO_FACES_SZ_MAX );
      for( i = 0 ; parser.obj_buf_sz > i ; i++ ) {
         tree_read = retroglu_parse_obj_c( &parser, parser.obj_buf[i] );
         assert( 0 <= tree_read );
      }
      free( parser.obj_buf );
      parser.obj_buf = NULL;
      parser.obj_buf_sz = 0;
      vertices_sz = parser.vertices_sz;
      faces_sz = parser.faces_sz;
      assert( 0 < vertices_sz );
      assert( 0 < faces_sz );

      /* Dump */
      tree_file = fopen( "test.txt", "w" );
      assert( NULL != tree_file );
      for( i = 0 ; vertices_sz > i ; i++ ) {
         fprintf( tree_file, "v %f %f %f\n",
            vertices[i].x, vertices[i].y, vertices[i].z );
      }
      for( i = 0 ; faces_sz > i ; i++ ) {
         if(
            0 == i ||
            0 != strncmp(
               faces[i].material, faces[i - 1].material,
               RETROGLU_FACE_MATERIAL_SZ_MAX )
         ) {
            fprintf( tree_file, "\nusemtl %s\n\n", faces[i].material );
         }
         fprintf( tree_file, "f " );
         for( j = 0 ; faces[i].vertice_idxs_sz > j ; j++ ) {
            fprintf( tree_file, "%d", faces[i].vertice_idxs[j] );
            if( j + 1 < faces[i].vertice_idxs_sz ) {
               fprintf( tree_file, "/" );
            }
         }
         fprintf( tree_file, "\n" );
      }
      fclose( tree_file );
      tree_file = NULL;

   }

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_Q:
      tx += DEMO_ZOOM_INC;
      printf( "%f, %f, %f\n", tx, ty, tz );
      break;

   case RETROFLAT_KEY_A:
      tx -= DEMO_ZOOM_INC;
      printf( "%f, %f, %f\n", tx, ty, tz );
      break;

   case RETROFLAT_KEY_W:
      ty += DEMO_ZOOM_INC;
      printf( "%f, %f, %f\n", tx, ty, tz );
      break;

   case RETROFLAT_KEY_S:
      ty -= DEMO_ZOOM_INC;
      printf( "%f, %f, %f\n", tx, ty, tz );
      break;

   case RETROFLAT_KEY_E:
      tz += DEMO_ZOOM_INC;
      printf( "%f, %f, %f\n", tx, ty, tz );
      break;

   case RETROFLAT_KEY_D:
      tz -= DEMO_ZOOM_INC;
      printf( "%f, %f, %f\n", tx, ty, tz );
      break;

   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   if( retroflat_get_ms() < next ) {
      return;
   }

   /* Draw */

   retroflat_draw_lock( NULL );

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT );

   glLoadIdentity();
   glTranslatef( tx, ty, tz );
   glRotatef( rotate_x, 1.0f, 0.0f, 0.0f );
   glRotatef( rotate_y, 0.0f, 1.0f, 0.0f );

   for( i = 0 ; faces_sz > i ; i++ ) {
      glBegin( GL_TRIANGLES );
   
      /*
      if(
         0 == i ||
         0 != strncmp(
            faces[i].material, faces[i - 1].material,
            RETROGLU_FACE_MATERIAL_SZ_MAX )
      ) {
         glColor3f(
            fmod( rand() * 0.1, 1.0f ),
            fmod( rand() * 0.1, 1.0f ),
            fmod( rand() * 0.1, 1.0f ) ); 
      }
      */

      glColor3f(
         hash_mat_r( faces[i].material ),
         hash_mat_g( faces[i].material ),
         hash_mat_b( faces[i].material ) );
   
      for( j = 0 ; faces[i].vertice_idxs_sz > j ; j++ ) {
         assert( 0 < faces[i].vertice_idxs[j] );
         assert( 3 == faces[i].vertice_idxs_sz );
         glVertex3f(
            vertices[faces[i].vertice_idxs[j] - 1].x,
            vertices[faces[i].vertice_idxs[j] - 1].y,
            vertices[faces[i].vertice_idxs[j] - 1].z );
      }

      glEnd();
   }

   glFlush();

   retroflat_draw_release( NULL );

   rotate_y += 5;

   next = retroflat_get_ms() + retroflat_fps_next();
}

