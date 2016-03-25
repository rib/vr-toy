/*
 * Rig
 *
 * UI Engine & Editor
 *
 * Copyright (C) 2012  Intel Corporation
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
 */

#ifndef __RIG_DOF_EFFECT_H__
#define __RIG_DOF_EFFECT_H__

#include <cglib/cglib.h>

#include <rut.h>

typedef struct _rig_depth_of_field_t rig_depth_of_field_t;

#include "rig-engine.h"

rig_depth_of_field_t *rig_dof_effect_new(rig_engine_t *engine);

void rig_dof_effect_free(rig_depth_of_field_t *dof);

void rig_dof_effect_set_framebuffer_size(rig_depth_of_field_t *dof,
                                         int width,
                                         int height);

cg_framebuffer_t *rig_dof_effect_get_depth_pass_fb(rig_depth_of_field_t *dof);

cg_framebuffer_t *rig_dof_effect_get_color_pass_fb(rig_depth_of_field_t *dof);

void rig_dof_effect_draw_rectangle(rig_depth_of_field_t *dof,
                                   cg_framebuffer_t *fb,
                                   float x1,
                                   float y1,
                                   float x2,
                                   float y2);

#endif /* __RIG_DOF_EFFECT_H__ */
