# The function sets the given variable in a parent scope to a
# value in CLANG_RT_LIB_DIR environment variable if it
# is set. Otherwise the value with path to a directory with
# Clang RT libraries is composed manually. Function raises
# a fatal message if C compiler is not Clang.

function(SetClangRTLibDir outvar)
  set(ClangRTLibDir $ENV{CLANG_RT_LIB_DIR})
  if (NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "C compiler is not a Clang")
  endif ()
  if (NOT ClangRTLibDir)
    string(REPLACE "." ";" VERSION_LIST ${CMAKE_C_COMPILER_VERSION})
    list(GET VERSION_LIST 0 CLANG_VERSION_MAJOR)
    # Clang <= 15: /usr/lib/llvm-xxx/lib/clang/X.Y.Z/lib/linux/
    # Clang >  15: /usr/lib/llvm-xxx/lib/clang/X/lib/linux/
    set(CLANG_VERSION ${CMAKE_C_COMPILER_VERSION})
    if (CLANG_VERSION_MAJOR GREATER 15)
      set(CLANG_VERSION ${CLANG_VERSION_MAJOR})
    endif ()
    set(ClangRTLibDir "/usr/lib/llvm-${CLANG_VERSION_MAJOR}/lib/clang/${CLANG_VERSION}/lib/linux/")
  endif ()
  set(${outvar} ${ClangRTLibDir} PARENT_SCOPE)
  message(STATUS "[SetClangRTLibDir] ${outvar} is ${ClangRTLibDir}")
endfunction()
