#include "../include/header.h"

extern t_ping data;

static bool isnumeric(const char* str) {

    size_t idx = 0;
    while(str[idx] == ' '
            || str[idx] == '\t'
            || str[idx] == '\n'
            || str[idx] == '\r'
            || str[idx] == '\v'
            || str[idx] == '\f') idx++;

    if(str[idx] == '-' || str[idx] == '+') idx++;
    if(!str[idx]) return NO;

    while(str[idx]) {
        if(str[idx] < '0' || str[idx] > '9') return NO;
        idx++;
    }
    return YES;
}

void getargs(const int ac, char** const av) {

    const struct option opts[] = {
        {"count", required_argument, 0, 'c'},
        {"ttl", required_argument, 0, 0},
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
            if(!isnumeric(optarg)) {
                fprintf(stderr,
                        "%s: invalid value (`%s' near `%s')\n",
                        av[0], optarg, optarg);

                exit(EXIT_FAILURE);
            }
            data.opt.ttl = atoi(optarg);
            break;
        case 'c':
            if(!isnumeric(optarg)) {
                fprintf(stderr,
                        "%s: invalid value (`%s' near `%s')\n",
                        av[0], optarg, optarg);

                exit(EXIT_FAILURE);
            }
            data.opt.count = atoi(optarg);
            break;
        case 'l':
            if(!isnumeric(optarg)) {
                fprintf(stderr,
                        "%s: invalid value (`%s' near `%s')\n",
                        av[0], optarg, optarg);

                exit(EXIT_FAILURE);
            }
            data.opt.preload = atoi(optarg);
            break;
        case 's':
            if(!isnumeric(optarg)) {
                fprintf(stderr,
                        "%s: invalid value (`%s' near `%s')\n",
                        av[0], optarg, optarg);

                exit(EXIT_FAILURE);
            }
            data.opt.size_changed = YES;
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
            if(optopt) {
                printf("Try '%s --help' for more information.\n", av[0]);
                exit(EXIT_USAGE);
            }
            printf(USAGE, av[0]);
            exit(EXIT_SUCCESS);
        }
    }
    for(int x = optind; x < ac; x++) data.dst = av[x];
    if(data.dst) return;

    fprintf(stderr, ERR_OPERAND, av[0], av[0]);
    exit(EXIT_USAGE);
}
