#ifndef DEFINE_H
#define DEFINE_H

#define OPT "t:s:c:i:I:w:W:nDfvqh?"
#define USAGE "\nUsage\n"\
"  %s [options] <destination>\n\n"\
"Options\n"\
"  <destination>   dns name or ip address\n"\
"  -c <count>      stop after <count> replies\n"\
"  -D              print timestamp\n"\
"  -f              flood ping\n"\
"  -h              print help and exit\n"\
"  -I <interface>  either interface name or address\n"\
"  -i <interval>   seconds between sending each packet\n"\
"  -n              no dns name resolution\n"\
"  -q              quiet output\n"\
"  -s <size>       use <size> as number of data bytes to be sent\n"\
"  -t <ttl>        define time to live\n"\
"  -v              verbose output\n"\
"  -w <deadline>   reply wait <deadline> in seconds\n"\
"  -W <timeout>    time to wait for response\n"
#define ERR_USAGE "%s: usage error: Adresse de destination requise\n"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXIT_USAGE 2

#define YES 1
#define NO 0

typedef char byte;
typedef unsigned char bool;
typedef struct addrinfo t_addrinfo;

#endif
