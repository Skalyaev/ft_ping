#include "../include/header.h"

extern t_ping data;

static uint16_t checksum_send(const uint16_t* buffer, size_t len) {

    uint32_t sum = 0;

    for(; len > 1; len -= 2) sum += *buffer++;
    if(len == 1) sum += *(uint8_t*)buffer;

    sum = (sum >> 16) + (sum & 0xFFFF);
    return ~((uint16_t)sum);
}

static uint16_t checksum_check(const uint16_t* buffer, size_t len) {

    uint32_t sum = 0;

    for (; len > 1; len -= 2) sum += *buffer++;
    if (len == 1) sum += *(uint8_t*)buffer;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return (uint16_t)sum == 0xFFFF;
}

int8_t send_icmp(const int* const seq) {

    static const size_t addr_size = sizeof(data.addr_in);
    static const size_t hdr_size = sizeof(data.hdr);
    const size_t buffer_size = hdr_size + data.opt.size;

    memset(&data.hdr, 0, hdr_size);

    data.hdr.un.echo.id = htons(data.send_pid);
    data.hdr.un.echo.sequence = htons(*seq);

    data.hdr.type = ICMP_ECHO;
    data.hdr.code = 0;
    data.hdr.checksum = checksum_send((uint16_t*)&data.hdr, hdr_size);

    data.buffer = malloc(buffer_size);
    if(!data.buffer) {
        data.code = errno;
        return EXIT_FAILURE;
    }
    memcpy(data.buffer, &data.hdr, hdr_size);
    memset(data.buffer + hdr_size, 0, data.opt.size);

    const ssize_t sent = sendto(data.socket,
                                data.buffer,
                                buffer_size,
                                0,
                                (t_sockaddr*)&data.addr_in,
                                addr_size);

    char* const tmp = data.buffer;
    data.buffer = NULL;
    free(tmp);

    t_timeval start;
    if (gettimeofday(&start, NULL) < 0) {
        data.code = errno;
        return EXIT_FAILURE
    }
    t_transit** ptr = &data.queue;
    while(*ptr) ptr = &(*ptr)->next;

    *ptr = malloc(sizeof(t_transit));
    if(!*ptr) {
        data.code = errno;
        return EXIT_FAILURE;
    }
    (*ptr)->seq = *seq;
    (*ptr)->start = start;
    (*ptr)->next = NULL;

    if(sent < 0) {
        data.code = errno;
        return EXIT_FAILURE;
    }
    data.stats.transmitted++;
    return EXIT_SUCCESS;
}

int8_t recv_icmp() {

    t_sockaddr_in addr;
    static socklen_t addrlen = sizeof(addr);
    static const uint8_t buffer_size = 64;
    static const size_t hdr_size = sizeof(data.hdr);

    char buffer[buffer_size];
    const ssize_t received = recvfrom(data.socket,
                                      buffer,
                                      buffer_size,
                                      0,
                                      (t_sockaddr*)&addr,
                                      &addrlen);
    if (received < 0) {
        data.code = errno;
        return EXIT_FAILURE;
    }
    t_timeval end;
    if (gettimeofday(&end, NULL) < 0) {
        data.code = errno;
        return EXIT_FAILURE;
    }
    const t_iphdr* const ip_hdr = (t_iphdr*)buffer;
    const t_icmphdr* const icmp_hdr = (t_icmphdr*)(buffer + (ip_hdr->ihl << 2));

    const int seq = ntohs(icmp_hdr->un.echo.sequence);
    const char* const dst = inet_ntoa(addr.sin_addr);

    t_transit* tmp = data.queue;
    t_transit* prev = tmp;

    while(tmp && tmp->seq != seq) {
        prev = tmp;
        tmp = tmp->next;
    }
    if(!tmp) {
        if(!data.opt.quiet)
            printf("%ld bytes from %s: Sequence error\n", received, dst);
        return EXIT_SUCCESS;
    }
    const t_timeval transit = {
        .tv_sec = end.tv_sec - tmp->start.tv_sec,
        .tv_usec = end.tv_usec - tmp->start.tv_usec
    };
    if(data.stats.max_transit.tv_usec < transit.tv_usec) {
        data.stats.max_transit = transit;
    }
    if(data.stats.min_transit.tv_usec > transit.tv_usec) {
        data.stats.min_transit = transit;
    }
    data.stats.total_transit.tv_sec += transit.tv_sec;
    data.stats.total_transit.tv_usec += transit.tv_usec;

    if(prev != tmp) {
        prev->next = tmp->next;
        free(tmp);
    }
    else {
        free(tmp);
        data.queue = NULL;
    }
    if (!checksum_check((uint16_t*)icmp_hdr, hdr_size)) {
        if (!data.opt.quiet)
            printf("%ld bytes from %s: Checksum error\n", received, dst);
    }
    else if (icmp_hdr->type == ICMP_ECHOREPLY) {
        if (!data.opt.quiet)
            printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",
                   received, dst, seq, ip_hdr->ttl, 0);
        data.stats.received++;
    }
    else if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
        if (!data.opt.quiet)
            printf("%ld bytes from %s: Time to live exceeded\n", received, dst);
    }
    return EXIT_SUCCESS;
}
