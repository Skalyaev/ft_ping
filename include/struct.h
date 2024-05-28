#ifndef STRUCT_H
#define STRUCT_H

typedef struct s_options{
    size_t ttl;
    size_t size;
    size_t count;
    size_t interval;
    size_t timeout;
    size_t deadline;
    char *interface;
    bool numeric;
    bool horodatage;
    bool flood;
    bool verbose;
    bool quiet;
}Options;

#endif
