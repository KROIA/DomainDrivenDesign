#pragma once

/// USER_SECTION_START 1

/// USER_SECTION_END

#include <chrono>
// Params
// <LIBRARY NAME SHORT>=DDD 
//
#ifndef BUILD_STATIC
	//#pragma message("DDD_LIB is a shared library")
	#if defined(DDD_LIB)
		#define DDD_EXPORT __declspec(dllexport)
	#else
		#define DDD_EXPORT __declspec(dllimport)
	#endif
#else 
	//#pragma message("DDD_LIB is a static library")
	#define DDD_EXPORT
#endif

/// USER_SECTION_START 2

/// USER_SECTION_END

#ifdef QT_ENABLED
	//#pragma message("QT is enabled")
	#ifdef QT_WIDGETS_ENABLED
		//#pragma message("QT_WIDGETS is enabled")
	#endif
#endif

/// USER_SECTION_START 3

/// USER_SECTION_END

// MSVC Compiler
#ifdef _MSC_VER 
	#define __PRETTY_FUNCTION__ __FUNCSIG__
	typedef std::chrono::steady_clock::time_point TimePoint;
#else
	typedef std::chrono::system_clock::time_point TimePoint;
#endif


#define DDD_UNUSED(x) (void)x;

/// USER_SECTION_START 4

/// USER_SECTION_END

#if defined(DDD_LIB)
	#pragma warning (error : 4715) // not all control paths return a value shuld be an error instead of a warning
	#pragma warning (error : 4700) // uninitialized local variable used shuld be an error instead of a warning
	#pragma warning (error : 4244) // Implicit conversions between data types 
	#pragma warning (error : 4100) // Unused variables
	#pragma warning (error : 4018) // Type mismatch 
	#pragma warning (error : 4996) // Unsafe function calls
	#pragma warning (error : 4456) // declaration of 'x' hides previous local declaration
	#pragma warning (error : 4065) // switch statement contains 'default' but no 'case' labels
	#pragma warning (error : 4189) // Unused return value
	#pragma warning (error : 4996) // unsafe function calls
	#pragma warning (error : 4018) // signed/unsigned mismatch
	#pragma warning (error : 4172) // Returning address of local temporary object
#endif

/// USER_SECTION_START 5

/// USER_SECTION_END