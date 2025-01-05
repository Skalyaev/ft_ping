#include "../include/header.h"

t_ping data = {0};

static int8_t init() {

    data.exiting = NO;
    if (!data.opt.interval) data.opt.interval = 1;
    if (!data.opt.ttl) data.opt.ttl = 64;
    if (!data.opt.size) data.opt.size = 56;

    data.addr.hints.ai_family = AF_INET;
    data.addr.hints.ai_socktype = SOCK_RAW;
    data.addr.hints.ai_protocol = IPPROTO_ICMP;

    const int status = getaddrinfo(data.dst, NULL,
                                   &data.addr.hints,
                                   &data.addr.res);
    if(status) {
        data.code = errno;
        return EXIT_FAILURE;
    }
    data.socket = socket(data.addr.res->ai_family,
                         data.addr.res->ai_socktype,
                         data.addr.res->ai_protocol);
    if(data.socket == -1) {
        data.code = errno;
        return EXIT_FAILURE;
    }
    data.addr_in = *(t_sockaddr_in*)data.addr.res->ai_addr;
    data.send_pid = getpid();

    data.code = EXIT_SUCCESS;
    return EXIT_SUCCESS;
}

static void intro() {

    printf("PING %s (%s): %zu data bytes\n",
           data.dst,
           inet_ntoa(data.addr_in.sin_addr),
           data.opt.size);
}

static void outro() {

    const uint8_t lost = 100 - (data.stats.received * 100 / data.stats.transmitted);

    printf("--- %s ping statistics ---\n", data.dst);
    printf("%ld packets transmitted, %ld packets received, %d%% packet loss\n",
           data.stats.transmitted,
           data.stats.received,
           lost);
}

static int8_t bye() {

    if(data.exiting = YES) return data.code;
    data.exiting = YES;

    if(data.queue) {
        t_transit* ptr = data.queue;
        t_transit* old = ptr;

        while(tmp) {
            ptr = ptr->next;
            free(old);
            old = ptr;
        }
        data.queue = NULL;
    }
    if(data.addr.res) freeaddrinfo(data.addr.res);
    if(data.socket > 0) close(data.socket);
    if(data.buffer) free(data.buffer);
    return data.code;
}

static void sigexit(const int sig) {

    if(data.exiting) return;
    if(sig == SIGINT) outro();
    exit(bye());
}

static int8_t ping(const size_t* const interval) {

    static int seq = 0;
    if(send_icmp(&seq) != EXIT_SUCCESS) return EXIT_FAILURE;
    seq++;

    if(data.opt.preload) data.opt.preload--;
    else sleep(*interval);
    return EXIT_SUCCESS;
}

static void* sender() {

    const size_t interval = data.opt.flood ? 0 : data.opt.interval;

    if(data.opt.count) {
        for(size_t x = 0; x < data.opt.count; x++)
            if(ping(&interval) != EXIT_SUCCESS) break;
    }
    else while(1) if(ping(&interval) != EXIT_SUCCESS) break;
    return NULL;
}

static void* receiver() {

    while(1) {
        if(recv_icmp() != EXIT_SUCCESS) break;
        if(data.stats.transmitted == data.opt.count) break;
        if(data.code != EXIT_SUCCESS) break;
    }
    return NULL;
}

int main(int ac, char** av) {

    signal(SIGINT, sigexit);
    signal(SIGQUIT, sigexit);
    signal(SIGTERM, sigexit);
    getargs(ac, av);

    if (init() != EXIT_SUCCESS) {
        if(data.code == EAI_NONAME) fprintf(stderr, ERR_HOST, av[0]);
        return bye();
    }
    intro();

    pthread_t t_sender, t_receiver;
    pthread_create(&t_sender, NULL, sender, NULL);
    pthread_create(&t_receiver, NULL, receiver, NULL);

    pthread_join(t_sender, NULL);
    pthread_join(t_receiver, NULL);

    if(data.code == EXIT_SUCCESS) outro();
    return bye();
}
