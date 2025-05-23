
#ifndef POLY_H
#define POLY_H

void poly_cube(
   mfix_t scale,
   RETROFLAT_COLOR color_bk, RETROFLAT_COLOR color_ft, RETROFLAT_COLOR color_rt,
   RETROFLAT_COLOR color_lt, RETROFLAT_COLOR color_tp, RETROFLAT_COLOR color_bt
);
void poly_cube_tex(
   retroflat_blit_t* tex, mfix_t scale, mfix_t tx_scale,
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
   retroflat_blit_t* tex, mfix_t tex_scale, RETROFLAT_COLOR color,
   const mfix_t width, const mfix_t depth, const mfix_t x_iter,
   float freq_mod, float amp_mod, mfix_t peak_offset );
void poly_sphere_checker( RETROFLAT_COLOR color1, RETROFLAT_COLOR color2 );
void poly_ortho_skybox(
   retroflat_blit_t* tex, RETROFLAT_COLOR color1, RETROFLAT_COLOR color2 );
void poly_water_skybox();

#endif /* !POLY_H */

