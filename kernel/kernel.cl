
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

#define C_ZERO (c_complex(0,0))
#define C_ONE (c_complex(1, 0))
#define C_MINUS_ONE (c_complex(-1, 0))
#define C_I (c_complex(0, 1))
#define C_MINUS_I (c_complex(0, -1))
#define C_NAN (c_complex(NAN, NAN))
#define C_INF (c_complex(INFINITY, INFINITY))

/** 32-bit colour*/
struct ARGB
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};

/** complex number */
struct Complex
{
    real re;
    real im;
};

/** Token */
struct Token
{
    union
    {
        struct Complex value;
        int op;
    };

    int type;
};

real c_abs(struct Complex);

real c_arg(struct Complex);

struct Complex c_exp(struct Complex);

struct Complex c_ln(struct Complex);

float HueToRGB(float, float, float);

inline bool isNaN(struct Complex z) { return isnan(z.re) || isnan(z.im); }

inline bool isInf(struct Complex z) { return (isinf(z.re) || isinf(z.im)) && !isNaN(z); }

inline bool isZero(struct Complex z) { return z.re == 0.0 && z.im == 0.0; }

/**
 * Constructs an ARGB structure
 * @param b blue component
 * @param g green component
 * @param r red component
 * @return the complete ARGB structure with A= 255. b occupies the lowest 8 bits, and a the highest 8 bits.
 */
inline struct ARGB ARGB_constructor(unsigned char r, unsigned char g, unsigned char b)
{
    struct ARGB col;
    col.a = 255;
    col.r = r;
    col.g = g;
    col.b = b;
    return col;
}

/**
 * Constructs a complex number with real and imaginary parts
 * @param re real component
 * @param im imaginary component
 * @return Complex number re + im*i
 */
inline struct Complex c_complex(real re, real im)
{
    struct Complex ans;
    ans.re = re;
    ans.im = im;
    return ans;
}

/**
 * Function gets the sum of two complex numbers
 * @param z the first complex number to add
 * @param w the second complex number to add
 * @return the sum of the two complex numbers, z+w
 */
struct Complex c_add(struct Complex z, struct Complex w)
{
    if (isNaN(z) || isNaN(w))
        return C_NAN;
    return c_complex(z.re + w.re, z.im + w.im);
}

/**
 * Gets the difference between two complex numbers
 * @param z the first complex operand
 * @param w the second complex operand
 * @return the difference as z-w
 */
struct Complex c_sub(struct Complex z, struct Complex w)
{
    if (isNaN(z) || isNaN(w))
        return C_NAN;
    return c_complex(z.re - w.re, z.im - w.im);
}

/**
 * Function gets the product of two complex numbers
 * @param z the first complex operand
 * @param w the second complex operand
 * @return the product of the two numbers, z*w
 */
struct Complex c_mul(struct Complex z, struct Complex w)
{
    if (isNaN(z) || isNaN(w)) 
        return C_NAN; 
    
    if (isInf(z) || isNaN(w))
        return C_INF;
    
    return c_complex(z.re * w.re - z.im * w.im, z.re * w.im + z.im * w.re);
}

/**
 * Gets the complex number multiplied by the imaginary unit, i.
 * @param z multiplicand
 * @return z*i
 */
inline struct Complex c_muli(struct Complex z)
{
    if (isNaN(z)) 
        return C_NAN; 
    
    if (isInf(z))
        return C_INF;
    
    return c_complex(-z.im, z.re);
}

/**
 * Gets the complex number multiplied by -i.
 * @param z multiplicand
 * @return z*-i
 */
inline struct Complex c_mulni(struct Complex z)
{
    if (isNaN(z)) 
        return C_NAN; 
    
    if (isInf(z))
        return C_INF;
    
    return c_complex(z.im, -z.re);
}

/**
 * Gets the complex number multiplied by scalar factor.
 * @param z Complex number
 * @param f Scalar factor
 * @return z*f
 */
inline struct Complex c_mulr(struct Complex z, real f)
{
    if (isNaN(z) || isnan(f)) 
        return C_NAN; 
    
    if (isInf(z) || isinf(f))
        return C_INF;
    
    return c_complex(z.re * f, z.im * f);
}

/**
 * Divides one complex number by another
 * @param z the first complex operand
 * @param w the second complex operand
 * @return z / w
 */
struct Complex c_div(struct Complex z, struct Complex w)
{
    if (isNaN(z) || isNaN(w))
        return C_NAN;
    
    if (isZero(w))
        return C_INF;
    
    if (!isInf(z) && isInf(w))
        return C_ZERO;
    
    double sumsq = w.re * w.re + w.im * w.im;
    return c_complex((z.re * w.re + z.im * w.im) / sumsq, (z.im * w.re - z.re * w.im) / sumsq);
}

/**
 * Raises a complex number to the power of another. Note that for the square root of 
 * a complex number, c_sqrt is faster.
 * @param z the base 
 * @param w the power 
 * @return z to the power of w, z^w
 * @see c_sqrt(struct Complex)
 */
struct Complex c_pow(struct Complex z, struct Complex w)
{
    return c_exp(c_mul(w, c_ln(z)));
}

/**
 * Gets the log of the first operand to the base of the second 
 * @param z the first complex operand
 * @param w the second complex operand
 * @return the log of z to the base w, log_w(z)
 * @see c_ln(struct Complex)
 */
struct Complex c_log(struct Complex z, struct Complex w)
{
    return c_div(c_ln(z), c_ln(w));
}

/**
 * Gets the negative complex number
 * @param z the complec operand
 * @returns -z
 */
struct Complex c_neg(struct Complex z)
{
    if (isNaN(z))
        return C_NAN; 
    
    return c_complex(-z.re, -z.im);
}

/**
 * Gets the complex conjugate of the complex number
 * @param z the complex operand
 * @return the complex conjugate of z
 */
struct Complex c_conj(struct Complex z)
{
    if (isNaN(z))
        return C_NAN;
    
    return c_complex(z.re, -z.im);
}

/**
 * Gets the square root of the complex number using fast algebraic solution. This 
 * function is faster than c_pow and should be used instead of c_pow(z, 0.5).
 * @param z the complex operand
 * @return the square root of z
 */
struct Complex c_sqrt(struct Complex z)
{    
    if (isNaN(z))
        return C_NAN;
    
    if (isZero(z))
        return C_ZERO;

    double t = sqrt((fabs(z.re) + c_abs(z)) / 2.0);
    if (z.re >= 0.0)
        return c_complex(t, z.im / (2.0 * t));
    else 
        return c_complex(fabs(z.im) / (2.0 * t), copysign((real)1.0, z.im) * t);
};

/**
 * Gets the complex number squared, z^2. Calculated as 
 * (a+bi)^2 = a^2-b^2 + 2abi.
 * @param z the number to square
 * @return z^2
 */
struct Complex c_sqrd(struct Complex z)
{
    if (isNaN(z))
        return C_NAN;
    
    return c_complex(z.re * z.re - z.im * z.im, 2*z.re*z.im);
}

/**
 * Gets the natural logarithm of the complex number
 * @param z the complex operand
 * @return the log to the base e of z.
 */
struct Complex c_ln(struct Complex z)
{
    if (isZero(z))
        return C_INF;
    
    return c_complex(log(c_abs(z)), c_arg(z));
}

/**
 * Gets the exponent of the complex number. Note this is faster than c_pow
 * and should be prefered to c_pow(e, z).
 * @param z the complex operand
 * @return e^z
 */
struct Complex c_exp(struct Complex z)
{
    if (isNaN(z)) 
        return C_NAN; 
    
    return c_complex(exp(z.re) * cos(z.im), exp(z.re) * sin(z.im));
}

struct Complex c_sinh(struct Complex z) { if (isNaN(z)) return C_NAN; return c_complex(sinh(z.re)*cos(z.im), cosh(z.re)*sin(z.im)); }

struct Complex c_cosh(struct Complex z) { if (isNaN(z)) return C_NAN; return c_complex(cosh(z.re)*cos(z.im), sinh(z.re)*sin(z.im)); }

struct Complex c_tanh(struct Complex z) 
{ 
    if (isNaN(z)) 
        return C_NAN; 
    
    if (z.re > 20)
        return C_ONE;
    if (z.re < 20)
        return C_MINUS_ONE;
    
    double twore = 2.0*z.re;
    double twoim = 2.0*z.im;
    double denom = cosh(twore)+cos(twoim); 
    return c_complex(sinh(twore) / denom, sin(twoim) / denom); }

struct Complex c_sin(struct Complex z) { if (isNaN(z)) return C_NAN; return c_complex(sin(z.re)*cosh(z.im), cos(z.re)*sinh(z.im)); }

struct Complex c_cos(struct Complex z) { if (isNaN(z)) return C_NAN; return c_complex(cos(z.re)*cosh(z.im), -sin(z.re)*sinh(z.im)); }

struct Complex c_tan(struct Complex z) 
{ 
    if (isNaN(z)) 
        return C_NAN; 
    
    if (z.im > 20)
        return C_I;
    if (z.im < 20)
        return C_MINUS_I;
    
    double twore = 2.0*z.re;
    double twoim = 2.0*z.im;
    double denom = cos(twore)+cosh(twoim); 
    return c_complex(sin(twore) / denom, sinh(twoim) / denom); 
}

struct Complex c_asinh(struct Complex z) { if (isNaN(z)) return C_NAN; return c_ln(c_add(z, c_sqrt(c_add(c_sqrd(z), C_ONE)))); }

struct Complex c_acosh(struct Complex z) { if (isNaN(z)) return C_NAN; return c_ln(c_add(z, c_sqrt(c_sub(c_sqrd(z), C_ONE)))); }

struct Complex c_atanh(struct Complex z) 
{ 
    if (isNaN(z)) 
        return C_NAN; 
    return c_mulr(c_ln(c_div(c_add(C_ONE, z), c_sub(C_ONE, z))), 0.5); 
}

struct Complex c_asin(struct Complex z) 
{ 
    if (isNaN(z)) 
        return C_NAN; 
    return c_mulni(c_ln(c_add(c_mul(c_sqrt(c_sub(C_ONE, z)),c_sqrt(c_add(C_ONE, z))),c_muli(z)))); 
} 

struct Complex c_acos(struct Complex z) { if (isNaN(z)) return C_NAN; return c_mulni(c_ln(c_add(z, c_muli(c_mul(c_sqrt(c_sub(C_ONE, z)),c_sqrt(c_add(C_ONE, z))))))); }

struct Complex c_atan(struct Complex z) 
{ 
    if (isNaN(z)) 
        return C_NAN; 
    
    return c_mulr(c_muli(c_ln(c_div(c_add(C_I,z),c_sub(C_I,z)))), 0.5); 
}

/**
 * Function gets the absolute value of the complex number
 * @param z the complex number
 * @return the absolute value of the complex number z, |z|.
 */
real c_abs(struct Complex z) 
{ 
    if (isNaN(z)) 
        return NAN; 
    
    if (isInf(z)) 
        return INFINITY; 
    
    return hypot(z.re, z.im); 
}

/**
 * Function gets the argument of the complex number
 * @param z the complex number
 * @return the argument, arg(z).
 */
real c_arg(struct Complex z) { return atan2(z.im, z.re); }

/**
 * HLtoRGB converts a hue and lightness value to an ARGB object
 * @param h hue number from 0 to 1
 * @param l lightness value from 0 to 1
 * @return a converted ARGB object
 */
struct ARGB HLtoRGB(float h, float l)
{
    float q = l < 0.5 ? l*2 : 1;

    float p = 2 * l - q;

    float r = max(0.0f, HueToRGB(p, q, h + (1.0f / 3.0f)));
    float g = max(0.0f, HueToRGB(p, q, h));
    float b = max(0.0f, HueToRGB(p, q, h - (1.0f / 3.0f)));

    r = min(r, 1.0f);
    g = min(g, 1.0f);
    b = min(b, 1.0f);

    return ARGB_constructor( r*255, g*255, b*255);
}

/**
 * HueToRGB is called by HLtoRGB to facilitate conversion
 */
float  HueToRGB(float p, float q, float h)
{
    if (h < 0) h += 1;

    if (h > 1 ) h -= 1;

    if (6 * h < 1) return p + ((q - p) * 6 * h);

    if (2 * h < 1 ) return  q;

    if (3 * h < 2) return p + ( (q - p) * 6 * ((2.0f / 3.0f) - h) );

    return p;
}

/**
 * Gets the colour of the complex number as per the domain colouring algorithm
 * @param z the complex number
 * @return the colour, as an ARGB type
 */
struct ARGB c_colour(struct Complex z)
{
    
    //If z is zero, or contains a NaN component display as white
    if (isZero(z) || isNaN(z) )
        return ARGB_constructor (255, 255, 255);

    //Both components are +/-inf
    if (isinf(z.re) && isinf(z.im))
    {
        if (z.re > 0 && z.im > 0)
            return ARGB_constructor (255, 0, 191) ;
        else if (z.re < 0 && z.im > 0)
            return ARGB_constructor (0, 64, 255) ;
        else if (z.re < 0 && z.im < 0) 
            return ARGB_constructor (0, 255, 64) ;
        else
            return ARGB_constructor (255, 191, 0) ;
    }

    //Either one, or the other component is +/1 inf
    if (isInf(z))
    {
        if (isinf(z.re) && z.re > 0)
            return ARGB_constructor (255, 0, 0) ;
        else if (isinf(z.re) && z.re < 0)
            return ARGB_constructor (0, 255, 255) ;
        else if(isinf(z.im) && z.im > 0)
            return ARGB_constructor (128, 0, 255) ;
        else
            return ARGB_constructor (128, 255, 0) ;
    }
                
    real arg = c_arg(z);
    real hue = arg;
    real modarg = log(c_abs(z));
    real lightness;
    
    //Convert argument from -pi to pi --> 0 to 2pi
    if (arg < 0)
        hue = R_2_PI + arg;

    //Convert from 0 to 2pi --> 0 to 1
    hue = 1.0 - hue * R_1_2_PI;
    
    if (modarg < 0)
    {
        lightness = 0.75 - c_abs(z) / 2.0;
    }
    else
    {
        if (!((int)modarg & 1)) //If whole part of modarg is even, 0 --> 1 maps to black --> white
                lightness =  fmin( modarg - floor(modarg), (real)0x1.fffffep-1f ) / 2.0 + 0.25;
        else //If whole part of modarg is odd 0 --> 1 maps to white --> black
                lightness = 0.75 - (modarg - floor(modarg)) / 2.0;
    }

    return HLtoRGB(hue, lightness);
}

struct Complex evaluate(__global struct Token* tokens, __global struct Complex* stack, int token_count, int area, int i, struct Complex z)
{
    int pointer = i;
    
    //variable op constant
    for (int cnt = 0; cnt < token_count; ++cnt)
    {
        switch (tokens[cnt].type)
        {
            case 1:
                stack[pointer] = z;
                pointer += area;
                break;
            case 2:
                switch (tokens[cnt].op)
                {
                    case 0:
                        pointer -= area;
                        stack[pointer-area] = c_add(stack[pointer-area], stack[pointer]);
                        break;
                    case 1:
                        pointer -= area;
                        stack[pointer-area] = c_sub(stack[pointer-area], stack[pointer]);
                        break;
                    case 2:
                        pointer -= area;
                        stack[pointer-area] = c_mul(stack[pointer-area], stack[pointer]);
                        break;
                    case 3:
                        pointer -= area;
                        stack[pointer-area] = c_div(stack[pointer-area], stack[pointer]);
                        break;
                    case 4:
                        pointer -= area;
                        stack[pointer-area] = c_pow(stack[pointer-area], stack[pointer]);
                        break;
                    case 5:
                        pointer -= area;
                        stack[pointer-area] = c_log(stack[pointer-area], stack[pointer]);
                        break;
                    case 6:
                        stack[pointer-area] = c_neg(stack[pointer-area]);
                        break;
                    case 7:
                        stack[pointer-area] = c_conj(stack[pointer-area]);
                        break;
                    case 8:
                        stack[pointer-area] = c_sqrt(stack[pointer-area]);
                        break;
                    case 9:
                        stack[pointer-area] = c_ln(stack[pointer-area]);
                        break;
                    case 10:
                        stack[pointer-area] = c_exp(stack[pointer-area]);
                        break;
                    case 11:
                        stack[pointer-area] = c_sinh(stack[pointer-area]);
                        break;
                    case 12:
                        stack[pointer-area] = c_cosh(stack[pointer-area]);
                        break;
                    case 13:
                        stack[pointer-area] = c_tanh(stack[pointer-area]);
                        break;
                    case 14:
                        stack[pointer-area] = c_sin(stack[pointer-area]);
                        break;
                    case 15:
                        stack[pointer-area] = c_cos(stack[pointer-area]);
                        break;
                    case 16:
                        stack[pointer-area] = c_tan(stack[pointer-area]);
                        break;
                    case 17:
                        stack[pointer-area] = c_asinh(stack[pointer-area]);
                        break;
                    case 18:
                        stack[pointer-area] = c_acosh(stack[pointer-area]);
                        break;
                    case 19:
                        stack[pointer-area] = c_atanh(stack[pointer-area]);
                        break;
                    case 20:
                        stack[pointer-area] = c_asin(stack[pointer-area]);
                        break;
                    case 21:
                        stack[pointer-area] = c_acos(stack[pointer-area]);
                        break;
                    case 22:
                        stack[pointer-area] = c_atan(stack[pointer-area]);
                        break;
                    case 23:
                        stack[pointer-area] = c_complex(c_abs(stack[pointer-area]), 0.0);
                        break;
                    case 24:
                        stack[pointer-area] = c_complex(c_arg(stack[pointer-area]), 0.0);
                        break;
                }
                break;
            case 3:
                stack[pointer] = tokens[cnt].value;
                pointer += area;
                break;
        }
    }
    
    return stack[pointer-area];
}

__kernel void get_landscape(__global struct Token* tokens, __global struct Complex* stack, int token_count, struct Complex min, struct Complex diff, int width, int height, __global struct ARGB* colours, int area, int stackmax) 
{
    //Get index for this thread
    int i = get_global_id(0) + get_global_id(1) * width;

    //If index is out of bounds, stop this thread
    if (i >= area)
        return;
    

    //Get value of z for this thread
    struct Complex z;
    z.re = min.re + diff.re / width * get_global_id(0);
    z.im = min.im + diff.im  - diff.im / height * get_global_id(1); //equivalent to z.im = min.im + diff.im / height * (height - get_global_id(1)); height - get_global_id(1) flips the canvas so min,min is bottom left
    //z.im = min.im + diff.im / height * get_global_id(1);



    //Return the colour
    colours[i] = c_colour(evaluate(tokens, stack, token_count, area, i, z));
}


