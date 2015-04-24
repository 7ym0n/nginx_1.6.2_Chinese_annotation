
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_EVENT_CONNECT_H_INCLUDED_
#define _NGX_EVENT_CONNECT_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


#define NGX_PEER_KEEPALIVE           1
#define NGX_PEER_NEXT                2
#define NGX_PEER_FAILED              4


typedef struct ngx_peer_connection_s  ngx_peer_connection_t;

//当使用长连接与上游服务器通信时，可通过该方法由连接池中获取一个新连接
typedef ngx_int_t (*ngx_event_get_peer_pt)(ngx_peer_connection_t *pc,
    void *data);
//当使用长连接与上游服务器通信时，通过该方法将使用完毕的连接释放给连接池
typedef void (*ngx_event_free_peer_pt)(ngx_peer_connection_t *pc, void *data,
    ngx_uint_t state);
#if (NGX_SSL)

typedef ngx_int_t (*ngx_event_set_peer_session_pt)(ngx_peer_connection_t *pc,
    void *data);
typedef void (*ngx_event_save_peer_session_pt)(ngx_peer_connection_t *pc,
    void *data);
#endif


struct ngx_peer_connection_s {
	//一个主动连接实际上也需要ngx_connection_t结构体中的大部分成员，并且出于重用的考虑而定义了connection成员
    ngx_connection_t                *connection;
	//无端服务器的socket地址
    struct sockaddr                 *sockaddr;
	//sockaddr地址的长度
    socklen_t                        socklen;
	//远端服务器的名称
    ngx_str_t                       *name;
	//表示连接一个远端服务器时，当前连接出现异常失败后可以重试的次数，也就是允许的最多失败次数
    ngx_uint_t                       tries;
	//获取连接的方法，如果使用长连接构成的连接池，那么必须实现get方法
    ngx_event_get_peer_pt            get;
	//与get方法对应的释放连接的方法
    ngx_event_free_peer_pt           free;
	//
    void                            *data;

#if (NGX_SSL)
    ngx_event_set_peer_session_pt    set_session;
    ngx_event_save_peer_session_pt   save_session;
#endif

#if (NGX_THREADS)
    ngx_atomic_t                    *lock;
#endif
	//本地地址信息
    ngx_addr_t                      *local;
	//套接字的接收缓冲区大小
    int                              rcvbuf;
	//记录日志的ngx_log_t对象
    ngx_log_t                       *log;
	//为1时表示此连接已经有了缓存
    unsigned                         cached:1;

                                     /* ngx_connection_log_error_e */
    unsigned                         log_error:2;
};


ngx_int_t ngx_event_connect_peer(ngx_peer_connection_t *pc);
ngx_int_t ngx_event_get_peer(ngx_peer_connection_t *pc, void *data);


#endif /* _NGX_EVENT_CONNECT_H_INCLUDED_ */