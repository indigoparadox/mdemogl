
#include <stdio.h>
#include <math.h>

#define DEMOS_C
#include "demos.h"

#include "poly.h"

int32_t g_frames_per_sec = 0;
int32_t g_frames_since_last = 0;
uint32_t g_last_frame_ms = 0;

MERROR_RETVAL demo_draw_fps( struct DEMO_BASE* base ) {
   struct RETROWIN* win = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   uint32_t now_ms = 0;

   mdata_vector_lock( &(base->win) );
   win = mdata_vector_get( &(base->win), base->demo_win_idx, struct RETROWIN );
   assert( NULL != win );
   retrowin_lock_gui( win );

   /* Generate FPS string. */
   now_ms = retroflat_get_ms();

   g_frames_since_last++;
   if( 0 == g_last_frame_ms || g_last_frame_ms + 1000 <= now_ms ) {
      g_last_frame_ms = now_ms;
      g_frames_per_sec = g_frames_since_last;
      g_frames_since_last = 0;
   }
   retrogui_set_ctl_text(
      win->gui_p, DEMO_IDC_FPS, 12, U32_FMT " FPS", g_frames_per_sec );

cleanup:

   retrowin_unlock_gui( win );
   mdata_vector_unlock( &(base->win) );

   return retval;
}

void demo_dump_obj( const char* filename, struct DEMO_OBJ_DATA* data ) {
#if 0
   FILE* obj_file = NULL;
   uint32_t i = 0, /* Index in file buffer, so long. */
      j = 0;

   /* TODO: Fix me to work with new obj parser! */
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

#if 0
void demo_dump_tex( * tex_bmp ) {
   size_t x, y;

   printf( "\n" );
   for( y = 0 ; tex_bmp->h > y ; y++ ) {
      for( x = 0 ; tex_bmp->w > x ; x++ ) {
         printf( "%02x|",
            tex_bmp->bytes[
               ((y * tex_bmp->w) * 4) + (x * 4)] );
         printf( "%02x|",
            tex_bmp->bytes[
               ((y * tex_bmp->w) * 4) + (x * 4) + 1] );
         printf( "%02x|",
            tex_bmp->bytes[
               ((y * tex_bmp->w) * 4) + (x * 4) + 2] );
         printf( "%02x ",
            tex_bmp->bytes[
               ((y * tex_bmp->w) * 4) + (x * 4) + 3] );
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
#endif

/* === */

/* === */


