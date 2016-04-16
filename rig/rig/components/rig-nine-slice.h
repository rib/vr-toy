/*
 * Rig
 *
 * UI Engine & Editor
 *
 * Copyright (C) 2013  Intel Corporation.
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

#pragma once

#include <cglib/cglib.h>

#include <rut.h>

#include <rig-engine.h>

typedef struct _rig_nine_slice_t rig_nine_slice_t;
extern rut_type_t rig_nine_slice_type;

rig_nine_slice_t *rig_nine_slice_new(rig_engine_t *engine,
                                     float top,
                                     float right,
                                     float bottom,
                                     float left,
                                     float width,
                                     float height);

void rig_nine_slice_set_size(rut_object_t *self, float width, float height);

void rig_nine_slice_get_size(rut_object_t *self, float *width, float *height);

void rig_nine_slice_set_image_size(rut_object_t *self, int width, int height);

cg_primitive_t *rig_nine_slice_get_primitive(rut_object_t *object);

rut_mesh_t *rig_nine_slice_get_pick_mesh(rut_object_t *object);

typedef void (*rig_nine_slice_update_callback_t)(rig_nine_slice_t *nine_slice,
                                                 void *user_data);

void
rig_nine_slice_add_update_callback(rig_nine_slice_t *nine_slice,
                                   rut_closure_t *closure);

void rig_nine_slice_set_width(rut_object_t *nine_slice, float width);

void rig_nine_slice_set_height(rut_object_t *nine_slice, float height);

void rig_nine_slice_set_left(rut_object_t *nine_slice, float left);

void rig_nine_slice_set_right(rut_object_t *nine_slice, float right);

void rig_nine_slice_set_top(rut_object_t *nine_slice, float top);

void rig_nine_slice_set_bottom(rut_object_t *nine_slice, float bottom);
