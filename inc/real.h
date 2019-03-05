#ifndef KERNEL_REAL_H
#define KERNEL_REAL_H

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define DOUBLE_SUPPORT_AVAILABLE
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#define DOUBLE_SUPPORT_AVAILABLE
#endif

#if defined(DOUBLE_SUPPORT_AVAILABLE)

// double
typedef double real;
#define R_PI M_PI
#define R_PI_2 M_PI_2
#define R_2_PI (6.28318530717958647692)     //  2*pi
#define R_1_2_PI (0.159154943091895335769)  //  1/(2*pi)

#else

// float
typedef float real;
#define R_PI M_PI_F
#define R_PI_2 M_PI_2_F
#define R_2_PI (6.28318530718f)             //  2*pi
#define R_1_2_PI (0.159154943091f)          //  1/(2*pi)

#endif

#endif
