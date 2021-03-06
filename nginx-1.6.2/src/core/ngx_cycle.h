
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CYCLE_H_INCLUDED_
#define _NGX_CYCLE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef NGX_CYCLE_POOL_SIZE
#define NGX_CYCLE_POOL_SIZE     NGX_DEFAULT_POOL_SIZE
#endif


#define NGX_DEBUG_POINTS_STOP   1
#define NGX_DEBUG_POINTS_ABORT  2


typedef struct ngx_shm_zone_s  ngx_shm_zone_t;

typedef ngx_int_t (*ngx_shm_zone_init_pt) (ngx_shm_zone_t *zone, void *data);

struct ngx_shm_zone_s {
    void                     *data;
    ngx_shm_t                 shm;
    ngx_shm_zone_init_pt      init;
    void                     *tag;
};


struct ngx_cycle_s {
    void                  ****conf_ctx; /*配置上下文数组(含所有模块，所有模块的配置结构体)，首先指向一个成员皆为指针的数组，其中每个成员指针又指向另外一个成员皆为指针的数组，第二2个数组中的成员指针才会指向各模块生成的配置结构体*/
    ngx_pool_t               *pool;	/*内存池*/

    ngx_log_t                *log; /*日志*/
    ngx_log_t                 new_log; /**/

    ngx_uint_t                log_use_stderr;  /* unsigned  log_use_stderr:1; */

    ngx_connection_t        **files; /*连接文件*/
    ngx_connection_t         *free_connections; /*空闲连接*/
    ngx_uint_t                free_connection_n; /*空间连接数*/

    ngx_queue_t               reusable_connections_queue; /*再利用连接队列*/

    ngx_array_t               listening; /*监听数组  ngx_listening_t*/
    ngx_array_t               paths; /*路径数组  ngx_path_t */
    ngx_list_t                open_files; /*打开文件链表 ngx_open_file_t*/
    ngx_list_t                shared_memory; /*共享内存链表 ngx_shm_zone_t*/

    ngx_uint_t                connection_n; /*连接个数*/
    ngx_uint_t                files_n; /*打开文件个数*/
	//创建的所有连接
    ngx_connection_t         *connections; 
	//有多少个连接就有多少个读事件
    ngx_event_t              *read_events; 
	//有多少个连接就有多少个写事件
    ngx_event_t              *write_events; 

    ngx_cycle_t              *old_cycle; /*old cycle指针*/

    ngx_str_t                 conf_file; /*配置文件*/
    ngx_str_t                 conf_param; /*配置参数*/
    ngx_str_t                 conf_prefix; /*配置前缀*/
    ngx_str_t                 prefix; /*前缀*/
    ngx_str_t                 lock_file; /*锁文件*/
    ngx_str_t                 hostname; /*主机名*/
};

//nginx核心配置
typedef struct {
     ngx_flag_t               daemon;
     ngx_flag_t               master;

     ngx_msec_t               timer_resolution;
	 //工作进程数
     ngx_int_t                worker_processes;
     ngx_int_t                debug_points;

     ngx_int_t                rlimit_nofile;
     ngx_int_t                rlimit_sigpending;
     off_t                    rlimit_core;

     int                      priority;

     ngx_uint_t               cpu_affinity_n;
     uint64_t                *cpu_affinity;

     char                    *username;
     ngx_uid_t                user;
     ngx_gid_t                group;

     ngx_str_t                working_directory;
     ngx_str_t                lock_file;

     ngx_str_t                pid;
     ngx_str_t                oldpid;

     ngx_array_t              env;
     char                   **environment;

#if (NGX_THREADS)
     ngx_int_t                worker_threads;
     size_t                   thread_stack_size;
#endif

} ngx_core_conf_t;


typedef struct {
     ngx_pool_t              *pool;   /* pcre's malloc() pool */
} ngx_core_tls_t;


#define ngx_is_init_cycle(cycle)  (cycle->conf_ctx == NULL)


ngx_cycle_t *ngx_init_cycle(ngx_cycle_t *old_cycle);
ngx_int_t ngx_create_pidfile(ngx_str_t *name, ngx_log_t *log);
void ngx_delete_pidfile(ngx_cycle_t *cycle);
ngx_int_t ngx_signal_process(ngx_cycle_t *cycle, char *sig);
void ngx_reopen_files(ngx_cycle_t *cycle, ngx_uid_t user);
char **ngx_set_environment(ngx_cycle_t *cycle, ngx_uint_t *last);
ngx_pid_t ngx_exec_new_binary(ngx_cycle_t *cycle, char *const *argv);
uint64_t ngx_get_cpu_affinity(ngx_uint_t n);
ngx_shm_zone_t *ngx_shared_memory_add(ngx_conf_t *cf, ngx_str_t *name,
    size_t size, void *tag);


extern volatile ngx_cycle_t  *ngx_cycle;
extern ngx_array_t            ngx_old_cycles;
extern ngx_module_t           ngx_core_module;
extern ngx_uint_t             ngx_test_config;
extern ngx_uint_t             ngx_quiet_mode;
#if (NGX_THREADS)
extern ngx_tls_key_t          ngx_core_tls_key;
#endif


#endif /* _NGX_CYCLE_H_INCLUDED_ */
