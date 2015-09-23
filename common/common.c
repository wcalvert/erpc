/* 
    Binding of nanopb streams to tcp sockets and usb.
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <libusb-1.0/libusb.h>
#include <stdio.h>

#include "common.h"

static bool sock_write_callback(pb_ostream_t *stream, const uint8_t *buf, size_t count) {
    int fd = (intptr_t)stream->state;
    return send(fd, buf, count, 0) == count;
}

static bool sock_read_callback(pb_istream_t *stream, uint8_t *buf, size_t count) {
    int fd = (intptr_t)stream->state;
    int result;
    
    result = recv(fd, buf, count, MSG_WAITALL);
    
    if (result == 0)
        stream->bytes_left = 0; /* EOF */
    
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
