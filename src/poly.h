
#ifndef POLY_H
#define POLY_H

void poly_cube(
   const float color_bk[], const float color_ft[], const float color_rt[],
   const float color_lt[], const float color_tp[], const float color_bt[] );
void poly_well(
   const float color[], const float radius, const float ang_iter );
void poly_water_ring(
   const float color[], const float radius, const float radius_iter,
   const float ang_iter, float freq_mod, float amp_mod, float peak_offset );
void poly_water_sheet(
   const float color[],
   const float width, const float depth, const float x_iter,
   float freq_mod, float amp_mod, float peak_offset );
void poly_sphere_checker( const float color1[], const float color2[] );

#endif /* !POLY_H */

