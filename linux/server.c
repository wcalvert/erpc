/* RPC Thingy in progress.
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include <pb_encode.h>
#include <pb_decode.h>

#include "rpcproto.pb.h"
#include "common.h"

uint32_t rpc_args[MAX_ARGS] = {0};
uint32_t num_args = 0;

uint32_t add(void) {
    uint32_t total = 0;
    for(int i=0; i<num_args; i++) {
        printf("adding: %d\n", rpc_args[i]);
        total += rpc_args[i];
    }
    return total;
}

uint32_t mult(void) {
    uint32_t total = rpc_args[0];
    for(int i=1; i<num_args; i++) {
        total *= rpc_args[i];
    }
    return total;
}

RPCMethod command_table[] = {
    { add, "add", 2, 2, "Add two numbers", "int,int" },
    { mult, "mult", 2, 2, "Multiply two numbers", "int,int" }
};

#define NUM_COMMANDS (sizeof(command_table)/sizeof(RPCMethod))

bool listmethods_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {

    for(int i=0; i<NUM_COMMANDS; i++) {
        MethodInfo methodInfo = {};
        
        strncpy(methodInfo.name, command_table[i].name, sizeof(command_table[i].name));
        methodInfo.name[sizeof(methodInfo.name)-1] = '\0';

        methodInfo.min_args = command_table[i].min_args;
        methodInfo.max_args = command_table[i].max_args;

        strncpy(methodInfo.description, command_table[i].description, sizeof(command_table[i].description));
        methodInfo.description[sizeof(methodInfo.description)-1] = '\0';

        if(!pb_encode_tag_for_field(stream, field)) {
            return false;
        }

        if(!pb_encode_submessage(stream, MethodInfo_fields, &methodInfo)) {
            return false;
        }
    }

    return true;
}

bool decodeargs_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    uint64_t _arg;
    if (!pb_decode_varint(stream, &_arg)) {
        return false;
    }
    if(num_args < MAX_ARGS) {
        rpc_args[num_args] = (uint32_t)_arg;
        printf("Got arg: %d\n", rpc_args[num_args]);
        num_args++;
    } else {
        printf("Too many args received, ignoring!\n");
    }

    return true;
}

void handle_connection(int connfd) {
    
    RPCMessage request = {};
    pb_istream_t input = pb_istream_from_socket(connfd);
    request.args.funcs.decode = &decodeargs_callback;

    if (!pb_decode(&input, RPCMessage_fields, &request)) {
        printf("Decode failed: %s\n", PB_GET_ERROR(&input));
        return;
    }

    RPCMessage response = {};
    pb_ostream_t output = pb_ostream_from_socket(connfd);
    
    if(request.type ==  RPCMessage_message_type_LIST_METHODS_REQUEST) {
        response.type = RPCMessage_message_type_LIST_METHODS_RESPONSE;
        response.method.funcs.encode = &listmethods_callback;
        response.method.arg = NULL;

        if (!pb_encode(&output, RPCMessage_fields, &response)) {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&output));
        }
    } else if(request.type == RPCMessage_message_type_REQUEST) {
        response.type = RPCMessage_message_type_RESPONSE;
        bool valid_method = false;
        for(int i=0; i<NUM_COMMANDS; i++) {
            if(strcmp(request.name, command_table[i].name) == 0) {
                response.has_result = true;
                response.result = command_table[i].func();
                valid_method = true;
            }
        }

        if(!valid_method) {
            printf("invalid method: %s\n", request.name);
            /* create error response here? */
        }
        
        if (!pb_encode(&output, RPCMessage_fields, &response)) {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&output));
        }
        num_args = 0;
    }
}

int main(int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    int reuse = 1;
    
    /* Listen on localhost:1234 for TCP connections */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1234);
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("bind");
        return 1;
    }
    
    if (listen(listenfd, 5) != 0) {
        perror("listen");
        return 1;
    }
    
    printf("Ready...\n");
    for(;;) {
        /* Wait for a client */
        connfd = accept(listenfd, NULL, NULL);
        
        if (connfd < 0) {
            perror("accept");
            return 1;
        }
        
        printf("Got connection.\n");
        
        handle_connection(connfd);
        
        printf("Closing connection.\n");
        
        close(connfd);
    }
    
    return 0;
}
