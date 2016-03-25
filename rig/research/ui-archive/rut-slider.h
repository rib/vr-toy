/*
 * Rut
 *
 * Rig Utilities
 *
 * Copyright (C) 2011 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef __RUT_SLIDER_H__
#define __RUT_SLIDER_H__

#include "rut-shell.h"

typedef struct _rut_slider_t rut_slider_t;

extern rut_type_t rut_slider_type;

rut_slider_t *rut_slider_new(rut_shell_t *shell, rut_axis_t axis, float min,
			     float max, float length);

void rut_slider_set_range(rut_slider_t *slider, float min, float max);

void rut_slider_set_length(rut_slider_t *slider, float length);

void rut_slider_set_progress(rut_object_t *slider, float progress);

#endif /* __RUT_SLIDER_H__ */
