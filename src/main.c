#include "../include/header.h"

t_ping data = {0};

static int8_t init() {

    data.addr.hints.ai_family = AF_INET;
    data.addr.hints.ai_socktype = SOCK_RAW;
    data.addr.hints.ai_protocol = IPPROTO_ICMP;

    const int status = getaddrinfo(data.dst, NULL,
                                   &data.addr.hints,
                                   &data.addr.res);
    if(status) {
        perror("getaddrinfo");
        data.code = status;
        return EXIT_FAILURE;
    }
    data.socket = socket(data.addr.res->ai_family,
                         data.addr.res->ai_socktype,
                         data.addr.res->ai_protocol);
    if(data.socket == -1) {
        perror("socket");
        data.code = errno;
        return EXIT_FAILURE;
    }
    if(setsockopt(data.socket, IPPROTO_IP, IP_TTL,
                  &data.opt.ttl, sizeof(data.opt.ttl)) == -1) {
        perror("setsockopt");
        data.code = errno;
        return EXIT_FAILURE;
    }
    data.addr_in = *(t_sockaddr_in*)data.addr.res->ai_addr;

    pthread_mutex_init(&data.queue_mutex, NULL);
    data.pid = getpid();
    data.code = EXIT_SUCCESS;
    return EXIT_SUCCESS;
}

static void intro() {

    printf("PING %s (%s): %zu data bytes",
           data.dst,
           inet_ntoa(data.addr_in.sin_addr),
           data.opt.size);

    if(data.opt.verbose) printf(" id=0x%x = %d", data.pid, data.pid);
    printf("\n");
}

static void outro() {

    uint8_t lost;
    if(!data.stats.transmitted) lost = 0;
    else lost = 100 - (data.stats.received * 100 / data.stats.transmitted);

    printf("--- %s ping statistics ---\n", data.dst);
    printf("%ld packets transmitted, %ld packets received, %d%% packet loss\n",
           data.stats.transmitted,
           data.stats.received,
           lost);
    if(!data.stats.received) return;

    const double avg = data.stats.total_transit / data.stats.received;
    const double variance = (data.stats.square_sum / data.stats.received) - (avg * avg);

    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
           data.stats.min_transit,
           avg,
           data.stats.max_transit,
           sqrt(variance));
}

static int8_t bye() {

    pthread_mutex_lock(&data.queue_mutex);
    if(data.queue) {
        t_transit* ptr = data.queue;
        t_transit* old = ptr;

        while(ptr) {
            ptr = ptr->next;
            free(old);
            old = ptr;
        }
        data.queue = NULL;
    }
    if(data.pending_seq) {
        t_pending_seq* ptr = data.pending_seq;
        t_pending_seq* old = ptr;

        while(ptr) {
            ptr = ptr->next;
            free(old);
            old = ptr;
        }
        data.pending_seq = NULL;
    }
    if(data.addr.res) freeaddrinfo(data.addr.res);
    if(data.socket > 0) close(data.socket);
    if(data.buffer) free(data.buffer);

    pthread_mutex_destroy(&data.queue_mutex);
    return data.code;
}

static void sigexit(const int sig) {

    static bool exiting = NO;
    if(exiting == YES) return;
    exiting = YES;

    pthread_mutex_lock(&data.queue_mutex);
    if(data.sender) {
        pthread_cancel(data.sender);
        pthread_join(data.sender, NULL);
    }
    if(data.receiver) {
        pthread_cancel(data.receiver);
        pthread_join(data.receiver, NULL);
    }
    pthread_mutex_unlock(&data.queue_mutex);

    if(sig == SIGINT) outro();
    exit(bye());
}

static int8_t ping(const size_t* const interval,  const size_t* const idx) {

    static int seq = -1;
    seq++;

    if(send_icmp(&seq) != EXIT_SUCCESS) return EXIT_FAILURE;
    if(idx && *idx == data.opt.count - 1) return EXIT_SUCCESS;

    if(data.opt.preload) data.opt.preload--;
    else usleep(*interval);
    return EXIT_SUCCESS;
}

static void* sender() {

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    const size_t interval = data.opt.flood ? 10000 : data.opt.interval * 1000000;

    if(data.opt.count) {
        for(size_t x = 0; x < data.opt.count; x++)
            if(ping(&interval, &x) != EXIT_SUCCESS) break;
    }
    else while(1) if(ping(&interval,NULL) != EXIT_SUCCESS) break;
    return NULL;
}

static void* receiver() {

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    size_t total = 0;
    while(1) {
        if(recv_icmp() != EXIT_SUCCESS) break;

        total = data.stats.received + data.stats.failed;
        if(total == data.opt.count) break;
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

    pthread_create(&data.sender, NULL, sender, NULL);
    pthread_create(&data.receiver, NULL, receiver, NULL);

    pthread_join(data.sender, NULL);
    pthread_join(data.receiver, NULL);

    if(data.code == EXIT_SUCCESS) outro();
    return bye();
}
