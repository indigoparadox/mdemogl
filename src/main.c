
#include <time.h>
#include <stdlib.h>

#define MAUG_C
#include <maug.h>
#include <retroflt.h>
#include <retroani.h>

#include "demos.h"

retroflat_loop_iter g_loop = NULL;
void* g_data = NULL;

static MERROR_RETVAL demo_cli_cb(
   const char* arg, struct RETROFLAT_ARGS* args
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
   const char* arg, struct RETROFLAT_ARGS* args
) {
   g_timer = 1;
   debug_printf( 3, "timer enabled" );
   return RETROFLAT_OK;
}

static MERROR_RETVAL demo_obj_cli_cb(
   const char* arg, struct RETROFLAT_ARGS* args
) {
   if( 0 != strncmp( MAUG_CLI_SIGIL, arg, MAUG_CLI_SIGIL_SZ ) ) {
      strncpy( g_demo_obj_name, arg, DEMO_OBJ_NAME_SZ_MAX );
      debug_printf( 3, "demo obj manually selected: %s", g_demo_obj_name );
   }

   return RETROFLAT_OK;
}

static MERROR_RETVAL demo_dump_cli_cb(
   const char* arg, struct RETROFLAT_ARGS* args
) {
   if( 0 != strncmp( MAUG_CLI_SIGIL, arg, MAUG_CLI_SIGIL_SZ ) ) {
      strncpy( g_demo_dump_name, arg, DEMO_OBJ_NAME_SZ_MAX );
      debug_printf( 3, "demo dump manually selected: %s", g_demo_dump_name );
   }
   return RETROFLAT_OK;
}

static MERROR_RETVAL demo_wire_cli_cb(
   const char* arg, struct RETROFLAT_ARGS* args
) {
   g_demo_flags |= DEMO_FLAG_WIRE;
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

   maug_mzero( &args, sizeof( struct RETROFLAT_ARGS ) );

   args.title = "mdemo";
   args.assets_path = "assets";
   args.flags = RETROFLAT_FLAGS_KEY_REPEAT;

   maug_add_arg(
      MAUG_CLI_SIGIL "t", MAUG_CLI_SIGIL_SZ + 1,
      "show the on-screen timer", 0,
      (maug_cli_cb)demo_timer_cli_cb, NULL, &args );

   maug_add_arg(
      MAUG_CLI_SIGIL "o", MAUG_CLI_SIGIL_SZ + 1,
      "load the specified object", 0,
      (maug_cli_cb)demo_obj_cli_cb, NULL, &args );

   maug_add_arg(
      MAUG_CLI_SIGIL "d", MAUG_CLI_SIGIL_SZ + 1,
      "dump demo object", 0,
      (maug_cli_cb)demo_dump_cli_cb, NULL, &args );

   maug_add_arg(
      MAUG_CLI_SIGIL "w", MAUG_CLI_SIGIL_SZ + 1,
      "view in wireframe mode", 0,
      (maug_cli_cb)demo_wire_cli_cb, NULL, &args );

   /* Add demos to CLI parser. */
   for( i = 0 ; '\0' != gc_demo_names[i][0] ; i++ ) {
      maug_add_arg(
         gc_demo_names[i], 0, "display this demo", 0,
         (maug_cli_cb)demo_cli_cb, NULL, &args );
   }

   retval = retroflat_init( argc, argv, &args );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

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

   retroflat_loop( (retroflat_loop_iter)g_loop, NULL, g_data );

cleanup:

#ifndef RETROFLAT_OS_WASM
   
   debug_printf( 1, "shutting down..." );

   retroflat_shutdown( retval );

   free( g_data );

   logging_shutdown();

#endif /* !RETROFLAT_OS_WASM */

   return retval;
}
END_OF_MAIN()

