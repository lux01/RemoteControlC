#ifndef H_GETOPT
#define H_GETOPT

#define P_PASS 0x01
#define P_USER 0x02
#define P_PORT 0x04
#define P_NOAU 0x08

struct options {
    char * username;
    char * password;
    char * ports;
    int    disableAuth;
};

void usage();
int parse_opts(int argc, char ** argv, struct options * opts);
void set_default_opts(struct options * opts);

#endif
