
#include <time.h>
#include <stdlib.h>

#define MAUG_C
#include <maug.h>
#include <retroflt.h>
#include <retroani.h>

#include "demos.h"

retroflat_loop_iter g_loop = NULL;
void* g_data = NULL;

MERROR_RETVAL demo_setup_win( struct DEMO_BASE* base ) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL ctl;
   struct RETROWIN3D* win = NULL;

   retval = retro3dw_push_win(
      NULL, /* This window should create and manage its own GUI. */
      &(base->win),
      DEMO_IDC_WIN, "unscii-8.hex",
      10, 10,
      DEMO_WIN_W, DEMO_WIN_H, 0 );
   maug_cleanup_if_not_ok();

   mdata_vector_lock( &(base->win) );
   win = mdata_vector_get_last( &(base->win), struct RETROWIN3D );
   assert( NULL != win );
   retrogui_lock( win->gui );

      retrogui_init_ctl( &ctl, RETROGUI_CTL_TYPE_LABEL, DEMO_IDC_TITLE_1 );

      ctl.base.x = 10;
      ctl.base.y = 10;
      ctl.base.w = 100;
      ctl.base.h = 20;
      ctl.base.fg_color = RETROFLAT_COLOR_BLACK;
      ctl.BUTTON.label = "commit " MDEMO_COMMIT_HASH;
      ctl.BUTTON.label_sz = 12; /* "commit " + 5 chars */

      retval = retrogui_push_ctl( win->gui, &ctl );
      maug_cleanup_if_not_ok();

   retrogui_unlock( win->gui );
   mdata_vector_unlock( &(base->win) );

cleanup:

   return retval;
}

/* === */

static MERROR_RETVAL demo_cli_cb(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   int i = 0;

   for( i = 0 ; '\0' != gc_demo_names[i][0] ; i++ ) {
      if( 0 == strncmp( gc_demo_names[i], arg, strlen( gc_demo_names[i] ) ) ) {
         g_loop = gc_demo_loops[i];
         g_data = calloc( 1, gc_demo_data_sz[i] );
         debug_printf(
            3, "demo loop manually selected: %s", gc_demo_names[i] );
         break;
      }
   }

   return RETROFLAT_OK;
}

static MERROR_RETVAL demo_timer_cli_cb(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 1 == arg_c ) {
      g_timer = 1;
      debug_printf( 3, "timer enabled" );
   }
   return RETROFLAT_OK;
}

static MERROR_RETVAL demo_obj_cli_cb(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 2 == arg_c ) {
      strncpy( g_demo_obj_name, arg, DEMO_OBJ_NAME_SZ_MAX );
      debug_printf( 3, "demo obj manually selected: %s", g_demo_obj_name );
   }

   return RETROFLAT_OK;
}

static MERROR_RETVAL demo_dump_cli_cb(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 2 == arg_c ) {
      strncpy( g_demo_dump_name, arg, DEMO_OBJ_NAME_SZ_MAX );
      debug_printf( 3, "demo dump manually selected: %s", g_demo_dump_name );
   }
   return RETROFLAT_OK;
}

static MERROR_RETVAL demo_wire_cli_cb(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( 1 == arg_c ) {
      g_demo_flags |= DEMO_FLAG_WIRE;
   }
   return RETROFLAT_OK;
}

int main( int argc, char** argv ) {
   int retval = 0;
   struct RETROFLAT_ARGS args;
   int i = 0,
      j = 0;

   /* === Setup === */

   srand( time( NULL ) );

   logging_init();

   retval = retrogxc_init();
   maug_cleanup_if_not_ok();

   maug_mzero( &args, sizeof( struct RETROFLAT_ARGS ) );

   args.title = "mdemo";
   args.assets_path = "assets";
   args.flags = RETROFLAT_FLAGS_KEY_REPEAT;

   maug_add_arg(
      MAUG_CLI_SIGIL "t", MAUG_CLI_SIGIL_SZ + 1,
      "show the on-screen timer", 0,
      (maug_cli_cb)demo_timer_cli_cb, &args );

   maug_add_arg(
      MAUG_CLI_SIGIL "o", MAUG_CLI_SIGIL_SZ + 1,
      "load the specified object", 0,
      (maug_cli_cb)demo_obj_cli_cb, &args );

   maug_add_arg(
      MAUG_CLI_SIGIL "d", MAUG_CLI_SIGIL_SZ + 1,
      "dump demo object", 0,
      (maug_cli_cb)demo_dump_cli_cb, &args );

   maug_add_arg(
      MAUG_CLI_SIGIL "w", MAUG_CLI_SIGIL_SZ + 1,
      "view in wireframe mode", 0,
      (maug_cli_cb)demo_wire_cli_cb, &args );

   /* Add demos to CLI parser. */
   for( i = 0 ; '\0' != gc_demo_names[i][0] ; i++ ) {
      maug_add_arg(
         gc_demo_names[i], 0, "display this demo", 0,
         (maug_cli_cb)demo_cli_cb, &args );
   }

   retval = retroflat_init( argc, argv, &args );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

   /*
   g_loop = draw_sphere_iter;
   g_data = calloc( 1, sizeof( struct DEMO_SPHERE_DATA ) );
   */

#ifdef RETROFLAT_API_LIBNDS
   g_loop = draw_cube_iter;
   g_data = calloc( 1, sizeof( struct DEMO_SPHERE_DATA ) );
#else
   if( NULL == g_loop ) {
      j = rand() % i;
      g_loop = gc_demo_loops[j];
      debug_printf( 3, "auto-selecting demo loop (%d of %d): %s",
         j, i, gc_demo_names[j] );
      g_data = calloc( 1, gc_demo_data_sz[j] );
   }
#endif

   /* === Main Loop === */

   assert( NULL != g_loop );
   assert( NULL != g_data );

   retroflat_loop( (retroflat_loop_iter)g_loop, NULL, g_data );

cleanup:

#ifndef RETROFLAT_OS_WASM

   debug_printf( 1, "shutting down..." );

   retrogxc_shutdown();

   retroflat_shutdown( retval );

   free( g_data );

   logging_shutdown();

#endif /* !RETROFLAT_OS_WASM */

   return retval;
}
END_OF_MAIN()

