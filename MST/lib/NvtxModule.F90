! NVTX annotations share the default domain with the CUDA matrix ranges.
! Shared by MST and its drivers; non-CUDA builds have no NVTX dependency.
! Logical nesting is flattened by the C bridge: a child pauses its parent,
! and ending the child resumes the parent in a new, non-overlapping segment.
! Reported times are exclusive; instance counts and averages describe segments.
module NvtxModule
#if defined(CUDA) || defined(ACCELERATOR_CUDA_C)
   use, intrinsic :: iso_c_binding, only : c_char, c_null_char
#endif
   implicit none
   private
   public :: nvtxStartRange, nvtxEndRange

#if defined(CUDA) || defined(ACCELERATOR_CUDA_C)
   interface
      subroutine mst_nvtx_range_push(name) bind(C, name='mst_nvtx_range_push')
         import :: c_char
         character(kind=c_char), intent(in) :: name(*)
      end subroutine mst_nvtx_range_push

      subroutine mst_nvtx_range_pop() bind(C, name='mst_nvtx_range_pop')
      end subroutine mst_nvtx_range_pop
   end interface
#endif

contains

   subroutine nvtxStartRange(name)
      character(len=*), intent(in) :: name
#if defined(CUDA) || defined(ACCELERATOR_CUDA_C)
      call mst_nvtx_range_push(trim(name)//c_null_char)
#endif
   end subroutine nvtxStartRange

   subroutine nvtxEndRange()
#if defined(CUDA) || defined(ACCELERATOR_CUDA_C)
      call mst_nvtx_range_pop()
#endif
   end subroutine nvtxEndRange

end module NvtxModule
