#include "complex.h"
#include "real.h"

__kernel void get_landscape(__global struct Token* tokens, __global struct Complex* stack, int token_count, real minRe, real minIm, real diffRe, real diffIm, int width, int height, __global struct ARGB* colours, int area, int stackmax) 
{
    //Get index for this thread
    int i = get_global_id(0) + get_global_id(1) * width;

    //If index is out of bounds, stop this thread
    if (i >= area)
        return;
    

    //Get value of z for this thread
    struct Complex z;
    z.re = minRe + diffRe / width * get_global_id(0);
    z.im = minIm + diffIm / height * get_global_id(1);

    //Return the colour
    colours[i] =  c_colour(evaluate(tokens, stack, token_count, area, i, z)); //evaluate(tokens, stack, token_count, area, i, z)
}
