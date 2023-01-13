
#include <stdio.h>

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
   static struct RETROGLU_VERTEX vertices[1024];
   struct RETROGLU_PARSER parser;
   static int vertices_sz = 0;
   long int tree_read = 0;
   unsigned long int i = 0;

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

      parser.vertices = vertices;
      parser.vertices_max = 1024;
      retroglu_parse_init( &parser );
      for( i = 0 ; parser.obj_buf_sz > i ; i++ ) {
         tree_read = retroglu_parse_obj_c( &parser, parser.obj_buf[i] );
         assert( 0 <= tree_read );
      }
      free( parser.obj_buf );
      parser.obj_buf = NULL;
      parser.obj_buf_sz = 0;
      vertices_sz = parser.vertices_sz;
      assert( 0 < vertices_sz );
   }

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

   glBegin( GL_POLYGON );
   
   for( i = 0 ; vertices_sz > i ; i++ ) {
      glVertex3f( vertices[i].x, vertices[i].y, vertices[i].z );     
   }

   glEnd();

   glFlush();

   retroflat_draw_release( NULL );

   rotate_y += 5;

   next = retroflat_get_ms() + retroflat_fps_next();
}

