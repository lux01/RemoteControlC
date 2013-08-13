#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getopt.h"

void usage() {
    printf(
        "Usage:\n"
        "  rrc [<option> <value> [..]]\n"
        "\n"
        "Options:\n"
        "  -h, -?       : Show this message.\n"
        "  -p <PASSWORD>: Sets the password for the server. Use -p! for no password.\n"
        "                 If not set, you will be prompted for a password during startup.\n"
        "  -u <USERNAME>: Sets the username for the server. (Default: \"user\")\n"
        "  -n <PORTNUMS>: Sets the Mongoose portnum string for the server. (Default: \"8080\")\n"
        "  -p!          : Do not set a password, this disables authentication.\n"
        "\n"
        "Note that if you specify -p, -u, or -n, the next string will be taken verbatim\n"
        "as the parameter, even if it is another option.\n"
        );

    exit(EXIT_SUCCESS);
}

int parse_opts(int argc, char ** argv, struct options * opts) {
    int i, ret = 0; // Loop counter

    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-p") == 0) { // The next string is the password
            if(i == argc - 1) {
                fprintf(stderr, "Error: -p set but no password supplied! Use -p! to disable authentication.\n");
                exit(EXIT_FAILURE);
            }
            opts->password = argv[++i];
            ret |= P_PASS;
        } else if(strcmp(argv[i], "-u") == 0) { // The next string is the username
            if(i == argc - 1) {
                fprintf(stderr, "Error: -u set but no username supplied!\n");
                exit(EXIT_FAILURE);
            }
            opts->username = argv[++i];
            ret |= P_USER;
        } else if(strcmp(argv[i], "-n") == 0) {
            if(i == argc - 1) {
                fprintf(stderr, "Error: -n set but no ports supplied!\n");
                exit(EXIT_FAILURE);
            }
            opts->ports = argv[++i];
            ret |= P_PORT;
        } else if(strcmp(argv[i], "-p!") == 0) {
            opts->disableAuth = 1;
            ret |= P_NOAU;
        } else if(strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "-h") == 0) {
            usage();
        } else {
            fprintf(stderr, "Unrecognised option: %s\n\n", argv[i]);
            usage();
        }

    }

    return ret;
}

void set_default_opts(struct options * opts) {
    opts->username = "user";
    opts->password = "password";
    opts->ports    = "8080";
    opts->disableAuth = 0;
}
