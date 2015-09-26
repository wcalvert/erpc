#ifndef _ERPC_H_
#define _ERPC_H_

#include <pb.h>

pb_ostream_t pb_ostream_from_socket(int fd);
pb_istream_t pb_istream_from_socket(int fd);

bool listmethods_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);
bool decodeargs_callback(pb_istream_t *stream, const pb_field_t *field, void **arg);
void handle_connection(int connfd);

#endif
