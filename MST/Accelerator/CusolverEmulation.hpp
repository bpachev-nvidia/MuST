#ifndef CUSOLVER_EMULATION_HPP
#define CUSOLVER_EMULATION_HPP

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cuda_runtime.h>
#include <cusolverDn.h>
#include "acclib.hpp"

// Match the cuBLAS enable switch. -1 means native FP64; 0 selects the
// library's default fixed mantissa count. Read afresh for each energy.
inline int cusolverEmulationMantissaBits() {
   const char *enabled = std::getenv("CUBLAS_EMULATE_DOUBLE_PRECISION");
   if (!enabled || std::strcmp(enabled, "1") != 0) return -1;

   const char *value = std::getenv("CUBLAS_FIXEDPOINT_EMULATION_MANTISSA_BIT_COUNT");
   if (!value) return 0;

   char *end = nullptr;
   errno = 0;
   const long bits = std::strtol(value, &end, 10);
   if (errno == ERANGE || end == value || *end != '\0' || bits < 0 || bits > INT_MAX) {
      std::fprintf(stderr, "CUBLAS_FIXEDPOINT_EMULATION_MANTISSA_BIT_COUNT "
                           "must be a nonnegative integer, got '%s'\n", value);
      std::exit(EXIT_FAILURE);
   }
   return static_cast<int>(bits);
}

// cuSOLVER's internal cuBLAS handles ignore cuBLAS environment variables.
// Apply these settings before querying the LU workspace size.
inline void configureCusolverEmulation(cusolverDnHandle_t handle, int mantissa_bits) {
#if CUDART_VERSION >= 13020
   if (mantissa_bits < 0) {
      checkCusolverErrors(cusolverDnSetMathMode(handle, CUSOLVER_DEFAULT_MATH));
      return;
   }
   checkCusolverErrors(cusolverDnSetEmulationStrategy(handle, CUDA_EMULATION_STRATEGY_PERFORMANT));
   checkCusolverErrors(cusolverDnSetFixedPointEmulationMantissaControl(
                         handle, CUDA_EMULATION_MANTISSA_CONTROL_FIXED));
   checkCusolverErrors(cusolverDnSetFixedPointEmulationMaxMantissaBitCount(handle, mantissa_bits));
   checkCusolverErrors(cusolverDnSetMathMode(handle, CUSOLVER_FP64_EMULATED_FIXEDPOINT_MATH));
#else
   (void)handle;
   if (mantissa_bits >= 0) {
      std::fprintf(stderr, "cuSOLVER FP64 emulation requires CUDA Toolkit 13.2 or newer\n");
      std::exit(EXIT_FAILURE);
   }
#endif
}

#endif
