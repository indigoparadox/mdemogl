
#include <math.h>

#include "demos.h"
#include "poly.h"

void poly_cube(
   const float color_bk[], const float color_ft[], const float color_rt[],
   const float color_lt[], const float color_tp[], const float color_bt[]
) {
   /* Note that the normals begin in the middle of the face and line
      * up with the face with the most similar vertexes.
      */

   /* TODO: Break into triangles. */

   /* BACK */
   glBegin( GL_POLYGON );
   glNormal3f(  0, 0, 1.0f );
   glColor3fv( color_bk );
   glVertex3f(  0.5, -0.5, 0.5 );
   glVertex3f(  0.5,  0.5, 0.5 );
   glVertex3f( -0.5,  0.5, 0.5 );
   glVertex3f( -0.5, -0.5, 0.5 );
   glEnd();
   
   /* RIGHT */
   glBegin( GL_POLYGON );
   glColor3fv( color_rt );
   glColor3f(  1.0,  0.0,  1.0 );
   glVertex3f( 0.5, -0.5, -0.5 );
   glVertex3f( 0.5,  0.5, -0.5 );
   glVertex3f( 0.5,  0.5,  0.5 );
   glVertex3f( 0.5, -0.5,  0.5 );
   glEnd();
   
   /* LEFT */
   glBegin( GL_POLYGON );
   glNormal3f(  -1.0f, 0, 0 );
   glColor3fv( color_lt );
   glVertex3f( -0.5, -0.5,  0.5 );
   glVertex3f( -0.5,  0.5,  0.5 );
   glVertex3f( -0.5,  0.5, -0.5 );
   glVertex3f( -0.5, -0.5, -0.5 );
   glEnd();

   /* FRONT */
   glBegin( GL_POLYGON );
   glNormal3f(  0, 0, -1.0f );
   glColor3fv( color_ft );
   glVertex3f( -0.5, -0.5, -0.5 );
   glVertex3f( -0.5,  0.5, -0.5 );
   glVertex3f(  0.5,  0.5, -0.5 );
   glVertex3f(  0.5, -0.5, -0.5 );
   glEnd();
   
   /* TOP */
   glBegin( GL_POLYGON );
   glNormal3f(  0, 1.0f, 0 );
   glColor3fv( color_tp );
   glVertex3f(  0.5,  0.5,  0.5 );
   glVertex3f(  0.5,  0.5, -0.5 );
   glVertex3f( -0.5,  0.5, -0.5 );
   glVertex3f( -0.5,  0.5,  0.5 );
   glEnd();
   
   /* BOTTOM */
   glBegin( GL_POLYGON );
   glNormal3f(  0, -1.0f, 0 );
   glColor3fv( color_bt );
   glVertex3f(  0.5, -0.5, -0.5 );
   glVertex3f(  0.5, -0.5,  0.5 );
   glVertex3f( -0.5, -0.5,  0.5 );
   glVertex3f( -0.5, -0.5, -0.5 );
   glEnd();
}

void poly_well(
   const float color[], const float radius, const float ang_iter
) {
   float
      x = 0,
      x_next = 0,
      z = 0,
      z_next = 0,
      ang = 0;

   for( ang = 0 ; 2 * RETROFLAT_PI > ang ; ang += ang_iter ) {
      x = radius * cos( ang );
      x_next = radius * cos( ang + ang_iter );
      z = radius * sin( ang );
      z_next = radius * sin( ang + ang_iter );

      /* Inner Wall */
      glBegin( GL_QUADS );
      glColor3fv( color );
      glNormal3f( x * -1.0f,  0, z * -1.0f );
      glVertex3f( x_next,  2.0f, z_next );
      glVertex3f( x,       2.0f, z );
      glVertex3f( x,          0, z );
      glVertex3f( x_next,     0, z_next );
      glEnd();

      /* Upper Lip */
      glBegin( GL_QUADS );
      glColor3fv( color );
      glNormal3f( x * -1.0f,  1, z * -1.0f );
      glVertex3f( x_next * 1.5f, 2.0f, z_next * 1.5f );
      glVertex3f( x * 1.5f,      2.0f, z * 1.5f );
      glVertex3f( x,             2.0f, z );
      glVertex3f( x_next,        2.0f, z_next );
      glEnd();
   }
}

void poly_water_ring(
   const float color[], const float radius, const float radius_iter,
   const float ang_iter, float freq_mod, float amp_mod, float peak_offset
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
      for( radi = 0 ; radius > radi ; radi += radius_iter
      ) {
         radi_next = radi + radius_iter;

         y = (sin( (radi + peak_offset)
            /* Modulate wave frequency. */
            * freq_mod) 
            /* Modulate wave height. */
            * amp_mod) 
            /* Keep it above zero. */
            + 1.0f;
         y_next =
            (sin( (radi_next + peak_offset)
            /* Modulate wave frequency. */
            * freq_mod) 
            /* Modulate wave height. */
            * amp_mod) 
            /* Keep it above 0. */
            + 1.0f;

         /* Water color and lighting. */
         glNormal3f( cos( ang ), y, 0 );
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

void poly_sphere_checker( const float color1[], const float color2[] ) {
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
            sin( ang_xy ) * cos( ang_xz ),
            cos( ang_xy ),
            sin( ang_xy ) * sin( ang_xz ) );
         glVertex3f( 
            sin( ang_xy + DEMO_SPHERE_INC_XY ) * cos( ang_xz ),
            cos( ang_xy + DEMO_SPHERE_INC_XY ),
            sin( ang_xy + DEMO_SPHERE_INC_XY ) * sin( ang_xz ) );
         glVertex3f( 
            sin( ang_xy + DEMO_SPHERE_INC_XY ) 
               * cos( ang_xz + DEMO_SPHERE_INC_XZ ),
            cos( ang_xy + DEMO_SPHERE_INC_XY ),
            sin( ang_xy + DEMO_SPHERE_INC_XY ) 
               * sin( ang_xz + DEMO_SPHERE_INC_XZ ) );

         /* Triangle 2 */
         glVertex3f( 
            sin( ang_xy + DEMO_SPHERE_INC_XY ) 
               * cos( ang_xz + DEMO_SPHERE_INC_XZ ),
            cos( ang_xy + DEMO_SPHERE_INC_XY ),
            sin( ang_xy + DEMO_SPHERE_INC_XY ) 
               * sin( ang_xz + DEMO_SPHERE_INC_XZ ) );
         glVertex3f(
            sin( ang_xy ) * cos( ang_xz + DEMO_SPHERE_INC_XZ ),
            cos( ang_xy ),
            sin( ang_xy ) * sin( ang_xz + DEMO_SPHERE_INC_XZ ) );
         glVertex3f( 
            sin( ang_xy ) * cos( ang_xz ),
            cos( ang_xy ),
            sin( ang_xy ) * sin( ang_xz ) );

         glEnd();
      }
   }
}

