/*
 * Copyright (C) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <clib.h>

#include <cglib/cg-defines.h>

/* As much as possible we try to avoid depending on build time checks
 * which don't work well for the various cross compiling use cases we
 * want to support...
 *
 * NB: this header is public
 */
#if defined(C_PLATFORM_UNIX) && !defined(C_PLATFORM_WEB)
#define CG_HAS_POLL_SUPPORT
#endif

#if defined(__GNUC__) || defined(__clang__)
#define CG_HAVE_FFS 1
#define ffs __builtin_ffs
#endif

/* These two builtins are available since GCC 3.4 */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define CG_HAVE_BUILTIN_FFSL
#define CG_HAVE_BUILTIN_POPCOUNTL
#define CG_HAVE_BUILTIN_CLZ
#endif

#if defined(C_PLATFORM_WEB) || defined(C_PLATFORM_DARWIN) || defined(C_PLATFORM_WINDOWS)
#define CG_HAVE_DIRECTLY_LINKED_GL_LIBRARY
#endif

#ifdef CG_HAVE_DIRECTLY_LINKED_GL_LIBRARY
  #ifdef CG_HAS_GL_SUPPORT
    #define CG_GL_LIBNAME ""
  #endif
  #ifdef CG_HAS_GLES2_SUPPORT
    #define CG_GLES2_LIBNAME ""
  #endif
#else
  #ifdef CG_HAS_GL_SUPPORT
    #define CG_GL_LIBNAME "libGL.so.1"
  #endif
  #ifdef CG_HAS_GLES2_SUPPORT
    #define CG_GLES2_LIBNAME "libGLESv2.so"
  #endif
#endif

