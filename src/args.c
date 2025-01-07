#include "../include/header.h"

extern t_ping data;

static ssize_t is_valid(const char* str, const bool isfloat) {

    ssize_t idx = 0;
    while(str[idx] == ' '
            || str[idx] == '\t'
            || str[idx] == '\n'
            || str[idx] == '\r'
            || str[idx] == '\v'
            || str[idx] == '\f') idx++;

    if(str[idx] == '-' || str[idx] == '+') idx++;
    if(!str[idx]) return idx;

    bool dot = NO;
    while(str[idx]) {
        if(str[idx] < '0' || str[idx] > '9') {

            if(!isfloat) return idx;
            if(dot) return idx;

            if(str[idx] == '.') dot = YES;
            else return idx;
        }
        idx++;
    }
    return -1;
}

static bool is_question_mark(char** const av, const int idx) {

    if(strcmp(av[idx], "-?") == 0) return YES;
    if(strcmp(av[idx], "--help") == 0) return YES;
    return NO;
}

void getargs(const int ac, char** const av) {

    data.opt.ttl = 64;
    data.opt.size = 56;
    data.opt.interval = 1;

    const struct option opts[] = {
        {"count", required_argument, 0, 'c'},
        {"interval", required_argument, 0, 0},
        {"ttl", required_argument, 0, 0},
        {"preload", required_argument, 0, 'l'},
        {"size", required_argument, 0, 's'},
        {"flood", no_argument, 0, 'f'},
        {"quiet", no_argument, 0, 'q'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, '?'},
        {0, 0, 0, 0}
    };
    int useless_idx = 0;
    int idx = 0;
    int opt;
    ssize_t invalid_at;

    while((opt = getopt_long(ac, av, OPT, opts, &useless_idx)) != -1) {
        switch(opt) {
        case 0:
            if((invalid_at = is_valid(optarg, NO)) >= 0) {
                fprintf(stderr,
                        "%s: invalid value (`%s' near `%s')\n",
                        av[0], optarg, optarg + invalid_at);

                exit(EXIT_FAILURE);
            }
            if((data.opt.ttl = atoi(optarg)) > 255) {
                fprintf(stderr, "%s: option value too big: %ld\n", av[0], data.opt.ttl);
                exit(EXIT_FAILURE);
            }
            else if(data.opt.ttl == 0) {
                fprintf(stderr, "%s: option value too small: %ld\n", av[0], data.opt.ttl);
                exit(EXIT_FAILURE);
            }
            break;
        case 'c':
            if((invalid_at = is_valid(optarg, NO)) >= 0) {
                fprintf(stderr,
                        "%s: invalid value (`%s' near `%s')\n",
                        av[0], optarg, optarg + invalid_at);

                exit(EXIT_FAILURE);
            }
            data.opt.count = atoi(optarg);
            break;
        case 'i':
            if((invalid_at = is_valid(optarg, YES)) >= 0) {
                fprintf(stderr,
                        "%s: invalid value (`%s' near `%s')\n",
                        av[0], optarg, optarg + invalid_at);

                exit(EXIT_FAILURE);
            }
            data.opt.interval = atof(optarg);
            if(data.opt.interval < 0.002 && getuid()) {
                fprintf(stderr,
                        "%s: cannot flood; minimal interval allowed for user is 2ms\n",
                        av[0]);

                exit(EXIT_FAILURE);
            }
            break;
        case 'l':
            if((invalid_at = is_valid(optarg, NO)) >= 0) {
                fprintf(stderr,
                        "%s: invalid value (`%s' near `%s')\n",
                        av[0], optarg, optarg + invalid_at);

                exit(EXIT_FAILURE);
            }
            if((data.opt.preload = atoi(optarg)) > 3 && getuid()) {
                fprintf(stderr,
                        "%s: cannot set preload to value greater than 3: %ld\n",
                        av[0], data.opt.preload);

                exit(EXIT_FAILURE);
            }
            break;
        case 's':
            if((invalid_at = is_valid(optarg, NO)) >= 0) {
                fprintf(stderr,
                        "%s: invalid value (`%s' near `%s')\n",
                        av[0], optarg, optarg + invalid_at);

                exit(EXIT_FAILURE);
            }
            data.opt.size = atoi(optarg);
            break;
        case 'f':
            if(getuid()) {
                fprintf(stderr,
                        "%s: cannot flood; minimal interval allowed for user is 2ms\n",
                        av[0]);

                exit(EXIT_FAILURE);
            }
            data.opt.flood = YES;
            data.opt.quiet = YES;
            break;
        case 'q':
            data.opt.quiet = YES;
            break;
        case 'v':
            data.opt.verbose = YES;
            break;
        case '?':
            if(is_question_mark(av, idx + 1) == YES) {
                printf(USAGE, av[0]);
                exit(EXIT_SUCCESS);
            }
            printf("Try '%s --help' for more information.\n", av[0]);
            exit(EXIT_USAGE);
        }
        idx++;
    }
    for(int x = optind; x < ac; x++) data.dst = av[x];
    if(data.dst) return;

    fprintf(stderr, ERR_OPERAND, av[0], av[0]);
    exit(EXIT_USAGE);
}
