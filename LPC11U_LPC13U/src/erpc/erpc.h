/**************************************************************************/

/**************************************************************************/
#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "projectconfig.h"
#include "core/usb/usb_hid.h"
#include "core/usb/usb_custom_class.h"

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------+
// PUBLIC API
//--------------------------------------------------------------------+
void erpc_task(void * p_para);
void erpc_init(void);

//--------------------------------------------------------------------+
// Callback API
//--------------------------------------------------------------------+
/*oid prot_cmd_received_cb(protMsgCommand_t const * p_mess) __attribute__ ((weak));
void prot_cmd_executed_cb(protMsgResponse_t const * p_resonse) __attribute__ ((weak));
void prot_cmd_error_cb(protMsgError_t const * p_error) __attribute__ ((weak));*/

#ifdef __cplusplus
}
#endif

#endif
