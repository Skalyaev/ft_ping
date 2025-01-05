#ifndef HEADER_H
#define HEADER_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

#include "define.h"
#include "struct.h"

void getargs(const int ac, char** const av);
int8_t send_icmp(const int* const seq);
int8_t recv_icmp();

#endif
