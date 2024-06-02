#include "../include/header.h"

Ping data = {0};

static byte bye(){

    if(data.res) freeaddrinfo(data.res);
    return data.code;
}

static void sighdl(const int sig){

    outro();
    exit(bye());
}

static void getargs(const int ac, char** const av){

    int opt = getopt(ac, av, OPT);
    while(opt != -1){
        switch(opt){
        case 't':
            data.opt.ttl = atoi(optarg);
            break;
        case 's':
            data.opt.size = atoi(optarg);
            break;
        case 'c':
            data.opt.count = atoi(optarg);
            break;
        case 'i':
            data.opt.interval = atoi(optarg);
            break;
        case 'w':
            data.opt.deadline = atoi(optarg);
            break;
        case 'W':
            data.opt.timeout = atoi(optarg);
            break;
        case 'n':
            data.opt.numeric = YES;
            break;
        case 'D':
            data.opt.horodatage = YES;
            break;
        case 'f':
            data.opt.flood = YES;
            break;
        case 'v':
            data.opt.verbose = YES;
            break;
        case 'q':
            data.opt.quiet = YES;
            break;
        case '?':
        case 'h':
            printf(USAGE, av[0]);
            exit(EXIT_USAGE);
        }
        opt = getopt(ac, av, OPT);
    }
    for(int x = optind; x < ac; x++) data.dst = av[x];
    if(data.dst) return;

    fprintf(stderr, ERR_USAGE, av[0]);
    exit(EXIT_FAILURE);
}

int main(int ac, char** av){

    setlocale(LC_ALL, "");
    signal(SIGINT, sighdl);
    getargs(ac, av, &data);

    data.code = EXIT_SUCCESS;
    if(init_addrinfo() != EXIT_SUCCESS) return bye();
    if(init_socket() != EXIT_SUCCESS) return bye();

    return bye();
}
