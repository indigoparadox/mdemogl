
#ifndef POLY_H
#define POLY_H

void poly_cube(
   const float color_bk[], const float color_ft[], const float color_rt[],
   const float color_lt[], const float color_tp[], const float color_bt[] );
void poly_well(
   const float color[], const float radius_outer, const float radius_inner,
   const float height, const float ang_iter );
void poly_water_ring(
   const float color[], const float height,
   const float radius, const float radius_iter,
   const float ang_iter, const float normal_mod,
   float freq_mod, float amp_mod, float peak_offset );
void poly_water_sheet(
   const float color[],
   const float width, const float depth, const float x_iter,
   float freq_mod, float amp_mod, float peak_offset );
void poly_sphere_checker(
   const float color1[], const float color2[], const float radius );
void poly_ortho_skybox( const float* color );
void poly_water_skybox();

#endif /* !POLY_H */

