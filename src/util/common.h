#ifndef COMMON_H_
#define COMMON_H_

//=====================
//        TYPES
//=====================

#include <stdbool.h>
#include <stdint.h>

/** Remove ambiguity around the signedness of a char */
typedef unsigned char byte;

/** Like UINT8_MIN/MAX and friends, but for sub byte-sized limits. */
#define UINT2_MIN 0b00
#define UINT2_MAX 0b11
#define UINT3_MIN 0b000
#define UINT3_MAX 0b111
#define UINT4_MIN 0b0000
#define UINT4_MAX 0b1111
#define UINT5_MIN 0b00000
#define UINT5_MAX 0b11111
#define UINT6_MIN 0b000000
#define UINT6_MAX 0b111111
#define UINT7_MIN 0b0000000
#define UINT7_MAX 0b1111111

/** Fill out the unsigned mins, missing from stdint */
#define UINT8_MIN  0
#define UINT16_MIN 0
#define UINT32_MIN 0


//=====================
//      DEFINES
//=====================


/** Fiddle with bits */
#define GET_BIT_IDX(val, bit)   (((val) >> (bit)) & 0x01)
#define GET_3_BITS(val, lsb)    (((val) >> (lsb)) & 0x07)
#define SET_BIT_IDX(val, bit)   ((val) | (0x01 << (bit))) //Not the same as stm32 hal's "SET_BIT"
#define SET_4_BITS(val, lsb)    ((val) | (0x0F << (lsb)))
#define CLEAR_BIT_IDX(val, bit) ((val) & ~(1 << (bit)))
#define LO8(x)                  ((x) & 0xFF)
#define HI8(x)                  (((x)>>8) & 0xFF)
#define TOHI8(x)                (((x)<<8) & 0xFF00)
#define SWAP16(x)               __builtin_bswap16(x) //use built-in over DIY: (HI8(x)|TOHI8(x))


/** Convert the integer n (representing a nybble) to a single uppercase hex digit */
#define NYBBLE_TO_HEX(n)    ((n) < 10 ? '0' + (n) : 'A' + (n) - 10)
/** Do the opposite of NYBBLE_TO_HEX */
#define HEX_TO_NYBBLE(n)    ((n) <= '9' ? (n) - '0' : 10 + (n) - 'A')

/** Useful for ignoring unused function parameters, eg. void foo(UNUSED(int notNeeded)) */
#define UNUSED_PARAM(x)     x __attribute__ ((unused))

/** Get the number of elements in a statically defined array */
#define NUM_ELEMS(x)        (sizeof(x) / sizeof((x)[0]))

/**
 * Force the compiler to tell you the sizeof something at compile time. Note this only produces
 * a warning in arm-none-eabi-gcc (with -Wextra). Couldn't find something that reliably errors!
 * \see https://stackoverflow.com/questions/20979565/how-can-i-print-the-result-of-sizeof-at-compile-time-in-c#comment94589431_53759510
 */
#define SIZEOF_WARNING(data)  char sizeof_is[] = {[sizeof(data)] = ""}; (void)sizeof_is

/**
 * Return the linear interpolation y=f(x) between (x0, y0) and (x1, y1) where x0 <= x <= x1.
 * Eg. LINEAR_INTERPOLATE(50, 40, 60, 100, 200) = 150
 */
#define LINEAR_INTERPOLATE(x, x0, x1, y0, y1) \
                            ((y0)) + ((y1)-(y0)) * ((x) - (x0)) / ((x1)-(x0))

/** Return the largest/smallest of two numbers */
#define MIN( a, b )         ( (a) < (b) ? (a) : (b) )
#define MAX( a, b )         ( (a) > (b) ? (a) : (b) )

/** Return absolute value */
#define ABS(x)              ((x)<0 ? -(x) : (x))

/** Return absolute difference. Works with unsigned values. */
#define ABS_DIFF(x,y)       ((x)>(y) ? (x)-(y) : (y)-(x))

/**
 * Get n rounded up to a multiple of m. Eg ROUND_UP(12,5) = 15, ROUND_UP(4,4) = 4.
 * Only intended for positive integers.
 * \see http://stackoverflow.com/questions/3407012/
 */
#define ROUND_UP(n,m)       ( (n)==0 ? (n) : ( ((n)+(m)-1) - (((n)-1)%(m)) ) )

/**
 * Get n rounded down to a multiple of m. Eg ROUND_DOWN(12,5) = 10, ROUND_DOWN(4,4) = 4, ROUND_DOWN(-1,4) = -4.
 * Works with negative integers for the first argument.
 * \see https://gist.github.com/aslakhellesoy/1134482
 */
#define ROUND_DOWN(n,m)     ( (n) >= 0 ? ((n) / (m)) * (m) : (((n) - (m) + 1) / (m)) * (m) )

/**
 * Perform integer division n/d but round to nearest integer instead of truncating by adding d/2 first.
 * Only intended for positive integers. See \ref DIV_ROUND_INT for signed integers support.
 * \see http://stackoverflow.com/questions/2422712/c-rounding-integer-division-instead-of-truncating
 */
#define DIV_ROUND(n,d)      ((((n) + (d)/2)/(d)))

/**
 * Perform integer division n/d but round to nearest integer instead of truncating.
 * Works with positive or negative integers.
 * \see http://stackoverflow.com/questions/2422712/c-rounding-integer-division-instead-of-truncating
 */
#define DIV_ROUND_INT(n,d)  ((((n) < 0) ^ ((d) < 0)) ? (((n) - (d)/2)/(d)) : (((n) + (d)/2)/(d)))

/**
 * Return -1, 0 or 1 depending on whether x is negative, zero or positive.
 * \see http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
 */
#define SIGN(x)             ((x > 0) - (x < 0))

#define IS_EVEN(x)          ((x) % 2 == 0)
#define IS_ODD(x)           ((x) % 2 == 1)

/** Return true if x is not zero and is an integer power of 2 */
#define IS_POWER_OF_TWO(x)  (x && (!(x & (x - 1))))

/**
 * Get the number of wordSize words required to wholly contain numBytes.
 * Eg. NUM_WORDS_FOR_BYTES(12,5) = 3, NUM_WORDS_FOR_BYTES(4,4) = 1.
 * Only intended for positive integers. Could do with a better name.
 */
#define NUM_WORDS_FOR_BYTES(numBytes, wordSize) \
                            (((numBytes)+(wordSize)-1)/(wordSize))

/** Get the sizeof a member of a struct type. */
#define SIZEOF_MEMBER(type, member) \
                            sizeof( ((type *)0)->member )

/** Turn a define into a string literal by wrapping it in double quotes */
#define STRINGIFY(s)        STRINGIFY_HELPER(s)
#define STRINGIFY_HELPER(s) #s /* two macro levels required, as per https://gcc.gnu.org/onlinedocs/cpp/Stringification.html */

/** Turn a define into a char rvalue by dereferencing a string */
#define CHARIFY(c)          CHARIFY_HELPER(c)
#define CHARIFY_HELPER(c)   *#c /* Ref: https://stackoverflow.com/a/79187995/3697870 */


/**
 * Allow macro names to be overloaded with different numbers of arguments.
 * Ref: https://stackoverflow.com/a/11763277/3697870
 *
 * To overload a macro with two instances, one with one argument and one with two, use:
 *   #MY_MACRO(...) OVERLOAD_MACRO2(__VA_ARGS__, MY_MACRO2, MY_MACRO1)(__VA_ARGS__)
 * then MY_MACRO1() will handle calls to MY_MACRO() with a single argument
 * and  MY_MACRO2() will handle calls to MY_MACRO() with two arguments.
 *
 * To overload a macro with three instances (one, two and three arguments), use OVERLOAD_MACRO3.
 */
#define OVERLOAD_MACRO2(_1, _2, macro, ...)       macro
#define OVERLOAD_MACRO3(_1, _2, _3, macro, ...)   macro


#endif //COMMON_H_
