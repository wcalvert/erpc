#ifndef _CLIENT_H_
#define _CLIENT_H_

#define USB_BUF_SIZE 64

typedef enum Transport {
    USB,
    TCP
} Transport;

typedef struct MultiTransportClient {
    uint32_t rpc_args[MAX_ARGS];
    uint32_t num_args;
    pb_ostream_t output;
    pb_istream_t input;
    unsigned char buffer_in[USB_BUF_SIZE];
    unsigned char buffer_out[USB_BUF_SIZE];
    Transport transport;
    libusb_device_handle *device;
} MultiTransportClient;

#endif
