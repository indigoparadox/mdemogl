
#include <stdio.h>

#include <math.h>

#define DEMOS_C
#define RETROGLU_C
#include "demos.h"

void demo_init_scene( uint8_t flags ) {
   debug_printf( 3, "initializing..." );
   glEnable( GL_CULL_FACE );
   glEnable( GL_TEXTURE_2D );
   glEnable( GL_NORMALIZE );

   if( DEMO_INIT_LIGHTS == (DEMO_INIT_LIGHTS & flags) ) {
      glEnable( GL_LIGHTING );
      glEnable( GL_LIGHT0 );
   }

   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

   /* Setup depth buffer so triangles in back are hidden. */
   glEnable( GL_DEPTH_TEST );
   glDepthMask( GL_TRUE );
   glDepthFunc( GL_LESS );
   glDepthRange( 0.0f, 1.0f );
   
   glShadeModel( GL_SMOOTH );
}

void demo_init_projection( uint8_t demo_proj ) {
   /* Setup projection. */
   glViewport(
      0, 0, (GLint)retroflat_screen_w(), (GLint)retroflat_screen_h() );

   /* Switch to projection matrix for setup. */
   glMatrixMode( GL_PROJECTION );

   /* Zero everything out. */
   glLoadIdentity();

   switch( demo_proj ) {
   case DEMO_PROJ_FRUSTUM:
      /* This is really tweaky, and when it breaks, polygons seem to get drawn
         * out of order? Still experimenting/researching. */
      glFrustum(
         /* The smaller these are, the closer it lets us get to the camera? */
         -0.5, 0.5, -0.5, 0.5,
         /* Near plane can't be zero! */
         0.5f, 10.0f );
      break;

   case DEMO_PROJ_ORTHO:
      /* This is much simpler/more forgiving than frustum. */
      glOrtho( -1.0, 1.0, -1.0, 1.0, -100.0, 100.0 );
      break;
   }

   /* Revert to model matrix for later instructions (out of this scope). */
   glMatrixMode( GL_MODELVIEW );
}

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
   const float l_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
   const float l_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};

   if( !data->init ) {

      data->cube_list = glGenLists( 1 );
      glNewList( data->cube_list, GL_COMPILE );

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

      /* Yellow side - FRONT */
      glBegin( GL_POLYGON );
      glColor3f(   1.0,  1.0, 0.0 );
      glVertex3f( -0.5, -0.5, -0.5 );
      glVertex3f( -0.5,  0.5, -0.5 );
      glVertex3f(  0.5,  0.5, -0.5 );
      glVertex3f(  0.5, -0.5, -0.5 );
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

      glEndList();

      demo_init_scene( 0 );
      demo_init_projection( DEMO_PROJ_FRUSTUM );

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

   glLightfv( GL_LIGHT0, GL_DIFFUSE, l_diffuse );
   glLightfv( GL_LIGHT0, GL_AMBIENT, l_ambient );

   glCallList( data->cube_list );

   glPopMatrix();

   glFlush();

   retroflat_draw_release( NULL );

   data->rotate_y += 5;
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

      demo_init_scene( DEMO_INIT_LIGHTS );
      demo_init_projection( DEMO_PROJ_FRUSTUM );
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
   /* static float sprite_fv[8][2];
   static float sprite_tx_fv[8][2]; */
   static int init = 0;
   static uint32_t tex_x = 0;
   const float l_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
   const float l_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
   static int tex_countdown = 0;

   if( 0 == init ) {
      demo_load_sprite( "test", &(data->sprite) );

      retroglu_set_sprite_clip(
         &(data->sprite), 0, 48, 0, 32, 16, 16, RETROGLU_FLAGS_INIT_VERTICES );
      retroglu_set_sprite_pos( &(data->sprite), 400, 300 );
      data->sprite.scale_x = 4.0f;
      data->sprite.scale_y = 4.0f;

      /* TODO: Generate list for each texture anim frame? */
      data->sprite_list = glGenLists( 1 );
      glNewList( data->sprite_list, GL_COMPILE );

      retroglu_draw_sprite( &(data->sprite) );

      glEndList();

      /* Setup projection. */

      demo_init_scene( 0 );
      demo_init_projection( DEMO_PROJ_ORTHO );

      init = 1;
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

   glLightfv( GL_LIGHT0, GL_DIFFUSE, l_diffuse );
   glLightfv( GL_LIGHT0, GL_AMBIENT, l_ambient );

   /* Create a new matrix to apply transformations for this frame. */
   glPushMatrix();

   retroglu_tsrot_sprite( &(data->sprite) );

   glCallList( data->sprite_list );

   glPopMatrix();

   glFlush();
   retroflat_draw_release( NULL );

   /* Set the walking frame. */
   tex_countdown--;
   if( 0 >= tex_countdown ) {
      tex_x = (0 == tex_x ? 16 : 0);
      retroglu_set_sprite_clip(
         &(data->sprite), tex_x, 48, tex_x, 32, 16, 16, 0 );
      tex_countdown = 30;
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

