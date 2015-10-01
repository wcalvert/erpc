/*
 * Copyright (c) 2015, Wes Calvert
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "mbed.h"
#include "common.h"
#include "erpc.h"
#include "commands.h"
#include "lwip/sockets.h"
#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "rpcproto.pb.h"

uint32_t rpc_args[MAX_ARGS] = { 0 };
uint32_t num_args = 0;
extern RPCMethod command_table[];

static bool sock_write_callback(pb_ostream_t *stream, const uint8_t *buf, size_t count) {
    int fd = (intptr_t)stream->state;
    return lwip_send(fd, buf, count, 0) == count;
}

static bool sock_read_callback(pb_istream_t *stream, uint8_t *buf, size_t count) {
    int fd = (intptr_t)stream->state;
    int result;
    
    result = lwip_recv(fd, buf, count, MSG_WAITALL);
    
    if (result == 0)
        stream->bytes_left = 0;
    
    return result == count;
}

pb_ostream_t pb_ostream_from_socket(int fd) {
    pb_ostream_t stream = {&sock_write_callback, (void*)(intptr_t)fd, SIZE_MAX, 0};
    return stream;
}

pb_istream_t pb_istream_from_socket(int fd) {
    pb_istream_t stream = {&sock_read_callback, (void*)(intptr_t)fd, SIZE_MAX};
    return stream;
}

bool listmethods_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {

    for(uint32_t i=0; i<NUM_COMMANDS; i++) {
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
        #ifdef DEBUG
        printf("Got arg: %d\n", rpc_args[num_args]);
        #endif
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
        for(uint32_t i=0; i<NUM_COMMANDS; i++) {
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
