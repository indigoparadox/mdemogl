
#include <stdio.h>

#include <math.h>

#define DEMOS_C
#define RETROGLU_C
#include "demos.h"

void demo_init_scene() {
   glEnable( GL_DEPTH_TEST );
   glEnable( GL_CULL_FACE );
}

int demo_load_obj(
   const char* filename, struct RETROGLU_PARSER* parser, struct DEMO_DATA* data
) {
   FILE* obj_file = NULL;
   unsigned long int i = 0; /* Index in file buffer, so long. */
   long int obj_read = 0;
   int auto_parser = 0; /* Did we provision parser? */
   unsigned char* obj_buf = NULL;
   unsigned long int obj_buf_sz = 0;

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
   printf( "opened %s, %lu bytes\n", filename, obj_buf_sz );
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

void demo_dump_obj( const char* filename, struct DEMO_DATA* data ) {
   FILE* obj_file = NULL;
   unsigned long int i = 0, /* Index in file buffer, so long. */
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

void draw_cube_iter( struct DEMO_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0;
   static int rotate_x = 10;
   static int rotate_y = 10;

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
}

void draw_obj_iter( struct DEMO_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0;
   /*
   static int rotate_x = 10;
   */
   static int rotate_y = 10;
   static int rotate_z = 0;
   static float tx = 0.0f,
      ty = 0.0f,
      tz = 0.0f;
   const float l_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
   const float l_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
   /*
   const float l_position[] = {0.0f, 0.0f, 2.0f, 1.0f};
   */

   if( 0 == data->vertices_sz ) {
      demo_load_obj( g_demo_obj_name, NULL, data );

      assert( 0 < data->vertices_sz );
      assert( 0 < data->faces_sz );
      assert( 0 < data->materials_sz );

      if( '\0' != g_demo_dump_name[0] ) {
         demo_dump_obj( g_demo_dump_name, data );
         printf( "demo data dumped to %s\n", g_demo_dump_name );
      }
   }

   /* Input */

   input_evt.allow_repeat = 1;
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

   case RETROFLAT_KEY_R:
      rotate_z += DEMO_ROTATE_INC;
      printf( "%d\n", rotate_z );
      break;

   case RETROFLAT_KEY_F:
      rotate_z -= DEMO_ROTATE_INC;
      printf( "%d\n", rotate_z );
      break;

   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* Draw */

   retroflat_draw_lock( NULL );

   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();

   glTranslatef( tx, ty, tz );

   /* glRotatef( 1, 1.0f, 0.0f, 0.0f ); */
   glRotatef( rotate_y, 0.0f, 1.0f, 0.0f );
   glRotatef( rotate_z, 0.0f, 0.0f, 1.0f );

   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glMatrixMode( GL_MODELVIEW );
   
   /*
   glLightfv( GL_LIGHT0, GL_POSITION, l_position );
   */
   glLightfv( GL_LIGHT0, GL_DIFFUSE, l_diffuse );
   glLightfv( GL_LIGHT0, GL_AMBIENT, l_ambient );
   glEnable( GL_LIGHT0 );
   glEnable( GL_LIGHTING );

   /*
   glPushMatrix();
   */

   retroglu_draw_poly(
      data->vertices, data->vertices_sz,
      data->vnormals, data->vnormals_sz,
      data->vtextures, data->vtextures_sz,
      data->faces, data->faces_sz,
      data->materials, data->materials_sz );

   /* glPopMatrix(); */

   glFlush();

   retroflat_draw_release( NULL );

   rotate_y += 5;

}

void draw_fp_iter( struct DEMO_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0;
   static float yrot;

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

