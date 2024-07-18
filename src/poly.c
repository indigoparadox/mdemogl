
#include <math.h>

#include "demos.h"
#include "poly.h"

void poly_cube(
   float scale,
   const float color_bk[], const float color_ft[], const float color_rt[],
   const float color_lt[], const float color_tp[], const float color_bt[]
) {
   poly_cube_tex( NULL, scale, color_bk, color_ft, color_rt, color_lt,
      color_tp, color_bt );
}

void poly_cube_tex(
   struct RETROFLAT_BITMAP* tex,
   float scale,
   const float color_bk[], const float color_ft[], const float color_rt[],
   const float color_lt[], const float color_tp[], const float color_bt[]
) {
#ifndef RETROGLU_NO_TEXTURES
   GLenum error = GL_NO_ERROR;
#endif /* !RETROGLU_NO_TEXTURES */

   /* Note that the normals begin in the middle of the face and line
      * up with the face with the most similar vertexes.
      */

   /*
   error = glGetError();
   assert( GL_NO_ERROR == error );
   */

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

   /* BACK */
   glBegin( GL_TRIANGLES );
   glNormal3i(      0,     0, scale );
   glColor3fv( color_bk );
   glTexCoord2i(    0,     0 );
   glVertex3f(   scale,  scale, scale );
   glTexCoord2i(    1,     0 );
   glVertex3f(  -scale,  scale, scale );
   glTexCoord2i(    0,     1 );
   glVertex3f(   scale, -scale, scale );
   glEnd();

   glBegin( GL_TRIANGLES );
   glTexCoord2i(    1,     0 );
   glVertex3f(  -scale,  scale, scale );
   glTexCoord2i(    1,     1 );
   glVertex3f(  -scale, -scale, scale );
   glTexCoord2i(    0,     1 );
   glVertex3f(   scale, -scale, scale );
   glEnd();
   
   /* RIGHT */
   glBegin( GL_TRIANGLES );
   glNormal3i(   scale,     0,     0 );
   glColor3fv( color_rt );
   glTexCoord2i(           1,     1 );
   glVertex3f(   scale, -scale, -scale );
   glTexCoord2i(           1,     0 );
   glVertex3f(   scale,  scale, -scale );
   glTexCoord2i(           0,     0 );
   glVertex3f(   scale,  scale,  scale );

   glTexCoord2i(           0,     0 );
   glVertex3f(   scale,  scale,  scale );
   glTexCoord2i(           0,     1 );
   glVertex3f(   scale, -scale,  scale );
   glTexCoord2i(           1,     1 );
   glVertex3f(   scale, -scale, -scale );
   glEnd();
   
   /* LEFT */
   glBegin( GL_TRIANGLES );
   glNormal3f( -1.0f,     0,    0 );
   glColor3fv( color_lt );
   glTexCoord2i(           0,     1 );
   glVertex3f( -scale, -scale,  scale );
   glTexCoord2i(           0,     0 );
   glVertex3f( -scale,  scale,  scale );
   glTexCoord2i(           1,     0 );
   glVertex3f( -scale,  scale, -scale );

   glTexCoord2i(           1,     0 );
   glVertex3f( -scale,  scale, -scale );
   glTexCoord2i(           1,     1 );
   glVertex3f( -scale, -scale, -scale );
   glTexCoord2i(           0,     1 );
   glVertex3f( -scale, -scale,  scale );
   glEnd();

   /* FRONT */
   glBegin( GL_TRIANGLES );
   glNormal3f(     0,     0, -1.0f );
   glColor3fv( color_ft );
   glTexCoord2i(    1,      1 );
   glVertex3f( -scale, -scale, -scale );
   glTexCoord2i(    1,      0 );
   glVertex3f( -scale,  scale, -scale );
   glTexCoord2i(    0,      0 );
   glVertex3f(  scale,  scale, -scale );

   glTexCoord2i(    0,      0 );
   glVertex3f(  scale,  scale, -scale );
   glTexCoord2i(    0,      1 );
   glVertex3f(  scale, -scale, -scale );
   glTexCoord2i(    1,      1 );
   glVertex3f( -scale, -scale, -scale );
   glEnd();
   
   /* TOP */
   glBegin( GL_TRIANGLES );
   glNormal3f(     0,  1.0f,     0 );
   glColor3fv( color_tp );
   glVertex3f(  scale,  scale,  scale );
   glVertex3f(  scale,  scale, -scale );
   glVertex3f( -scale,  scale, -scale );

   glVertex3f( -scale,  scale, -scale );
   glVertex3f( -scale,  scale,  scale );
   glVertex3f(  scale,  scale,  scale );
   glEnd();
   
   /* BOTTOM */
   glBegin( GL_TRIANGLES );
   glNormal3f(     0, -1.0f,     0 );
   glColor3fv( color_bt );
   glVertex3f(  scale, -scale, -scale );
   glVertex3f(  scale, -scale,  scale );
   glVertex3f( -scale, -scale,  scale );

   glVertex3f( -scale, -scale,  scale );
   glVertex3f( -scale, -scale, -scale );
   glVertex3f(  scale, -scale, -scale );
   glEnd();

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
}

void poly_well(
   const float color[], const float radius_outer, const float radius_inner,
   const float height, const float ang_iter
) {
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
}

void poly_water_ring(
   const float color[], const float height,
   const float radius, const float radius_iter,
   const float ang_iter, const float normal_mod,
   float freq_mod, float amp_mod, float peak_offset
) {
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
}

void poly_water_sheet(
   const float color[],
   const float width, const float depth, const float x_iter,
   float freq_mod, float amp_mod, float peak_offset
) {
   float x = 0,  
      x_next = 0,
      y = 0,
      y_next = 0;

   /* Flat rectangle of even waves based on sine. */
   for( x = 0 - (width / 2) ; width / 2 > x ; x += x_iter ) {
      x_next = x + x_iter;
      y = (sin( x + peak_offset ) * amp_mod) + 1.0f;
      y_next = (sin( x_next + peak_offset ) * amp_mod) + 1.0f;

      assert( 0 <= y );

      glBegin( GL_TRIANGLES );
      glNormal3f( 0.75f,           y / 4,      0 );
      glColor3fv( color );
      glVertex3f( x,           y, 0 - (depth / 2) ); /* Far Left */
      glVertex3f( x,           y,  depth / 2 ); /* Near Left */
      glVertex3f( x_next, y_next,  depth / 2 ); /* Near Right */

      glVertex3f( x_next, y_next,  depth / 2 ); /* Near Right */
      glVertex3f( x_next, y_next, 0 - (depth / 2) ); /* Far Right */
      glVertex3f( x,           y, 0 - (depth / 2) ); /* Far Left */
      glEnd();
   }
}

void poly_sphere_checker(
   const float color1[], const float color2[], const float radius
) {
   float ang_xy = 0;
   float ang_xz = 0;
   int even_col = 1;

   /* Generate a sphere using the cos() as X and sin() as Y of angles at 
      * DEMO_SPHERE_INC_XY and DEMO_SPHERE_INC_XZ increments around the
      * origin (0, 0).
      */
   for(
      ang_xz = 2 * RETROFLAT_PI ;
      0 <= ang_xz ;
      ang_xz -= DEMO_SPHERE_INC_XZ
   ) {
      for(
         ang_xy = 0 ;
         2 * RETROFLAT_PI > ang_xy ;
         ang_xy += DEMO_SPHERE_INC_XY
      ) {
         glBegin( GL_TRIANGLES );
         even_col = even_col ? 0 : 1;

         /* Checkerboard pattern. */
         if( even_col ) {
            glColor3fv( color1 );
         } else {
            glColor3fv( color2 );
         }

         /* Setup a normal for each face (2 triangles). */
         /* We're just copying the first vertex here... that should make
            * each face reflect in its own direction, which is what we want
            * for this demo.
            */
         glNormal3f( 
            sin( ang_xy ) * cos( ang_xz ),
            cos( ang_xy ),
            sin( ang_xy ) * sin( ang_xz ) );

         /* Quad panels at equal intervals around two circles intersecting
            * on orthogonal planes.
            */

         /* Each checkerboard square is 2 triangles: */

         /* Triangle 1 */
         glVertex3f( 
            sin( ang_xy ) * cos( ang_xz ) * radius,
            cos( ang_xy ) * radius,
            sin( ang_xy ) * sin( ang_xz ) * radius );
         glVertex3f( 
            sin( ang_xy + DEMO_SPHERE_INC_XY ) * cos( ang_xz ) * radius,
            cos( ang_xy + DEMO_SPHERE_INC_XY ) * radius,
            sin( ang_xy + DEMO_SPHERE_INC_XY ) * sin( ang_xz ) * radius );
         glVertex3f( 
            sin( ang_xy + DEMO_SPHERE_INC_XY ) 
               * cos( ang_xz + DEMO_SPHERE_INC_XZ ) * radius,
            cos( ang_xy + DEMO_SPHERE_INC_XY ) * radius,
            sin( ang_xy + DEMO_SPHERE_INC_XY ) 
               * sin( ang_xz + DEMO_SPHERE_INC_XZ ) * radius );

         /* Triangle 2 */
         glVertex3f( 
            sin( ang_xy + DEMO_SPHERE_INC_XY ) 
               * cos( ang_xz + DEMO_SPHERE_INC_XZ ) * radius,
            cos( ang_xy + DEMO_SPHERE_INC_XY ) * radius,
            sin( ang_xy + DEMO_SPHERE_INC_XY ) 
               * sin( ang_xz + DEMO_SPHERE_INC_XZ ) * radius );
         glVertex3f(
            sin( ang_xy ) * cos( ang_xz + DEMO_SPHERE_INC_XZ ) * radius,
            cos( ang_xy ) * radius,
            sin( ang_xy ) * sin( ang_xz + DEMO_SPHERE_INC_XZ ) * radius );
         glVertex3f( 
            sin( ang_xy ) * cos( ang_xz ) * radius,
            cos( ang_xy ) * radius,
            sin( ang_xy ) * sin( ang_xz ) * radius );

         glEnd();
      }
   }
}

void poly_ortho_skybox( const float* color, struct RETROFLAT_BITMAP* tex ) {
   /*
   GLenum error = GL_NO_ERROR;
   */

   glColor3fv( color );

   /* Create a skybox. Note the normals, crucial for making the sides
      * show up properly in lighting.
      * This is a frustum shape to enhance its "3D" appearance in ortho
      * rendering.
      */

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
      /*
      error = glGetError();
      assert( GL_NO_ERROR == error );
      */
      glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   }
#endif /* !RETROGLU_NO_TEXTURES */

   /* TODO: Break into triangles. */

   /* Back Face */
   glBegin( GL_QUADS );
   glNormal3f(  0, 0, 1.0f );
   glTexCoord2i(   0,     0 );
   glVertex3f(  1.0f,  1.0f, -10.0f ); /* Top Right */
   glTexCoord2i(   1,     0 );
   glVertex3f( -1.0f,  1.0f, -10.0f ); /* Top Left */
   glTexCoord2i(   1,     1 );
   glVertex3f( -1.0f, -1.0f, -10.0f ); /* Bottom Left */
   glTexCoord2i(   0,     1 );
   glVertex3f(  1.0f, -1.0f, -10.0f ); /* Bottom Right */
   glEnd();

   /* Bottom Face */
   glBegin( GL_QUADS );
   glNormal3f(  0, 1.0f, 0 );
   glVertex3f(  1.0f, -1.0f, -10.0f );
   glVertex3f( -1.0f, -1.0f, -10.0f );
   glVertex3f( -2.0f, -2.0f,  10.0f );
   glVertex3f(  2.0f, -2.0f,  10.0f );
   glEnd();

   /* Right Face */
   glBegin( GL_QUADS );
   glNormal3f(  -1.0f, 0, 0 );
   glTexCoord2i(   0,     0 );
   glVertex3f(  2.0f,  2.0f,  10.0f );
   glTexCoord2i(   1,     0 );
   glVertex3f(  1.0f,  1.0f, -10.0f );
   glTexCoord2i(   1,     1 );
   glVertex3f(  1.0f, -1.0f, -10.0f );
   glTexCoord2i(   0,     1 );
   glVertex3f(  2.0f, -2.0f,  10.0f );
   glEnd();

   /* Top Face */
   glBegin( GL_QUADS );
   glNormal3f(  0, -1.0f, 0 );
   glVertex3f(  2.0f,  2.0f,  10.0f );
   glVertex3f( -2.0f,  2.0f,  10.0f );
   glVertex3f( -1.0f,  1.0f, -10.0f );
   glVertex3f(  1.0f,  1.0f, -10.0f );
   glEnd();

   /* Left Face */
   glBegin( GL_QUADS );
   glNormal3f(  1.0f, 0, 0 );
   glTexCoord2i(   0,     0 );
   glVertex3f( -1.0f,  1.0f, -10.0f );
   glTexCoord2i(   1,     0 );
   glVertex3f( -2.0f,  2.0f,  10.0f );
   glTexCoord2i(   1,     1 );
   glVertex3f( -2.0f, -2.0f,  10.0f );
   glTexCoord2i(   0,     1 );
   glVertex3f( -1.0f, -1.0f, -10.0f );
   glEnd();

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
}

void poly_water_skybox() {

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
}

