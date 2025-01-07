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

    data.hdr.un.echo.id = htons(data.pid);
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

    pthread_mutex_lock(&data.queue_mutex);

    t_transit** ptr = &data.queue;
    while(*ptr) ptr = &(*ptr)->next;

    *ptr = malloc(sizeof(t_transit));
    if(!*ptr) {
        pthread_mutex_unlock(&data.queue_mutex);
        data.code = errno;
        return EXIT_FAILURE;
    }
    (*ptr)->seq = *seq;
    (*ptr)->next = NULL;

    const ssize_t sent = sendto(data.socket,
                                data.buffer,
                                buffer_size,
                                0,
                                (t_sockaddr*)&data.addr_in,
                                addr_size);

    char* const tmp = data.buffer;
    data.buffer = NULL;
    free(tmp);

    if(sent < 0) {
        data.code = errno;
        pthread_mutex_unlock(&data.queue_mutex);
        return EXIT_FAILURE;
    }
    data.stats.transmitted++;

    if (gettimeofday(&(*ptr)->start, NULL) < 0) {
        data.code = errno;
        pthread_mutex_unlock(&data.queue_mutex);
        return EXIT_FAILURE;
    }
    pthread_mutex_unlock(&data.queue_mutex);
    return EXIT_SUCCESS;
}

static int8_t new_pending_seq(const int* const seq, const t_timeval* const end) {

    t_pending_seq** ptr = &data.pending_seq;
    while(*ptr) ptr = &(*ptr)->next;

    *ptr = malloc(sizeof(t_pending_seq));
    if(!*ptr) {
        data.code = errno;
        return EXIT_FAILURE;
    }
    (*ptr)->seq = *seq;
    (*ptr)->end = *end;
    (*ptr)->next = NULL;
    return EXIT_SUCCESS;
}

static void check_pending_seq() {

    pthread_mutex_lock(&data.queue_mutex);
    t_pending_seq* tmp = data.pending_seq;

    t_transit* ptr;
    t_transit* prev;
    if(!tmp) {
        pthread_mutex_unlock(&data.queue_mutex);
        return;
    }
    while(tmp && data.queue) {
        ptr = data.queue;
        prev = ptr;

        while(ptr && ptr->seq != tmp->seq) {
            prev = ptr;
            ptr = ptr->next;
        }
        if(!ptr) {
            tmp = tmp->next;
            continue;
        }
        const t_timeval transit = {
            .tv_sec = tmp->end.tv_sec - ptr->start.tv_sec,
            .tv_usec = tmp->end.tv_usec - ptr->start.tv_usec
        };
        if (prev != ptr) {
            prev->next = ptr->next;
            free(ptr);
        } else {
            free(ptr);
            data.queue = NULL;
        }
        pthread_mutex_unlock(&data.queue_mutex);
        const double time = (double)transit.tv_sec * 1000
                            + (double)transit.tv_usec / 1000;

        if (!data.stats.received) {
            data.stats.min_transit = time;
            data.stats.max_transit = time;
        } else {
            if (data.stats.min_transit > time) data.stats.min_transit = time;
            if (data.stats.max_transit < time) data.stats.max_transit = time;
        }
        data.stats.total_transit += time;
        data.stats.square_sum += time * time;
        data.stats.received++;

        if (!data.opt.quiet)
            printf("From %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
                   inet_ntoa(data.addr_in.sin_addr), tmp->seq, data.hdr.un.echo.id, time);

        t_pending_seq* old = tmp;
        tmp = tmp->next;
        free(old);
    }
    pthread_mutex_unlock(&data.queue_mutex);
}

static void dump_packet(const t_icmphdr* const icmp_hdr) {

    const uint8_t* const ptr = (const uint8_t*)icmp_hdr + 8;

    const t_ip* ip_hdr = (const t_ip*)ptr;
    size_t ip_len = ip_hdr->ip_hl << 2;

    printf("IP Hdr Dump:\n");
    for (size_t x = 0; x < ip_len; x += 2) {

        if (x + 1 < ip_len) printf(" %02x%02x", ptr[x], ptr[x + 1]);
        else printf(" %02x", ptr[x]);
    }
    printf("\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");

    char src_buf[INET_ADDRSTRLEN];
    char dst_buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_hdr->ip_src, src_buf, sizeof(src_buf));
    inet_ntop(AF_INET, &ip_hdr->ip_dst, dst_buf, sizeof(dst_buf));

    printf(" %u  %u  %02x %04x %04x  %2u %04x  %02x  %02x %04x %s  %s\n",
           ip_hdr->ip_v,
           ip_hdr->ip_hl,
           ip_hdr->ip_tos,
           ntohs(ip_hdr->ip_len),
           ntohs(ip_hdr->ip_id),
           (ntohs(ip_hdr->ip_off) & 0xe000) >> 13,
           (ntohs(ip_hdr->ip_off) & 0x1fff),
           ip_hdr->ip_ttl,
           ip_hdr->ip_p,
           ntohs(ip_hdr->ip_sum),
           src_buf,
           dst_buf);

    const t_icmphdr* original_icmp = (const t_icmphdr*)((const uint8_t*)ip_hdr + ip_len);
    const size_t ntohs_len = ntohs(ip_hdr->ip_len);
    const size_t icmp_len = (ntohs_len > ip_len) ? (ntohs_len - ip_len) : 0;

    printf("ICMP: type %d, code %d, size %zu, id 0x%04x, seq 0x%04x\n",
           original_icmp->type,
           original_icmp->code,
           icmp_len,
           ntohs(original_icmp->un.echo.id),
           ntohs(original_icmp->un.echo.sequence));
}

int8_t recv_icmp() {

    check_pending_seq();

    t_sockaddr_in addr;
    static socklen_t addrlen = sizeof(addr);
    static const int buffer_size = 1024;

    char buffer[buffer_size];
    ssize_t received = recvfrom(data.socket,
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
    const t_ip* ip_hdr = (const t_ip*)buffer;
    const t_icmphdr* icmp_hdr = (const t_icmphdr*)(buffer + (ip_hdr->ip_hl << 2));

    int seq = -1;

    if (icmp_hdr->type == ICMP_ECHOREPLY){
        seq = ntohs(icmp_hdr->un.echo.sequence);
    	received -= ip_hdr->ip_hl << 2;
    }
    else if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
        const t_ip* oip = (const t_ip*)((const uint8_t*)icmp_hdr + 8);

        const t_icmphdr* oicmp = (const t_icmphdr*)((const uint8_t*)oip + (oip->ip_hl << 2));
        seq = ntohs(oicmp->un.echo.sequence);
    	received -= oip->ip_hl << 2;
    }
    pthread_mutex_lock(&data.queue_mutex);
    t_transit* tmp = data.queue;
    t_transit* prev = NULL;

    while (tmp && tmp->seq != seq) {
        prev = tmp;
        tmp = tmp->next;
    }
    if (!tmp) {
        pthread_mutex_unlock(&data.queue_mutex);
        if(new_pending_seq(&seq, &end) != EXIT_SUCCESS) return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }
    const t_timeval transit = {
        .tv_sec = end.tv_sec - tmp->start.tv_sec,
        .tv_usec = end.tv_usec - tmp->start.tv_usec
    };
    if(!prev) {
        if(!tmp->next) data.queue = NULL;
        else data.queue = tmp->next;
        free(tmp);
    } else {
        prev->next = tmp->next;
        free(tmp);
    }
    pthread_mutex_unlock(&data.queue_mutex);
    const char* dst = inet_ntoa(addr.sin_addr);

    if (!checksum_check((uint16_t*)icmp_hdr, sizeof(data.hdr))) {
        data.stats.failed++;
        if (!data.opt.quiet)
            printf("%ld bytes from %s: Checksum error\n", received, dst);
    }
    else if (icmp_hdr->type == ICMP_ECHOREPLY) {
        double time = (double)transit.tv_sec * 1000 + (double)transit.tv_usec / 1000;

        if(!data.stats.received) {
            data.stats.min_transit = time;
            data.stats.max_transit = time;
        } else {
            if(data.stats.min_transit > time) data.stats.min_transit = time;
            if(data.stats.max_transit < time) data.stats.max_transit = time;
        }
        data.stats.total_transit += time;
        data.stats.square_sum += time * time;
        data.stats.received++;

        if (!data.opt.quiet)
            printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
                   received, dst, seq, ip_hdr->ip_ttl, time);
    }
    else if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
        data.stats.failed++;
        if (!data.opt.quiet)
            printf("%ld bytes from %s: Time to live exceeded\n", received, dst);

        if(data.opt.verbose) dump_packet(icmp_hdr);
    }
    return EXIT_SUCCESS;
}
