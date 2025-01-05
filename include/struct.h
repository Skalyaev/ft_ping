#ifndef STRUCT_H
#define STRUCT_H

typedef struct s_opts {
    size_t count;
    size_t interval;
    size_t ttl;
    size_t timeout;
    bool flood;
    size_t preload;
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
    t_timeval max_transit;
    t_timeval min_transit;
    t_timeval total_transit;
} t_stats;

typedef struct s_transit {
    int seq;
    t_timeval start;
    struct s_transit* next;
} t_transit;

typedef struct s_ping {
    char* dst;
    t_opts opt;
    t_addr addr;
    t_sockaddr_in addr_in;
    int socket;
    t_icmphdr hdr;
    int code;
    char* buffer;
    bool exiting;
    pid_t send_pid;
    t_stats stats;
    t_transit* queue;
} t_ping;

#endif
