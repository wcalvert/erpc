/**************************************************************************
*/
#include <stdio.h>
#include "projectconfig.h"

#ifdef CFG_ERPC

#include <pb_encode.h>
#include <pb_decode.h>

#include "erpc.h"
#include "rpcproto.pb.h"
#include "common.h"

/* Callback functions to let us know when new data arrives via USB, etc. */
#if defined(CFG_ERPC_VIA_HID)
  #define command_received_isr  usb_hid_generic_recv_isr
  #define command_send          usb_hid_generic_send
#elif defined(CFG_ERPC_VIA_BULK)
  #define command_received_isr  usb_custom_received_isr
  #define command_send          usb_custom_send
#endif




/**************************************************************************/

/**************************************************************************/
void erpc_init(void)
{
  /*fifo_clear(&ff_prot_cmd);*/
}

/**************************************************************************/

/**************************************************************************/
void erpc_task(void * p_para)
{
  /*if ( !fifo_isEmpty(&ff_prot_cmd) )
  {

  }*/
}


void command_received_isr(uint8_t * p_data, uint32_t length)
{
  /*fifo_write(&ff_prot_cmd, p_data);*/
  printf("erpc got data!\n");
}

#endif
