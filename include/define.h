#ifndef DEFINE_H
#define DEFINE_H

#define OPT "c:i:w:l:s:fqv?"

#define USAGE "Usage: %s [OPTION...] HOST ...\n"\
"Send ICMP ECHO_REQUEST packets to network hosts.\n"\
"\n"\
"  -c, --count=NUMBER         stop after sending NUMBER packets\n"\
"  -i, --interval=NUMBER      wait NUMBER seconds between sending each packet\n"\
"      --ttl=N                specify N as time-to-live\n"\
"  -w, --timeout=N            stop after N seconds\n"\
"  -f, --flood                flood ping (root only)\n"\
"  -l, --preload=NUMBER       send NUMBER packets as fast as possible before\n"\
"                             falling into normal mode of behavior (root only)\n"\
"  -q, --quiet                quiet output\n"\
"  -s, --size=NUMBER          send NUMBER data octets\n"\
"  -v, --verbose              verbose output\n"\
"  -?, --help                 give this help list\n"\
"\n"\
"Mandatory or optional arguments to long options"\
"are also mandatory or optional for any corresponding short options.\n"

#define ERR_OPERAND "%s: missing host operand\n"\
"Try 'ping --help' for more information.\n"

#define ERR_ARG "%s: invalid option -- '%s'\n"\
"Try 'ping --help' for more information.\n"

#define ERR_HOST "%s: unknown host\n"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXIT_USAGE 64

#define YES 1
#define NO 0

typedef unsigned char bool;
typedef struct addrinfo t_addrinfo;
typedef struct sockaddr t_sockaddr;
typedef struct sockaddr_in t_sockaddr_in;
typedef struct icmphdr t_icmphdr;
typedef struct iphdr t_iphdr;
typedef struct timeval t_timeval;

#endif
