#ifndef MST_GREEN_FUNCTION_GPU_HPP
#define MST_GREEN_FUNCTION_GPU_HPP

// All arrays use Fortran column-major ordering and complex double precision.
// phi[s], dphi[s]: (nr, kp, kk); kau: (kk, kk, spins*spins).
// gaunt: (kp, kg, kp); green, dgreen: (nout, kg, spins*spins).
// conjugate_index is zero-based; phase contains the corresponding (-1)^m.
// Returns 1 after downloading the result, or 0 without modifying outputs when
// GPU reconstruction is unavailable. dphi/dgreen may be null without derivatives.
// Set MST_GPU_GREEN=0 to force the CPU/reference path.
extern "C" int mst_reconstruct_green_gpu(
   int nr, int nout, int kp, int kk, int kg, int spins, int derivatives,
   const void *const *phi, const void *const *dphi, const void *kau,
   const void *gaunt, const int *conjugate_index, const int *phase,
   void *green, void *dgreen);

#endif
