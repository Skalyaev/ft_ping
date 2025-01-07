#ifndef STRUCT_H
#define STRUCT_H

typedef struct s_opts {
    size_t count;
    size_t ttl;
    size_t preload;
    double interval;
    bool flood;
    bool quiet;
    size_t size;
    bool verbose;
} t_opts;

typedef struct s_addr {
    t_addrinfo hints;
    t_addrinfo* res;
} t_addr;

typedef struct s_stats {
    size_t transmitted;
    size_t received;
    size_t failed;
    double max_transit;
    double min_transit;
    double total_transit;
    double square_sum;
} t_stats;

typedef struct s_transit {
    int seq;
    t_timeval start;
    struct s_transit* next;
} t_transit;

typedef struct s_pending_seq {
    int seq;
    t_timeval end;
    struct s_pending_seq* next;
} t_pending_seq;

typedef struct s_ping {
    pid_t pid;
    char* dst;
    t_opts opt;
    t_addr addr;
    t_sockaddr_in addr_in;
    int socket;
    t_icmphdr hdr;
    int code;
    char* buffer;
    t_stats stats;
    t_transit* queue;
    pthread_mutex_t queue_mutex;
    pthread_t sender;
    pthread_t receiver;
    t_pending_seq* pending_seq;
} t_ping;

#endif
