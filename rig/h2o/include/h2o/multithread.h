/*
 * Copyright (c) 2015 DeNA Co., Ltd., Kazuho Oku, Tatsuhiko Kubo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef h2o__multithread_h
#define h2o__multithread_h

#ifdef H2O_USE_LIBUV
#include <uv.h>
#else
#include <pthread.h>
#endif
#include "h2o/linklist.h"
#include "h2o/socket.h"

typedef struct st_h2o_multithread_receiver_t h2o_multithread_receiver_t;
typedef struct st_h2o_multithread_queue_t h2o_multithread_queue_t;
typedef struct st_h2o_multithread_request_t h2o_multithread_request_t;

typedef void (*h2o_multithread_receiver_cb)(h2o_multithread_receiver_t *receiver, h2o_linklist_t *messages);
typedef void (*h2o_multithread_response_cb)(h2o_multithread_request_t *req);

struct st_h2o_multithread_receiver_t {
    h2o_multithread_queue_t *queue;
    h2o_linklist_t _link;
    h2o_linklist_t _messages;
    h2o_multithread_receiver_cb cb;
};

typedef struct st_h2o_multithread_message_t {
    h2o_linklist_t link;
} h2o_multithread_message_t;

struct st_h2o_multithread_request_t {
    h2o_multithread_message_t super;
    h2o_multithread_receiver_t *source;
    h2o_multithread_response_cb cb;
};

/**
 * creates a queue that is used for inter-thread communication
 */
h2o_multithread_queue_t *h2o_multithread_create_queue(h2o_loop_t *loop);
/**
 * destroys the queue
 */
void h2o_multithread_destroy_queue(h2o_multithread_queue_t *queue);
/**
 * registers a receiver for specific type of message
 */
void h2o_multithread_register_receiver(h2o_multithread_queue_t *queue, h2o_multithread_receiver_t *receiver,
                                       h2o_multithread_receiver_cb cb);
/**
 * unregisters a receiver
 */
void h2o_multithread_unregister_receiver(h2o_multithread_queue_t *queue, h2o_multithread_receiver_t *receiver);
/**
 * sends a message
 */
void h2o_multithread_send_message(h2o_multithread_receiver_t *receiver, h2o_multithread_message_t *message);
/**
 * sends a request
 */
void h2o_multithread_send_request(h2o_multithread_receiver_t *receiver, h2o_multithread_request_t *req);

#ifdef H2O_USE_LIBUV
#define H2O_ONCE_INIT UV_ONCE_INIT
typedef uv_once_t h2o_once_t;
#define h2o_once uv_once
typedef uv_thread_t h2o_thread_t;
#define h2o_thread_join(tid) uv_thread_join(tid)
typedef uv_mutex_t h2o_mutex_t;
#define h2o_mutex_init(mutex) uv_mutex_init(mutex)
#define h2o_mutex_destroy(mutex) uv_mutex_destroy(mutex)
#define h2o_mutex_lock(mutex) uv_mutex_lock(mutex)
#define h2o_mutex_trylock(mutex) uv_mutex_trylock(mutex)
#define h2o_mutex_unlock(mutex) uv_mutex_unlock(mutex)
typedef uv_cond_t h2o_cond_t;
#define h2o_cond_init(cond) uv_cond_init(cond)
#define h2o_cond_destroy(cond) uv_cond_destroy(cond)
#define h2o_cond_wait(cond, mutex) uv_cond_wait(cond, mutex)
#define h2o_cond_signal(cond) uv_cond_signal(cond)
#define h2o_cond_broadcast(cond) uv_cond_breadcast(cond)
#else
#define H2O_ONCE_INIT PTHREAD_ONCE_INIT
typedef pthread_once_t h2o_once_t;
#define h2o_once pthread_once
typedef pthread_t h2o_thread_t;
#define h2o_thread_join(tid) pthread_join(tid, NULL)
typedef pthread_mutex_t h2o_mutex_t;
#define h2o_mutex_init(mutex) pthread_mutex_init(mutex, NULL)
#define h2o_mutex_destroy(mutex) pthread_mutex_destroy(mutex)
#define h2o_mutex_lock(mutex) pthread_mutex_lock(mutex)
#define h2o_mutex_trylock(mutex) pthread_mutex_trylock(mutex)
#define h2o_mutex_unlock(mutex) pthread_mutex_unlock(mutex)
typedef pthread_cond_t h2o_cond_t;
#define h2o_cond_init(cond) pthread_cond_init(cond, NULL)
#define h2o_cond_destroy(cond) pthread_cond_destroy(cond)
#define h2o_cond_wait(cond, mutex) pthread_cond_wait(cond, mutex)
#define h2o_cond_signal(cond) pthread_cond_signal(cond)
#define h2o_cond_broadcast(cond) pthread_cond_breadcast(cond)
#endif

/**
 * create a thread
 */
void h2o_multithread_create_thread(h2o_thread_t *tid, void (*func)(void *), void *arg);

#endif
