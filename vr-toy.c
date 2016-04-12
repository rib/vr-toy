/*
 * Copyright (C) 2015 Intel Corporation.
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

#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <assert.h>

#include <clib.h>
#include <rut.h>

#include <rig-c.h>
#include <rig-c-mesh.h>

static RObject *cam;
static RObject *test;

static RObject *text;
static RObject *text_comp;

static RObject *rects[100];

static RObject *vertex_buf;
static RObject *indices_buf;
static RObject *sphere_mesh;

struct vert {
    float pos[3];
    float s, t;
};

static void
create_mesh(RModule *module)
{
    struct vert vertices[10000];
    uint16_t indices[99*99*6];
    float normal[3] = { 0, 0, 1 };
    float tangent[3] = { 1, 0, 0 };

    sphere_mesh = r_mesh_new(module);

    vertex_buf = r_buffer_new(module, sizeof(vertices));
    indices_buf = r_buffer_new(module, sizeof(indices));

    RObject *attributes[] = {
        r_attribute_new(module,
                        vertex_buf,
                        "cg_position_in",
                        sizeof(struct vert),
                        offsetof(struct vert, pos),
                        3,
                        R_ATTRIBUTE_TYPE_FLOAT),
        r_attribute_new(module,
                        vertex_buf,
                        "cg_tex_coord0_in",
                        sizeof(struct vert),
                        offsetof(struct vert, s),
                        2,
                        R_ATTRIBUTE_TYPE_FLOAT),
#if 0
        r_attribute_new(module,
                        vertex_buf,
                        "cg_normal_in",
                        sizeof(struct vert),
                        offsetof(struct vert, pos),
                        3,
                        R_ATTRIBUTE_TYPE_FLOAT),
#endif
        r_attribute_new_const(module,
                              "cg_normal_in",
                              3, /* n components */
                              1, /* n columns */
                              false, /* no transpose */
                              normal),
        r_attribute_new_const(module,
                              "tangent_in",
                              3, /* n components */
                              1, /* n columns */
                              false, /* no transpose */
                              tangent),
    };

    /* Tesselate a sphere for a rectilinear unproject of the video
     * source.
     *
     * HACK...
     */
    for (int y = 0; y < 100; y++) {
        float v_angle = (M_PI / 100.0) * y;

        for (int x = 0; x < 100; x++) {
            float h_angle = (M_PI * 2.0 / 100.0) * x;
            int i = 100 * y + x;

            vertices[i].pos[0] = sin(h_angle) * fabs(sin(v_angle));
            vertices[i].pos[1] = 1.0 - y * 0.02;
            vertices[i].pos[2] = cos(h_angle) * fabs(sin(v_angle));

            vertices[i].s = x * 0.01;
            vertices[i].t = y * 0.01;
        }
    }
#if 0
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            int i = 100 * y + x;
            vertices[i].pos[0] = -5.0 + x * 0.1;
            vertices[i].pos[1] = -5.0 + y * 0.1;
            vertices[i].pos[2] = 0;
        }
    }
#endif

    int i = 0;
    for (int y = 0; y < 99; y++) {
        for (int x = 0; x < 99; x++) {
            int i0 = 100 * y + x;
            int i1 = 100 * (y + 1) + x;
            int i2 = 100 * (y + 1) + x + 1;
            int i3 = 100 * y + x + 1;

            indices[i++] = i0;
            indices[i++] = i1;
            indices[i++] = i2;
            indices[i++] = i0;
            indices[i++] = i2;
            indices[i++] = i3;
        }
    }
    assert(i == 99 * 99 * 6);

    r_set_enum_by_name(module, sphere_mesh, "vertices_mode", R_VERTICES_MODE_TRIANGLES);
    r_set_integer_by_name(module, sphere_mesh, "n_vertices", 10000);
    r_set_enum_by_name(module, sphere_mesh, "indices_type", R_INDICES_TYPE_UINT16);
    r_set_object_by_name(module, sphere_mesh, "indices", indices_buf);
    r_set_integer_by_name(module, sphere_mesh, "n_indices", 99*99*6);

    r_buffer_set_data(module, vertex_buf, 0, vertices, sizeof(vertices));
    r_buffer_set_data(module, indices_buf, 0, indices, sizeof(indices));

    r_mesh_set_attributes(module, sphere_mesh, attributes, 3);
}

void
vr_toy_load(RModule *module)
{
    RObject *nine_slice  = r_nine_slice_new(module, 0, 0, 0, 0, 8.5, 8.5);
    RObject *material = r_material_new(module);
    RColor light_ambient = { .2, .2, .2, 1 };
    RColor light_diffuse = { .6, .6, .6, 1 };
    RColor light_specular = { .4, .4, .4, 1 };
    RObject *view;
    RObject *controller;
    int x, y;
    RObject *video_source;

    create_mesh(module);

    RObject *e = r_entity_new(module, NULL);
    r_set_text_by_name(module, e, "label", "light");
    r_set_vec3_by_name(module, e, "position", (float [3]){0, 0, 500});

    r_entity_rotate_x_axis(module, e, 20);
    r_entity_rotate_y_axis(module, e, -20);

    RObject *light = r_light_new(module);
    //r_set_color_by_name(module, light, "ambient", (RColor *)&{ .2, .2, .2, 1});
    r_set_color_by_name(module, light, "ambient", &light_ambient);
    r_set_color_by_name(module, light, "diffuse", &light_diffuse);
    r_set_color_by_name(module, light, "specular", &light_specular);
    r_add_component(module, e, light);

    RObject *light_frustum = r_camera_new(module);
    r_set_vec4_by_name(module, light_frustum, "ortho",
                       (float [4]){-1000, -1000, 1000, 1000});
    r_set_float_by_name(module, light_frustum, "near", 1.1f);
    r_set_float_by_name(module, light_frustum, "far", 1500.f);
    r_add_component(module, e, light_frustum);

    e = r_entity_new(module, NULL);
    r_set_vec3_by_name(module, e, "position", (float [3]){0, 0, 0});
    r_set_text_by_name(module, e, "label", "play-camera");

    RObject *play_cam = r_camera_new(module);
    r_set_enum_by_name(module, play_cam, "mode", R_PROJECTION_PERSPECTIVE);
    r_set_float_by_name(module, play_cam, "fov", 90);
    r_set_float_by_name(module, play_cam, "near", 10);
    r_set_float_by_name(module, play_cam, "far", 10000);
    r_set_boolean_by_name(module, play_cam, "clear", false);

    r_add_component(module, e, play_cam);

    view = r_view_new(module);
    r_set_object_by_name(module, view, "camera_entity", e);

    controller = r_controller_new(module, "Controller 0");
    r_controller_bind(module, controller, play_cam, "viewport_width", view, "width");
    r_controller_bind(module, controller, play_cam, "viewport_height", view, "height");
    r_set_boolean_by_name(module, controller, "active", true);

    r_set_color_by_name(module, material, "ambient",
                        r_color_str(module, "#ffffff"));
    r_set_color_by_name(module, material, "diffuse",
                        r_color_str(module, "#ffffff"));
    r_set_color_by_name(module, material, "specular",
                        r_color_str(module, "#ffffff"));

    video_source = r_source_new(module, "file://test.webm");
    //video_source = r_source_new(module, "file://cat.gif");
    //video_source = r_source_new(module, NULL);
    //r_set_boolean_by_name(module, video_source, "running", true);

    r_set_object_by_name(module, material, "color_source", video_source);

    test = r_entity_new(module, NULL);
    //r_add_component(module, test, shape);
    r_add_component(module, test, sphere_mesh);
    //r_add_component(module, test, nine_slice);
    r_add_component(module, test, material);

    /* TODO: support associating a simpler pick mesh with a component
     * otherwise picking can be very expensive */
    //RObject *button = r_button_input_new(module);
    //r_add_component(module, test, button);

    r_entity_rotate_z_axis(module, test, 180.f);

    r_set_vec3_by_name(module, test, "position", (float [3]){0, 0, 0});

    r_set_text_by_name(module, test, "label", "test");

    for (y = 0; y < 1; y++)
        for (x = 0; x < 1; x++) {
            RObject *rect = r_entity_clone(module, test);
            r_set_float_by_name(module, rect, "scale", 100);
            //r_set_vec3_by_name(module, rect, "position", (float [3]){-5 + x, -5 + y, 0});

            rects[10 * y + x] = rect;
        }

    //XXX: maybe add an 'enabled' property on entities
    r_set_boolean_by_name(module, material, "visible", false);

#if 1
    text = r_entity_new(module, NULL);
    text_comp = r_text_new(module);

    r_set_text_by_name(module, text_comp, "text", "Hello World");
    r_add_component(module, text, text_comp);
#endif
    cam = r_find(module, "play-camera");

    c_debug("vr_toy_load callback");
}

void
vr_toy_update(RModule *module, RUpdateState *update)
{
    double delta_seconds = update->progress;

    //static float n = 0;

    //n -= 0.5;
    //r_entity_translate(module, cam, 0, 0, 1);
    //r_set_vec3_by_name(module, test, "position", (float [3]){0, 0, n--});

    //for (int i = 0; i < 100; i++)
    //    r_entity_rotate_z_axis(module, rects[i], delta_seconds * 90.f);
    //r_entity_rotate_z_axis(module, rects[0], delta_seconds * 90.f);

    r_request_animation_frame(module);

    c_debug("vr_toy_update callback (delta = %f)", delta_seconds);
}

void
vr_toy_input(RModule *module, RInputEvent *event)
{
    //c_debug("vr_toy_input callback");
}

int
main(int argc, char **argv)
{
    r_engine_t *engine;

    /* HACK... */
    setenv("RIG_USE_HMD", "dummy", true);

    engine = r_engine_new(&(REngineConfig) { .require_vr_hmd = true });

    r_engine_add_self_as_native_component(engine,
                                          R_ABI_LATEST,
                                          "vr_toy_");

    r_engine_run(engine);
}

