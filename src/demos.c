
#include <stdio.h>

#include <math.h>

#define DEMOS_C
#define RETROGLU_C
#include "demos.h"

void demo_init_scene() {
   glEnable( GL_DEPTH_TEST );
   glEnable( GL_CULL_FACE );
   glEnable( GL_BLEND );
   glEnable( GL_TEXTURE_2D );
   glEnable( GL_LIGHTING );
   glEnable( GL_NORMALIZE );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

int demo_load_obj(
   const char* filename, struct RETROGLU_PARSER* parser, struct DEMO_DATA* data
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

void demo_dump_obj( const char* filename, struct DEMO_DATA* data ) {
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

void demo_load_bmp(
   const char* filename, GLuint* p_texture_id,
   uint32_t* p_bmp_w, uint32_t* p_bmp_h
) {
   static uint8_t* bmp_buf = NULL;
   uint32_t bmp_buf_sz = 0;
   uint32_t bmp_read = 0;
   FILE* bmp_file;

   /* Open the file and allocate the buffer. */
   bmp_file = fopen( "test.bmp", "rb" );
   assert( NULL != bmp_file );
   fseek( bmp_file, 0, SEEK_END );
   bmp_buf_sz = ftell( bmp_file );
   fseek( bmp_file, 0, SEEK_SET );
   debug_printf( 3, "opened test.bmp, " UPRINTF_U32 " bytes", bmp_buf_sz );
   assert( NULL == bmp_buf );
   bmp_buf = calloc( 1, bmp_buf_sz );
   assert( NULL != bmp_buf );
   bmp_read = fread( bmp_buf, 1, bmp_buf_sz, bmp_file );
   assert( bmp_read == bmp_buf_sz );
   fclose( bmp_file );

   retroglu_load_tex_bmp(
      bmp_buf, bmp_buf_sz, p_texture_id,
      p_bmp_w, p_bmp_h );

   free( bmp_buf );
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
      ty = -0.5f,
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
         debug_printf( 3, "demo data dumped to %s", g_demo_dump_name );
      }
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

   /*
   glLightfv( GL_LIGHT0, GL_POSITION, l_position );
   */
   glLightfv( GL_LIGHT0, GL_DIFFUSE, l_diffuse );
   glLightfv( GL_LIGHT0, GL_AMBIENT, l_ambient );
   glEnable( GL_LIGHT0 );

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

void draw_bmp_iter( struct DEMO_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   int input = 0,
      i = 0;
   /* static float sprite_fv[8][2];
   static float sprite_tx_fv[8][2]; */
   static GLuint texture = 0;
   static int init = 0;
   static uint32_t bmp_w = 0;
   static uint32_t bmp_h = 0;
   static int rotate_y = 0;
   const float l_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
   const float l_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
   static float tex_x = 0;
   static int tex_countdown = 0;
   static struct RETROGLU_SPRITE sprite;

   if( 0 == init ) {
      demo_load_bmp( "test.bmp", &texture, &bmp_w, &bmp_h );

      sprite.texture_id = texture;
      sprite.texture_w = bmp_w;
      sprite.texture_h = bmp_h;

      retroglu_set_sprite_zoom( &sprite, 64, 64 );
      retroglu_set_sprite_clip( &sprite, 0, 0, 16, 16 );
      retroglu_set_sprite_pos( &sprite, 400, 300 );

#if 0
      /* Setup the sprite vertices. */
      sprite_fv[SPRITE_LL][SPRITE_X] = 0;
      sprite_fv[SPRITE_LL][SPRITE_Y] = 0;
      
      sprite_fv[SPRITE_LR][SPRITE_X] = SPRITE_W * 1.0 / retroflat_screen_w();
      sprite_fv[SPRITE_LR][SPRITE_Y] = 0;
      
      sprite_fv[SPRITE_UR][SPRITE_X] = SPRITE_W * 1.0 / retroflat_screen_w();
      sprite_fv[SPRITE_UR][SPRITE_Y] = SPRITE_H * 1.0 / retroflat_screen_h();
      
      sprite_fv[SPRITE_UL][SPRITE_X] = 0;
      sprite_fv[SPRITE_UL][SPRITE_Y] = SPRITE_H * 1.0 / retroflat_screen_h();

      /* Back face. */

      sprite_fv[4][SPRITE_X] = SPRITE_W * 1.0 / retroflat_screen_w();
      sprite_fv[4][SPRITE_Y] = 0;

      sprite_fv[5][SPRITE_X] = 0;
      sprite_fv[5][SPRITE_Y] = 0;

      sprite_fv[6][SPRITE_X] = 0;
      sprite_fv[6][SPRITE_Y] = SPRITE_H * 1.0 / retroflat_screen_h();

      sprite_fv[7][SPRITE_X] = SPRITE_W * 1.0 / retroflat_screen_w();
      sprite_fv[7][SPRITE_Y] = SPRITE_H * 1.0 / retroflat_screen_h();
   
      /* Setup the sprite texture selection. */
      sprite_tx_fv[0][SPRITE_X] = tex_x;
      sprite_tx_fv[0][SPRITE_Y] = 0.75;

      sprite_tx_fv[1][SPRITE_X] = tex_x + 0.5;
      sprite_tx_fv[1][SPRITE_Y] = 0.75;

      sprite_tx_fv[2][SPRITE_X] = tex_x + 0.5;
      sprite_tx_fv[2][SPRITE_Y] = 1.0;

      sprite_tx_fv[3][SPRITE_X] = tex_x;
      sprite_tx_fv[3][SPRITE_Y] = 1.0;

      /* Back face. */

      sprite_tx_fv[4][SPRITE_X] = tex_x;
      sprite_tx_fv[4][SPRITE_Y] = 0.5;

      sprite_tx_fv[5][SPRITE_X] = tex_x + 0.5;
      sprite_tx_fv[5][SPRITE_Y] = 0.5;

      sprite_tx_fv[6][SPRITE_X] = tex_x + 0.5;
      sprite_tx_fv[6][SPRITE_Y] = 0.75;

      sprite_tx_fv[7][SPRITE_X] = tex_x;
      sprite_tx_fv[7][SPRITE_Y] = 0.75;
#endif

      init = 1;
   }

#if 0
   sprite_tx_fv[0][SPRITE_X] = tex_x;
   sprite_tx_fv[1][SPRITE_X] = tex_x + 0.5;
   sprite_tx_fv[2][SPRITE_X] = tex_x + 0.5;
   sprite_tx_fv[3][SPRITE_X] = tex_x;

   sprite_tx_fv[4][SPRITE_X] = tex_x;
   sprite_tx_fv[5][SPRITE_X] = tex_x + 0.5;
   sprite_tx_fv[6][SPRITE_X] = tex_x + 0.5;
   sprite_tx_fv[7][SPRITE_X] = tex_x;
#endif

   /* Input */

   input_evt.allow_repeat = 1;
   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_A:
      rotate_y += 10;
      break;

   case RETROFLAT_KEY_D:
      rotate_y -= 10;
      break;

   case RETROFLAT_KEY_RIGHT:
   /*
      for( i = 0 ; 4 > i ; i++ ) {
         sprite_fv[i][SPRITE_X] += 0.01;
      }
      */
      break;

   case RETROFLAT_KEY_LEFT:
   /*
      for( i = 0 ; 4 > i ; i++ ) {
         sprite_fv[i][SPRITE_X] -= 0.01;
      }
      */
      break;

   case RETROFLAT_KEY_DOWN:
   /*
      for( i = 0 ; 4 > i ; i++ ) {
         sprite_fv[i][SPRITE_Y] -= 0.01;
      }
      */
      break;

   case RETROFLAT_KEY_UP:
   /*
      for( i = 0 ; 4 > i ; i++ ) {
         sprite_fv[i][SPRITE_Y] += 0.01;
      }
      */
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

   glRotatef( rotate_y, 0.0f, 1.0f, 0.0f );

   glLightfv( GL_LIGHT0, GL_DIFFUSE, l_diffuse );
   glLightfv( GL_LIGHT0, GL_AMBIENT, l_ambient );
   glEnable( GL_LIGHT0 );

/*
   glBindTexture( GL_TEXTURE_2D, texture );
   glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

   glBegin( GL_TRIANGLES );
   glColor3f( 1.0, 1.0, 1.0 );
*/

/*
   sprite_w = 16 * 1.0 / retroflat_screen_w();
   sprite_h = 16 * 1.0 / retroflat_screen_h();
   */

   retroglu_draw_sprite( &sprite );

#if 0
   for( i = 0 ; 8 > i ; i++ ) {
      glTexCoord2fv( sprite_tx_fv[i] );
      glVertex2fv( sprite_fv[i] );
   }
#endif

#if 0
   glTexCoord2fv( sprite_tx_fv[0] );
   glVertex2fv( sprite_fv[0] );

   glTexCoord2fv( sprite_tx_fv[1] );
   glVertex2fv( sprite_fv[1] );

   glTexCoord2fv( sprite_tx_fv[2] );
   glVertex2fv( sprite_fv[2] );

   glTexCoord2fv( sprite_tx_fv[2] );
   glVertex2fv( sprite_fv[2] );

   glTexCoord2fv( sprite_tx_fv[3] );
   glVertex2fv( sprite_fv[3] );

   glTexCoord2fv( sprite_tx_fv[0] );
   glVertex2fv( sprite_fv[0] );

   /* Back face */

   glTexCoord2fv( sprite_tx_fv[4] );
   glVertex2fv( sprite_fv[4] );

   glTexCoord2fv( sprite_tx_fv[5] );
   glVertex2fv( sprite_fv[5] );

   glTexCoord2fv( sprite_tx_fv[6] );
   glVertex2fv( sprite_fv[6] );

   glTexCoord2fv( sprite_tx_fv[6] );
   glVertex2fv( sprite_fv[6] );

   glTexCoord2fv( sprite_tx_fv[7] );
   glVertex2fv( sprite_fv[7] );

   glTexCoord2fv( sprite_tx_fv[4] );
   glVertex2fv( sprite_fv[4] );
#endif

   glEnd();

   tex_countdown--;
   if( 0 >= tex_countdown ) {
      tex_x = (0 == tex_x ? 0.5 : 0);
      tex_countdown = 30;
   }

   glFlush();
   retroflat_draw_release( NULL );
}

