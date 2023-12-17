#ifndef LUZER_MACROS_H_
#define LUZER_MACROS_H_

#include <assert.h>
#include <stdbool.h>

#ifdef DEBUG
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( false )
#else
#define DEBUG_PRINT(...) do{ } while ( false )
#endif /* DEBUG */

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

/**
 * If control flow reaches the point of the unreachable(), the program is
 * undefined. It is useful in situations where the compiler cannot deduce
 * the unreachability of the code.
 */
#if __has_builtin(__builtin_unreachable) || defined(__GNUC__)
#  define unreachable() (assert(0), __builtin_unreachable())
#else
#  define unreachable() (assert(0))
#endif

#define NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))

#ifdef __has_attribute
#if __has_attribute(no_sanitize)
#define NO_SANITIZE_MEMORY __attribute__((no_sanitize("memory")))
#else
#define NO_SANITIZE_MEMORY
#endif  // __has_attribute(no_sanitize)
#else
#define NO_SANITIZE_MEMORY
#endif  // __has_attribute

// https://clang.llvm.org/docs/SanitizerCoverage.html#disabling-instrumentation-with-attribute-no-sanitize-coverage
#ifdef __has_feature

#if __has_feature(coverage_sanitizer)
#define NO_SANITIZE_COVERAGE __attribute__((no_sanitize("coverage")))
#else // __has_feature(coverage_sanitizer)
#warning "compiler does not provide 'coverage_sanitizer' feature"
#define NO_SANITIZE_COVERAGE
#endif // __has_feature(coverage_sanitizer)

#else // __has_feature
#warning "compiler does not provide __has_feature, can't check for sanitization"
#endif // __has_feature

#define NO_SANITIZE NO_SANITIZE_ADDRESS NO_SANITIZE_MEMORY NO_SANITIZE_COVERAGE

#endif  // LUZER_MACROS_H_
