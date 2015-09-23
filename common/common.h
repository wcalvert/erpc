#ifndef _PB_EXAMPLE_COMMON_H_
#define _PB_EXAMPLE_COMMON_H_

#include <pb.h>

pb_ostream_t pb_ostream_from_socket(int fd);
pb_istream_t pb_istream_from_socket(int fd);

typedef struct RPCMethod { 
    uint32_t (*func)();
    char name[32];
    uint32_t min_args;
    uint32_t max_args;
    char description[128];
    char arg_info[128];
} RPCMethod;

#define MAX_ARGS 10

#endif
