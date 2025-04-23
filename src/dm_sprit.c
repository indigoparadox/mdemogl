
#include "demos.h"

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
   if(
      0 < data->sprite.rotate_y &&
      mfix_from_i( 180 ) > data->sprite.rotate_y
   ) {
      data->sprite.rotate_y += DEMO_ROTATE_INC;
   } else if(
      mfix_from_i( 180 ) < data->sprite.rotate_y &&
      mfix_from_i( 360 ) > data->sprite.rotate_y
   ) {
      data->sprite.rotate_y += DEMO_ROTATE_INC;
   } else if(
      0 > data->sprite.rotate_y &&
      mfix_from_i( -180 ) < data->sprite.rotate_y
   ) {
      data->sprite.rotate_y -= DEMO_ROTATE_INC;
   } else if(
      mfix_from_i( -180 ) > data->sprite.rotate_y &&
      mfix_from_i( -360 ) < data->sprite.rotate_y
   ) {
      data->sprite.rotate_y -= DEMO_ROTATE_INC;
   } else if(
      mfix_from_i( 360 ) == data->sprite.rotate_y ||
      mfix_from_i( -360 ) == data->sprite.rotate_y
   ) {
      data->sprite.rotate_y = 0;
   }

end_func:
   return;
}

