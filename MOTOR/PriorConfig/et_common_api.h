/**
@file et_common_api.h
Common definitions for all projects.
@authors Ed Simons
@copyright Prior Scientific Instruments Ltd., 2012
*/

#ifndef ET_COMMON_API_H
#define ET_COMMON_API_H

#ifdef __cplusplus
extern "C" {
#endif


/* Enumerations */
typedef enum
{
    SUCCESS = 0,
    ERR_UNKNOWN,
    ERR_INITIALISATION,
    ERR_CONFIGURATION,
    ERR_INPUT_PARAMETER,
    ERR_STATE,
    ERR_UNDERFLOW,
    ERR_OVERFLOW,
    ERR_DIVIDE_BY_ZERO,
    ERR_TX,
    ERR_RX,
    ERR_DOES_NOT_EXIST,
    ERR_DATA_CORRUPT,
    ERR_LENGTH_INVALID,
    ERR_BUSY
} ErrorCode;


/** Macro to work out the number of bits in a value (at compile time if the value is a literal).
 * @param value Value to check
 * @returns Number of bits in value (e.g. 64-127 return 7, 128-255 return 8)
 */
#define BITS_IN_VALUE(value)	(			\
	(((value) & 0x80000000uL) != 0) ? 32 :	\
	(((value) & 0x40000000uL) != 0) ? 31 :	\
	(((value) & 0x20000000uL) != 0) ? 30 :	\
	(((value) & 0x10000000uL) != 0) ? 29 :	\
	(((value) & 0x08000000uL) != 0) ? 28 :	\
	(((value) & 0x04000000uL) != 0) ? 27 :	\
	(((value) & 0x02000000uL) != 0) ? 26 :	\
	(((value) & 0x01000000uL) != 0) ? 25 :	\
	(((value) & 0x00800000uL) != 0) ? 24 :	\
	(((value) & 0x00400000uL) != 0) ? 23 :	\
	(((value) & 0x00200000uL) != 0) ? 22 :	\
	(((value) & 0x00100000uL) != 0) ? 21 :	\
	(((value) & 0x00080000uL) != 0) ? 20 :	\
	(((value) & 0x00040000uL) != 0) ? 19 :	\
	(((value) & 0x00020000uL) != 0) ? 18 :	\
	(((value) & 0x00010000uL) != 0) ? 17 :	\
	(((value) & 0x00008000uL) != 0) ? 16 :	\
	(((value) & 0x00004000uL) != 0) ? 15 :	\
	(((value) & 0x00002000uL) != 0) ? 14 :	\
	(((value) & 0x00001000uL) != 0) ? 13 :	\
	(((value) & 0x00000800uL) != 0) ? 12 :	\
	(((value) & 0x00000400uL) != 0) ? 11 :	\
	(((value) & 0x00000200uL) != 0) ? 10 :	\
	(((value) & 0x00000100uL) != 0) ? 9 :	\
	(((value) & 0x00000080uL) != 0) ? 8 :	\
	(((value) & 0x00000040uL) != 0) ? 7 :	\
	(((value) & 0x00000020uL) != 0) ? 6 :	\
	(((value) & 0x00000010uL) != 0) ? 5 :	\
	(((value) & 0x00000008uL) != 0) ? 4 :	\
	(((value) & 0x00000004uL) != 0) ? 3 :	\
	(((value) & 0x00000002uL) != 0) ? 2 :	\
			1 )


/** Internal macro - see ZERO_IF_EMPTY.
 */
#define ZERO_IF_EMPTY_SUFFIX_PASTE(value)	(value ## 0)

/** Macro which returns zero if the value is defined but has no value; or the
 * value itself if it is defined and has a value.
 *
 * Build numbers and other configuration values are often set in environment
 * settings for the build and passed as command-line compiler definitions.
 * During development builds though, these settings will usually not be present
 * in the environment.  The compiler command-line will then typically still
 * set up those definitions but leave them empty.
 *
 * We often need values such as build numbers to revert to some sensible value
 * for developer builds where those numbers are defined but empty.  The problem
 * we have is that defines with no value give compiler errors if you try
 * evaluating the value (even checking them with #if at compile time).  We
 * would like to pick up a value of zero if no value is already set up.
 *
 * The solution is to use token-pasting to suffix zero on the value.  If the
 * value is empty, then token-pasting with zero will give zero; and if the value
 * is not empty then suffixing zero will multiply the value by ten.  Dividing
 * the value by 10 will then give us the desired result of zero for empty values
 * or the actual value.
 *
 * Making this happen requires a bit of advanced macro work though.  If we just
 * called ZERO_IF_EMPTY_SUFFIX_PASTE(BUILD_NUMBER), the result would be
 * "BUILD_NUMBER0" which is not what we want.  Instead, we need to use a first
 * macro to evaluate the value.
 *
 * If BUILD_NUMBER is defined with a value of 123, this macro turns
 * ZERO_IF_EMPTY_SUFFIX_PASTE(BUILD_NUMBER) into
 * ZERO_IF_EMPTY_SUFFIX_PASTE(123) as required, which turns into 1230 after
 * token-pasting.  The subsequent divide by 10 then turns this back into a value
 * of 123.
 *
 * If BUILD_NUMBER is defined but empty, this macro turns
 * ZERO_IF_EMPTY_SUFFIX_PASTE(BUILD_NUMBER) into
 * ZERO_IF_EMPTY_SUFFIX_PASTE(). It is OK (under the C standard) to
 * omit arguments to function-like macros, and the result of token-pasting zero
 * to an empty argument will be zero.
 *
 * Note that it may seem like we could prefix zero on the build number and avoid the divide-by-10,
 * because 0123 is the same as 123, surely?  Actually it isn't - in C, an initial zero on an integer
 * tells the compiler that it's an octal value.
 *
 * CCS does give some preprocessor warnings about empty macro arguments.  Because we want to do this,
 * and because empty macro arguments are valid C, warning 77 must be disabled for projects using the
 * TI CCS compiler.
 *
 * @note This macro must only be passed a defined symbol or a literal value.  It
 * must never be passed a variable.
 */
#define ZERO_IF_EMPTY(value)		(ZERO_IF_EMPTY_SUFFIX_PASTE(value) / 10uL)


/** Internal macro - see STRINGIFY_TEXT
 */
#define STRINGIFY_TEXT_PASTE(arg)	#arg

/** Macro to turn the value passed into a text string.
 *
 * Stringification is a standard C preprocessor operation.  If we just called
 * STRINGIFY_TEXT_PASTE(BUILD_NUMBER) though, the result would just be the
 * string "BUILD_NUMBER" which is not what we want.  Instead we use this macro
 * to force evaluation of the value (e.g. to a value of 123), and
 * STRINGIFY_TEXT_PASTE will then turn that into a string (e.g. "123").
 */
#define STRINGIFY_TEXT(arg)			STRINGIFY_TEXT_PASTE(arg)


#ifdef __cplusplus
}
#endif

#endif /* ET_COMMON_API_H */
