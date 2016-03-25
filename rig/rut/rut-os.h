/*
 * Rut
 *
 * Rig Utilities
 *
 * Copyright (C) 2014 Intel Corporation.
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

#ifndef _RUT_OS_H_
#define _RUT_OS_H_

#include <stdint.h>
#include <string.h>

#include "rut-exception.h"

typedef enum _rut_io_exception_t {
    RUT_IO_EXCEPTION_BAD_VALUE = 1,
    RUT_IO_EXCEPTION_NO_SPACE,
    RUT_IO_EXCEPTION_IO,
} rut_io_exception_t;

bool rut_os_read(int fd, void *data, int *len, rut_exception_t **e);

/* Doesn't give up until it's read the expected amount of data... */
bool rut_os_read_len(int fd, void *data, int len, rut_exception_t **e);

bool rut_os_write(int fd, void *data, int len, rut_exception_t **e);

#ifdef __linux__
int rut_os_listen_on_abstract_socket(const char *socket_name,
                                     rut_exception_t **e);

int rut_os_connect_to_abstract_socket(const char *socket_name);
#endif /* linux */

int rut_os_listen_on_tcp_socket(int port, rut_exception_t **e);

#endif /* _RUT_OS_H_ */
