
#ifndef POLY_H
#define POLY_H

void poly_cube(
   int scale,
   RETROFLAT_COLOR color_bk, RETROFLAT_COLOR color_ft, RETROFLAT_COLOR color_rt,
   RETROFLAT_COLOR color_lt, RETROFLAT_COLOR color_tp, RETROFLAT_COLOR color_bt
);
void poly_cube_tex(
   struct RETROFLAT_BITMAP* tex,
   int scale,
   RETROFLAT_COLOR color_bk, RETROFLAT_COLOR color_ft, RETROFLAT_COLOR color_rt,
   RETROFLAT_COLOR color_lt, RETROFLAT_COLOR color_tp, RETROFLAT_COLOR color_bt
);
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
   RETROFLAT_COLOR color1, RETROFLAT_COLOR color2, const float radius );
void poly_ortho_skybox( const float* color, struct RETROFLAT_BITMAP* tex );
void poly_water_skybox();

#endif /* !POLY_H */

