#include "../include/header.h"

extern Ping data;

byte init_addrinfo(){
    data.addr.hints.ai_family = AF_INET;
    data.addr.hints.ai_socktype = SOCK_RAW;
    data.addr.hints.ai_protocol = IPPROTO_ICMP;
    const int status = getaddrinfo(data.addr.dst, NULL,
                                   &data.addr.hints,
                                   &data.addr.res);
    if(status){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        data.code = status;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

byte init_socket(){
    data.sock = socket(data.res->ai_family,
                       data.res->ai_socktype,
                       data.res->ai_protocol);
    if(data.sock == -1){
        perror("socket");
        data.code = errno;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
