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

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libusb-1.0/libusb.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "rpcproto.pb.h"
#include "common.h"
#include "client.h"

#define VID                 0x1FC9
#define PID                 0x2029
#define INTERFACE_NUM       3
#define ENDPOINT_OUT        0x04
#define ENDPOINT_IN         0x81

MultiTransportClient client;

/*
    Bulk transport to device.
*/
bool usb_transfer_out() {
    if(client.transport == USB) {
        int transferred;
        libusb_bulk_transfer(client.device, ENDPOINT_OUT, client.buffer_out, USB_BUF_SIZE, &transferred, 0);
        return transferred == USB_BUF_SIZE;
    }
    return true;
}

/*
    Bulk transport from device.
*/
bool usb_transfer_in() {
    if(client.transport == USB) {
        int transferred;
        libusb_bulk_transfer(client.device, ENDPOINT_IN, client.buffer_in, USB_BUF_SIZE, &transferred, 0);
        return transferred == USB_BUF_SIZE;
    }
    return true;
}

/* 
    Callback to serialize command arguments.
*/
bool encodeargs_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
    for(int i=0; i<client.num_args; i++) {
        if(!pb_encode_tag_for_field(stream, field)) {
            return false;
        }

        if(!pb_encode_varint(stream, (uint64_t)client.rpc_args[i])) {
            return false;
        }
    }

    return true;
}

/*
    Callback to list each method.
 */
bool listmethods_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    MethodInfo methodInfo = {};
    if (!pb_decode(stream, MethodInfo_fields, &methodInfo)) {
        return false;
    }
    printf("Name: %s, min args: %d, max args: %d, description: %s\n", 
        methodInfo.name, methodInfo.min_args, methodInfo.max_args, methodInfo.description);
    
    return true;
}

/*
    List all methods supported by server.
 */
bool listmethods() {
    RPCMessage request = {};
    request.type = RPCMessage_message_type_LIST_METHODS_REQUEST;

    if(!pb_encode(&client.output, RPCMessage_fields, &request)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&client.output));
        return false;
    }
    
    /* Signal the end of request with a 0 tag. */
    uint8_t zero = 0;
    pb_write(&client.output, &zero, 1);

    /* Do USB bulk transfers if needed. */
    usb_transfer_out();
    usb_transfer_in();
    
    /* Give a pointer to our callback function, which will handle the methods as they arrive. */
    RPCMessage response = {};
    response.method.funcs.decode = &listmethods_callback;
    
    if(!pb_decode(&client.input, RPCMessage_fields, &response)) {
        fprintf(stderr, "Decode failed: %s\n", PB_GET_ERROR(&client.input));
        return false;
    }

    return true;
}

/*
    Call a method.
*/
bool callmethod(int optind, int argc, char **argv) {
    RPCMessage request = {};
    request.type = RPCMessage_message_type_REQUEST;
    request.has_name = true;
    strncpy(request.name, argv[optind], strlen(argv[optind]));
    client.num_args = 0;
    for(int i=optind+1; i<argc; i++) {
        int index = i-2;
        if(index<MAX_ARGS) {
            client.rpc_args[client.num_args] = atoi(argv[i]);
            client.num_args++;
        } else {
            printf("Too many arguments supplied! Skipping arg: %s\n", argv[i]);
        }
    }
    request.args.funcs.encode = &encodeargs_callback;
    
    if(!pb_encode(&client.output, RPCMessage_fields, &request)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&client.output));
        return false;
    }

    /* We signal the end of request with a 0 tag. */
    uint8_t zero = 0;
    pb_write(&client.output, &zero, 1);

    /* Do USB bulk transfers if needed. */
    usb_transfer_out();
    usb_transfer_in();
    
    RPCMessage response = {};   
    if(!pb_decode(&client.input, RPCMessage_fields, &response)) {
        fprintf(stderr, "Decode failed: %s\n", PB_GET_ERROR(&client.input));
        return false;
    }

    printf("%d\n", response.result);

    return true;
}

/*
    Main program entry point.
*/
int main(int argc, char **argv) {
    int port = 1234;
    char *host = "127.0.0.1";
    char *transport = NULL;
    int c;

    opterr = 0;
    while((c = getopt(argc, argv, "p:h:t:")) != -1)
    switch(c) {
        case 'p':
            port = atoi(optarg);
            if(port == 0) {
                fprintf(stderr, "Invalid port specified\n");
                return -1;
            }
            break;
        case 'h':
            if(strcmp(optarg, "localhost") != 0) {
                host = optarg;
            }
            break;
        case 't':
            transport = optarg;
            break;
        case '?':
            if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return -1;
        default:
            return -1;
    }

    if(transport != NULL) {
        int sockfd;
        
        if(strcmp(transport, "usb") == 0) {
            /* Initialise libusbx */
            if(libusb_init(NULL)) {
                fprintf(stderr, "Failed to initialise libusbx\n");
                return -1;
            }

            /* Try to connect. */
            client.device = libusb_open_device_with_vid_pid(NULL, VID, PID);
            if (NULL == client.device) {
                fprintf(stderr, "Unable to open VID:0x%04X PID:0x%04X\n", VID, PID);
                libusb_exit(NULL);
                return -1;
            }

            /* Claim the interface. */
            if(libusb_claim_interface(client.device, INTERFACE_NUM)) {
                fprintf(stderr, "Unable to claim interface %d for VID:0x%04X PID:0x%04X\n", INTERFACE_NUM, VID, PID);
                libusb_exit(NULL);
                return -1;
            }

            /* Setup input and output streams. */
            client.output = pb_ostream_from_buffer(client.buffer_out, USB_BUF_SIZE);
            client.input = pb_istream_from_buffer(client.buffer_in, USB_BUF_SIZE);
            client.transport = USB;

        } else if(strcmp(transport, "tcp") == 0) {
            
            struct sockaddr_in servaddr;
            
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            
            /* Connect to server. */
            memset(&servaddr, 0, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = inet_addr(host);
            servaddr.sin_port = htons(port);
            
            if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
                fprintf(stderr, "Could not connect to device at %s:%d\n", host, port);
                return -1;
            }

            /* Setup input and output streams. */
            client.output = pb_ostream_from_socket(sockfd);
            client.input = pb_istream_from_socket(sockfd);
            client.transport = TCP;

        } else {
            fprintf(stderr, "Invalid transport specified\n");
            return -1;
        }

        /* Either list all RPC methods, or make an RPC call. */
        if(argc == optind) {
            if(!listmethods()) {
                return -1;
            }
        } else {
            if(!callmethod(optind, argc, argv)) {
                return -1;
            }
        }

        /* Close socket if needed. */
        if(client.transport == TCP) {
            close(sockfd);
        }

        /* Close libusb if needed. */
        if(client.transport == USB) {
            libusb_release_interface(client.device, INTERFACE_NUM);
            libusb_exit(NULL);
        }
    } else {
        fprintf(stderr, "Transport must be specified with '-t usb' or '-t tcp'.\n");
        return -1;
    }
    
    return 0;
}
