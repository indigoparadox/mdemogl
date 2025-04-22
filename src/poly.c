
#include <math.h>

#include "demos.h"
#include "poly.h"

void poly_cube(
   int scale,
   RETROFLAT_COLOR color_bk, RETROFLAT_COLOR color_ft, RETROFLAT_COLOR color_rt,
   RETROFLAT_COLOR color_lt, RETROFLAT_COLOR color_tp, RETROFLAT_COLOR color_bt
) {
   poly_cube_tex( NULL, scale, color_bk, color_ft, color_rt, color_lt,
      color_tp, color_bt );
}

/* === */

void poly_cube_tex(
   retroflat_blit_t* tex,
   int scale,
   RETROFLAT_COLOR color_bk, RETROFLAT_COLOR color_ft, RETROFLAT_COLOR color_rt,
   RETROFLAT_COLOR color_lt, RETROFLAT_COLOR color_tp, RETROFLAT_COLOR color_bt
) {
#ifndef RETROGLU_NO_TEXTURES
   /* GLenum error = GL_NO_ERROR; */
#endif /* !RETROGLU_NO_TEXTURES */

   /* Note that the normals begin in the middle of the face and line
      * up with the face with the most similar vertexes.
      */

   /*
   error = glGetError();
   assert( GL_NO_ERROR == error );
   */

#if 0
   glPushMatrix();

#ifndef RETROGLU_NO_TEXTURES
   if( NULL != tex ) {
      maug_mlock( tex->tex.bytes_h, tex->tex.bytes );
      assert( NULL != tex->tex.bytes );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
         RETROANI_TILE_W, RETROANI_TILE_H, 0,
         GL_RGBA, GL_UNSIGNED_BYTE,
         tex->tex.bytes ); 
#if 0
      glBindTexture( GL_TEXTURE_2D, tex->tex.id );
#endif
      error = glGetError();
      assert( GL_NO_ERROR == error );
      glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   }
#endif /* !RETROGLU_NO_TEXTURES */
#endif

   /* BACK */
   retro3d_tri_begin( color_bk, RETRO3D_TRI_FLAG_NORMAL_Z );
   /* glColor3fv( color_bk ); */
   retro3d_vx(   scale,  scale, scale, 0, 0 );
   retro3d_vx(  -scale,  scale, scale, 1, 0 );
   retro3d_vx(   scale, -scale, scale, 0, 1 );
   retro3d_tri_end();

   retro3d_tri_begin( color_bk, RETRO3D_TRI_FLAG_NORMAL_Z );
   retro3d_vx(  -scale,  scale, scale, 1, 0 );
   retro3d_vx(  -scale, -scale, scale, 1, 1 );
   retro3d_vx(   scale, -scale, scale, 0, 1 );
   retro3d_tri_end();
   
   /* RIGHT */
   retro3d_tri_begin( color_rt, RETRO3D_TRI_FLAG_NORMAL_X );
   /* glColor3fv( color_rt ); */
   retro3d_vx(   scale, -scale, -scale, 1, 1 );
   retro3d_vx(   scale,  scale, -scale, 1, 0 );
   retro3d_vx(   scale,  scale,  scale, 0, 0 );
   retro3d_tri_end();

   retro3d_tri_begin( color_rt, RETRO3D_TRI_FLAG_NORMAL_X );
   retro3d_vx(   scale,  scale,  scale, 0, 0 );
   retro3d_vx(   scale, -scale,  scale, 0, 1 );
   retro3d_vx(   scale, -scale, -scale, 1, 1 );
   retro3d_tri_end();
   
   /* LEFT */
   retro3d_tri_begin(
      color_lt, RETRO3D_TRI_FLAG_NORMAL_X | RETRO3D_TRI_FLAG_NORMAL_NEG );
   /* glColor3fv( color_lt ); */
   retro3d_vx( -scale, -scale,  scale, 0, 1 );
   retro3d_vx( -scale,  scale,  scale, 0, 0 );
   retro3d_vx( -scale,  scale, -scale, 1, 0 );
   retro3d_tri_end();

   retro3d_tri_begin(
      color_lt, RETRO3D_TRI_FLAG_NORMAL_X | RETRO3D_TRI_FLAG_NORMAL_NEG );
   retro3d_vx( -scale,  scale, -scale, 1, 0 );
   retro3d_vx( -scale, -scale, -scale, 1, 1 );
   retro3d_vx( -scale, -scale,  scale, 0, 1 );
   retro3d_tri_end();

   /* FRONT */
   retro3d_tri_begin(
      color_ft, RETRO3D_TRI_FLAG_NORMAL_Z | RETRO3D_TRI_FLAG_NORMAL_NEG );
   /* glColor3fv( color_ft ); */
   retro3d_vx( -scale, -scale, -scale, 1, 1 );
   retro3d_vx( -scale,  scale, -scale, 1 ,0 );
   retro3d_vx(  scale,  scale, -scale, 0, 0 );
   retro3d_tri_end();

   retro3d_tri_begin(
      color_ft, RETRO3D_TRI_FLAG_NORMAL_Z | RETRO3D_TRI_FLAG_NORMAL_NEG );
   retro3d_vx(  scale,  scale, -scale, 0, 0 );
   retro3d_vx(  scale, -scale, -scale, 0, 1 );
   retro3d_vx( -scale, -scale, -scale, 1, 1 );
   retro3d_tri_end();
   
   /* TOP */
   retro3d_tri_begin( color_tp, RETRO3D_TRI_FLAG_NORMAL_Y );
   /* glColor3fv( color_tp ); */
   retro3d_vx(  scale,  scale,  scale, 0, 0 );
   retro3d_vx(  scale,  scale, -scale, 0, 0 );
   retro3d_vx( -scale,  scale, -scale, 0, 0 );
   retro3d_tri_end();

   retro3d_tri_begin( color_tp, RETRO3D_TRI_FLAG_NORMAL_Y );
   retro3d_vx( -scale,  scale, -scale, 0, 0 );
   retro3d_vx( -scale,  scale,  scale, 0, 0 );
   retro3d_vx(  scale,  scale,  scale, 0, 0 );
   retro3d_tri_end();
   
   /* BOTTOM */
   retro3d_tri_begin(
      color_bt, RETRO3D_TRI_FLAG_NORMAL_Y | RETRO3D_TRI_FLAG_NORMAL_NEG );
   /* glColor3fv( color_bt ); */
   retro3d_vx(  scale, -scale, -scale, 0, 0 );
   retro3d_vx(  scale, -scale,  scale, 0, 0 );
   retro3d_vx( -scale, -scale,  scale, 0, 0 );
   retro3d_tri_end();

   retro3d_tri_begin(
      color_bt, RETRO3D_TRI_FLAG_NORMAL_Y | RETRO3D_TRI_FLAG_NORMAL_NEG );
   retro3d_vx( -scale, -scale,  scale, 0, 0 );
   retro3d_vx( -scale, -scale, -scale, 0, 0 );
   retro3d_vx(  scale, -scale, -scale, 0, 0 );
   retro3d_tri_end();

#if 0
#ifndef RETROGLU_NO_TEXTURES
   if( NULL != tex ) {
#if 0
      glBindTexture( GL_TEXTURE_2D, 0 );
#endif
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
         0, 0, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, NULL );

      maug_munlock( tex->tex.bytes_h, tex->tex.bytes );
   }
#endif /* !RETROGLU_NO_TEXTURES */

   glPopMatrix();
#endif
}

/* === */

void poly_well(
   const float color[], const float radius_outer, const float radius_inner,
   const float height, const float ang_iter
) {
#ifdef RETROFLAT_OPENGL
   float
      x = 0,
      x_next = 0,
      z = 0,
      z_next = 0,
      ang = 0,
      x_outer = 0,
      x_next_outer = 0,
      z_outer = 0,
      z_next_outer = 0;

   for( ang = 0 ; 2 * RETROFLAT_PI > ang ; ang += ang_iter ) {
      x = radius_inner * cos( ang );
      x_next = radius_inner * cos( ang + ang_iter );
      z = radius_inner * sin( ang );
      z_next = radius_inner * sin( ang + ang_iter );
      x_outer = radius_outer * cos( ang );
      x_next_outer = radius_outer * cos( ang + ang_iter );
      z_outer = radius_outer * sin( ang );
      z_next_outer = radius_outer * sin( ang + ang_iter );

      /* Inner Wall */
      glBegin( GL_TRIANGLES );
      glColor3fv( color );
      glNormal3f( x * -1.0f,  0, z * -1.0f );
      glVertex3f( x_next,  height, z_next );
      glVertex3f( x,       height, z );
      glVertex3f( x,          0, z );

      glVertex3f( x,          0, z );
      glVertex3f( x_next,     0, z_next );
      glVertex3f( x_next,  height, z_next );
      glEnd();

      /* Upper Lip */
      glBegin( GL_TRIANGLES );
      glColor3fv( color );
      glNormal3f( x * -1.0f,  1, z * -1.0f );
      glVertex3f( x_next_outer, height, z_next_outer );
      glVertex3f( x_outer,      height, z_outer );
      glVertex3f( x,             height, z );

      glVertex3f( x,             height, z );
      glVertex3f( x_next,        height, z_next );
      glVertex3f( x_next_outer, height, z_next_outer );
      glEnd();

      /* Outer Wall */
      glBegin( GL_TRIANGLES );
      glColor3fv( color );
      glNormal3f( x * -1.0f,  0, z * -1.0f );
      glVertex3f( x_outer,          0, z_outer );
      glVertex3f( x_outer,       height, z_outer );
      glVertex3f( x_next_outer,  height, z_next_outer );

      glVertex3f( x_next_outer,  height, z_next_outer );
      glVertex3f( x_next_outer,     0, z_next_outer );
      glVertex3f( x_outer,          0, z_outer );
      glEnd();


   }
#endif
}

void poly_water_ring(
   const float color[], const float height,
   const float radius, const float radius_iter,
   const float ang_iter, const float normal_mod,
   float freq_mod, float amp_mod, float peak_offset
) {
#ifdef RETROFLAT_OPENGL
   float y = 0,
      y_next = 0,
      ang = 0,
      radi = 0,
      radi_next = 0,
      ang_next = 0;

   /* static int rotate_x = 85; */
   for( ang = 0 ; 2 * RETROFLAT_PI > ang ; ang += ang_iter ) {
      ang_next = ang + ang_iter;
      for( radi = 0 ; radius > radi ; radi += radius_iter ) {
         radi_next = radi + radius_iter;

         y = (sin( (radi + peak_offset)
            /* Modulate wave frequency. */
            * freq_mod) 
            /* Modulate wave height. */
            * amp_mod) 
            /* Keep it above zero. */
            + height;
         y_next =
            (sin( (radi_next + peak_offset)
            /* Modulate wave frequency. */
            * freq_mod) 
            /* Modulate wave height. */
            * amp_mod) 
            /* Keep it above 0. */
            + height;

         /* Water color and lighting. */
         glNormal3f(
            cos( ang ) * normal_mod,
            y,
            sin( ang ) * normal_mod );
         glColor3fv( color );

         /* Water poly. Concentric rings of dual triangles. */
         /* Triangle 1 */
         glBegin( GL_TRIANGLES );
         glVertex3f(
            cos( ang ) * radi,
            y,       
            sin( ang ) * radi
            );
         glVertex3f(
            cos( ang_next ) * radi,
            y,
            sin( ang_next ) * radi
            );
         glVertex3f(
            cos( ang_next ) * radi_next,
            y_next,
            sin( ang_next ) * radi_next
            );
         glEnd();

         /* Triangle 2 */
         glBegin( GL_TRIANGLES );
         glVertex3f(
            cos( ang_next ) * radi_next,
            y_next,
            sin( ang_next ) * radi_next
            );
         glVertex3f(
            cos( ang ) * radi_next,
            y_next,
            sin( ang ) * radi_next
            );
         glVertex3f(
            cos( ang ) * radi,
            y,       
            sin( ang ) * radi
            );
         glEnd();
      }
   }
#endif
}

void poly_water_sheet(
   retroflat_blit_t* tex, RETROFLAT_COLOR color,
   const mfix_t width, const mfix_t depth, const mfix_t x_iter,
   float freq_mod, float amp_mod, float peak_offset
) {
   mfix_t x = 0,  
      x_next = 0,
      y = 0,
      y_next = 0;
   const mfix_t t1 = mfix_from_f( 0.1f );

   /* Flat rectangle of even waves based on sine. */
   for( x = 0 - (width / 2) ; width / 2 > x ; x += x_iter ) {
      x_next = x + x_iter;
      y = (mfix_sin( x + mfix_from_f( peak_offset ) ) *
         amp_mod ) + mfix_from_f( 1.0f );
      y_next = (mfix_sin( x_next + mfix_from_f( peak_offset ) ) *
         amp_mod) + mfix_from_f( 1.0f );

      assert( 0 <= y );

      retro3d_tri_begin( color, 0 );
      retro3d_vx( x,             y, 0 - (depth / 2), t1,  0 ); /* Far Left */
      retro3d_vx( x,             y,       depth / 2,  0,  0 ); /* Near Left */
      retro3d_vx( x_next,   y_next,       depth / 2,  0, t1 ); /* Near Right */
      retro3d_tri_end();

      retro3d_tri_begin( color, 0 );
      retro3d_vx( x_next,   y_next,       depth / 2,  0, t1 ); /* Near Right */
      retro3d_vx( x_next,   y_next, 0 - (depth / 2), t1, t1 ); /* Far Right */
      retro3d_vx( x,             y, 0 - (depth / 2), t1,  0 ); /* Far Left */
      retro3d_tri_end();
   }
}

void poly_sphere_checker( RETROFLAT_COLOR color1, RETROFLAT_COLOR color2 ) {
   mfix_t ang_xy = 0;
   mfix_t ang_xz = 0;
   int even_col = 1;
   int ct = 0;

   /* Generate a sphere using the cos() as X and sin() as Y of angles at 
      * DEMO_SPHERE_INC_XY and DEMO_SPHERE_INC_XZ increments around the
      * origin (0, 0).
      */
   for( ang_xz = 2 * MFIX_PI ; 0 <= ang_xz ; ang_xz -= DEMO_SPHERE_INC_XZ ) {
      for( ang_xy = 0 ; 2 * MFIX_PI > ang_xy ; ang_xy += DEMO_SPHERE_INC_XY ) {
         even_col = even_col ? 0 : 1;

         /* Checkerboard pattern. */
         if( even_col ) {
            retro3d_tri_begin( color1, 0 );
         } else {
            retro3d_tri_begin( color2, 0 );
         }

         /* Setup a normal for each face (2 triangles). */
         /* We're just copying the first vertex here... that should make
            * each face reflect in its own direction, which is what we want
            * for this demo.
            */
         /*
         glNormal3f( 
            sin( ang_xy ) * cos( ang_xz ),
            cos( ang_xy ),
            sin( ang_xy ) * sin( ang_xz ) );
         */

         /* Quad panels at equal intervals around two circles intersecting
            * on orthogonal planes.
            */

         /* Each checkerboard square is 2 triangles: */

         /* Triangle 1 */
         /* TODO: Port retrofp. */

         retro3d_vx(
            mfix_mult( mfix_sin( ang_xy ), mfix_cos( ang_xz ) ),
            mfix_cos( ang_xy ),
            mfix_mult( mfix_sin( ang_xy ), mfix_sin( ang_xz ) ),
            0, 0 );
         retro3d_vx(
            mfix_mult( mfix_sin( ang_xy + DEMO_SPHERE_INC_XY ),
               mfix_cos( ang_xz ) ),
            mfix_cos( ang_xy + DEMO_SPHERE_INC_XY ),
            mfix_mult( mfix_sin( ang_xy + DEMO_SPHERE_INC_XY ),
               mfix_sin( ang_xz ) ),
            0, 0 );
         retro3d_vx(
            mfix_mult( mfix_sin( ang_xy + DEMO_SPHERE_INC_XY ),
               mfix_cos( ang_xz + DEMO_SPHERE_INC_XZ ) ),
            mfix_cos( ang_xy + DEMO_SPHERE_INC_XY ),
            mfix_mult( mfix_sin( ang_xy + DEMO_SPHERE_INC_XY ),
               mfix_sin( ang_xz + DEMO_SPHERE_INC_XZ ) ),
            0, 0 );

         /* Triangle 2 */
         retro3d_vx(
            mfix_mult( mfix_sin( ang_xy + DEMO_SPHERE_INC_XY ),
               mfix_cos( ang_xz + DEMO_SPHERE_INC_XZ ) ),
            mfix_cos( ang_xy + DEMO_SPHERE_INC_XY ),
            mfix_mult( mfix_sin( ang_xy + DEMO_SPHERE_INC_XY ),
               mfix_sin( ang_xz + DEMO_SPHERE_INC_XZ ) ),
            0, 0 );
         retro3d_vx(
            mfix_mult( mfix_sin( ang_xy ),
               mfix_cos( ang_xz + DEMO_SPHERE_INC_XZ ) ),
            mfix_cos( ang_xy ),
            mfix_mult( mfix_sin( ang_xy ),
               mfix_sin( ang_xz + DEMO_SPHERE_INC_XZ ) ),
            0, 0 );
         retro3d_vx(
            mfix_mult( mfix_sin( ang_xy ), mfix_cos( ang_xz ) ),
            mfix_cos( ang_xy ),
            mfix_mult( mfix_sin( ang_xy ), mfix_sin( ang_xz ) ),
            0, 0 );
         ct++;

         retro3d_tri_end();
      }
   }
}

void poly_ortho_skybox(
   retroflat_blit_t* tex, RETROFLAT_COLOR color1, RETROFLAT_COLOR color2
) {
   const mfix_t w = mfix_from_f( 8.0f ),
      h = mfix_from_f( 6.0f ),
      d = mfix_from_f( 10.0f );

   /* Create a skybox.This is a frustum shape to enhance its "3D" appearance in
    * ortho rendering.
    */

   /* Back Face */
   retro3d_tri_begin( color1, 0 );
   retro3d_vx(  w,  h, -d, 0, 0 );
   retro3d_vx( -w,  h, -d, 0, 0 );
   retro3d_vx( -w, -h, -d, 0, 0 );
   retro3d_tri_end();

   retro3d_tri_begin( color1, 0 );
   retro3d_vx( -w, -h, -d, 0, 0 );
   retro3d_vx(  w, -h, -d, 0, 0 );
   retro3d_vx(  w,  h, -d, 0, 0 );
   retro3d_tri_end();

   /* Bottom Face */
   retro3d_tri_begin( color1, 0 );
   retro3d_vx(      w,     -h, -d, 0, 0 );
   retro3d_vx(     -w,     -h, -d, 0, 0 );
   retro3d_vx( 2 * -w, 2 * -h, d, 0, 0 );
   retro3d_tri_end();

   retro3d_tri_begin( color1, 0 );
   retro3d_vx( 2 * -w, 2 * -h, d, 0, 0 );
   retro3d_vx( 2 *  w, 2 * -h, d, 0, 0 );
   retro3d_vx(      w,     -h, d, 0, 0 );
   retro3d_tri_end();

   /* Right Face */
   retro3d_tri_begin( color1, 0 );
   retro3d_vx( 2 *  w, 2 *  h,  d, 0, 0 );
   retro3d_vx(      w,      h, -d, 0, 0 );
   retro3d_vx(      w,     -h, -d, 0, 0 );
   retro3d_tri_end();

   retro3d_tri_begin( color1, 0 );
   retro3d_vx(      w,     -h, -d, 0, 0 );
   retro3d_vx( 2 *  w, 2 * -h,  d, 0, 0 );
   retro3d_vx( 2 *  w, 2 *  h,  d, 0, 0 );
   retro3d_tri_end();

   /* Top Face */
   retro3d_tri_begin( color2, 0 );
   retro3d_vx( 2 *  w, 2 *  h,  d, 0, 0 );
   retro3d_vx( 2 * -w, 2 *  h,  d, 0, 0 );
   retro3d_vx(     -w,      h, -d, 0, 0 );
   retro3d_tri_end();

   retro3d_tri_begin( color2, 0 );
   retro3d_vx(     -w,      h, -d, 0, 0 );
   retro3d_vx(      w,      h, -d, 0, 0 );
   retro3d_vx( 2 *  w, 2 *  h,  d, 0, 0 );
   retro3d_tri_end();

   /* Left Face */
   retro3d_tri_begin( color2, 0 );
   retro3d_vx(     -w,      h, -d, 0, 0 );
   retro3d_vx( 2 * -w, 2 *  h,  d, 0, 0 );
   retro3d_vx( 2 * -w, 2 * -h,  d, 0, 0 );
   retro3d_tri_end();

   retro3d_tri_begin( color2, 0 );
   retro3d_vx( 2 * -w, 2 * -h,  d, 0, 0 );
   retro3d_vx(     -w,     -h, -d, 0, 0 );
   retro3d_vx(     -w,      h, -d, 0, 0 );
   retro3d_tri_end();

}

void poly_water_skybox() {
#if 0
   /* TODO: Use triangles. */

   glBegin( GL_QUADS );
   glColor3fv( RETROGLU_COLOR_GREEN );
   glNormal3f( 0, 1, 0 );
   glVertex3f( -200.0f, 0, -100.0f );
   glVertex3f( -200.0f, 0, 100.0f );
   glVertex3f( 200.0f, 0, 100.0f );
   glVertex3f( 200.0f, 0, -100.0f );
   glEnd();

   glBegin( GL_QUADS );
   glColor3fv( RETROGLU_COLOR_BLUE );
   glNormal3f( 0, 0, 1 );
   glVertex3f( 200.0f, 0, -50.0f );
   glVertex3f( 200.0f, 200.0f, -50.0f );
   glVertex3f( -200.0f, 200.0f, -50.0f );
   glVertex3f( -200.0f, 0, -50.0f );
   glEnd();
#endif
}

