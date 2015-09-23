#include "mbed.h"
#include "EthernetInterface.h"

#include <pb_encode.h>
#include <pb_decode.h>
#include "rpcproto.pb.h"
#include "common.h"

#define MBED_DEV_IP       "10.0.0.20"
#define MBED_DEV_MASK     "255.255.255.0"
#define MBED_DEV_GW       "10.0.0.1"
#define ECHO_SERVER_PORT   1234
#define BUF_SIZE 256

uint32_t rpc_args[MAX_ARGS] = {0};
uint32_t num_args = 0;
char buffer_in[BUF_SIZE];
char buffer_out[BUF_SIZE];
AnalogOut aout(DAC0_OUT);
PwmOut pwm1(PTC10);


uint32_t add(void) {
    uint32_t total = 0;
    for(uint32_t i=0; i<num_args; i++) {
        total += rpc_args[i];
    }
    return total;
}

uint32_t mult(void) {
    uint32_t total = rpc_args[0];
    for(uint32_t i=1; i<num_args; i++) {
        total *= rpc_args[i];
    }
    return total;
}

uint32_t dac(void) {
    if(rpc_args[0] > 65535) {
        return 0;
    }
    aout.write_u16((uint16_t)rpc_args[0]);
    return 1;
}

uint32_t pwm(void) {
    if(rpc_args[1] > rpc_args[0]) {
        return 0;
    }
    pwm1.period_us(rpc_args[0]);
    pwm1.pulsewidth_us(rpc_args[1]);
    return 1;
}

RPCMethod command_table[] = {
    { add, "add", 2, 2, "Add two numbers", "int,int" },
    { mult, "mult", 2, 2, "Multiply two numbers", "int,int" },
    { dac, "dac", 1, 1, "Set DAC0 output", "int16"},
    { pwm, "pwm", 2, 2, "Set PTC10 period and pulsewidth (uS)", "int,int"}
};

#define NUM_COMMANDS (sizeof(command_table)/sizeof(RPCMethod))

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
        printf("Got arg: %d\n", rpc_args[num_args]);
        num_args++;
    } else {
        printf("Too many args received, ignoring!\n");
    }

    return true;
}

void handle_connection() {
    
    RPCMessage request = {};
    pb_istream_t input = pb_istream_from_buffer((uint8_t*)buffer_in, BUF_SIZE);
    request.args.funcs.decode = &decodeargs_callback;

    if (!pb_decode(&input, RPCMessage_fields, &request)) {
        printf("Decode failed: %s\n", PB_GET_ERROR(&input));
        return;
    }

    RPCMessage response = {};
    pb_ostream_t output = pb_ostream_from_buffer((uint8_t*)buffer_out, BUF_SIZE);
    
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
 
int main (void) {
    EthernetInterface eth;
    eth.init(MBED_DEV_IP, MBED_DEV_MASK, MBED_DEV_GW); //Assign a device ip, mask and gateway
    eth.connect();
    printf("IP Address is %s\n", eth.getIPAddress());
    
    TCPSocketServer server;
    server.bind(ECHO_SERVER_PORT);
    server.listen();
    
    while (true) {
        printf("Waiting for connection...\n");
        TCPSocketConnection client;
        server.accept(client);
        client.set_blocking(false, 500); // Timeout after (1.5)s
        
        printf("Connection from: %s\n", client.get_address());
        
        int n = client.receive_all(buffer_in, BUF_SIZE);

        printf("After receive_all\n");

        handle_connection();

        client.send_all(buffer_out, BUF_SIZE);
        
        client.close();
    }
}