#ifndef STRUCT_H
#define STRUCT_H

typedef struct s_icmp{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;
}Icmp;

typedef struct s_addrinfo{
    t_addrinfo hints;
    t_addrinfo* res;
}Addrinfo;

typedef struct s_options{
    size_t ttl;
    size_t size;
    size_t count;
    size_t interval;
    size_t timeout;
    size_t deadline;
    char* interface;
    bool numeric;
    bool horodatage;
    bool flood;
    bool verbose;
    bool quiet;
}Options;

typedef struct s_ping{
    char* dst;
    Options opt;
    Addrinfo addr;
    int socket;
    Icmp header;
    int code;
}Ping;

#endif
