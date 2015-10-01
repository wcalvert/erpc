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
#include "EthernetInterface.h"
#include "common.h"
#include "erpc.h"
#include "commands.h"
#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "rpcproto.pb.h"
#include "lwip/sockets.h"

#define DEVICE_IP               "10.0.0.20"
#define DEVICE_MASK             "255.255.255.0"
#define DEVICE_GATEWAY          "10.0.0.1"
#define DEVICE_PORT             1234
#define MAX_CONNECTIONS         5


int main (void) {
    EthernetInterface eth;
    eth.init(DEVICE_IP, DEVICE_MASK, DEVICE_GATEWAY);
    eth.connect(1000);
    printf("IP Address is %s\n", eth.getIPAddress());
    
    int listenfd = lwip_socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DEVICE_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    
    if(lwip_bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("Could not bind to socket\n");
    }
    
    if(lwip_listen(listenfd, MAX_CONNECTIONS) != 0) {
        printf("Could not listen to socket\n");
    }

    int connfd;
    while(1) {
        connfd = lwip_accept(listenfd, NULL, NULL);

        if(connfd < 0) {
            printf("accept returned error\n");
        } else {
            #ifdef DEBUG
            printf("Got connection!\n");
            #endif
            handle_connection(connfd);
            #ifdef DEBUG
            printf("Closing connection!\n");
            #endif

            lwip_close(connfd);
        }
    }
}