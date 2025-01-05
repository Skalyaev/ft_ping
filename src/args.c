#include "../include/header.h"

extern t_ping data;

void getargs(const int ac, char** const av) {

    const struct option opts[] = {
        {"count", required_argument, 0, 'c'},
        {"interval", required_argument, 0, 'i'},
        {"ttl", required_argument, 0, 0},
        {"timeout", required_argument, 0, 'w'},
        {"preload", required_argument, 0, 'l'},
        {"size", required_argument, 0, 's'},
        {"flood", no_argument, 0, 'f'},
        {"quiet", no_argument, 0, 'q'},
        {"verbose", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    int idx = 0;
    int opt;

    while((opt = getopt_long(ac, av, OPT, opts, &idx)) != -1) {
        switch(opt) {
        case 0:
            data.opt.ttl = atoi(optarg);
            break;
        case 'c':
            data.opt.count = atoi(optarg);
            break;
        case 'i':
            data.opt.interval = atoi(optarg);
            break;
        case 'w':
            data.opt.timeout = atoi(optarg);
            break;
        case 'l':
            data.opt.preload = atoi(optarg);
            break;
        case 's':
            data.opt.size = atoi(optarg);
            break;
        case 'f':
            data.opt.flood = YES;
            break;
        case 'q':
            data.opt.quiet = YES;
            break;
        case 'v':
            data.opt.verbose = YES;
            break;
        case '?':
            printf(USAGE, av[0]);
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, ERR_ARG, av[0], (char *)&opt);
            exit(EXIT_USAGE);
        }
    }
    for(int x = optind; x < ac; x++) data.dst = av[x];
    if(data.dst) return;

    fprintf(stderr, ERR_OPERAND, av[0]);
    exit(EXIT_USAGE);
}
