/*****************************************************************************
*
*  evnt_handler.h  - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__
#include "hci.h"
#include "socket.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef  __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//!  hci_wait_for_event
//!
//!  @param  usOpcode      command operation code
//!  @param  pRetParams    command return parameters
//!
//!  @return               none
//!
//!  @brief                Wait for event, pass it to the hci_event_handler and
//!                        update the event opcode in a global variable.
//
//*****************************************************************************
void
hci_wait_for_event(unsigned short usOpcode, void *pRetParams);

//*****************************************************************************
//
//!  hci_wait_for_data
//!
//!  @param  pBuf       data buffer
//!  @param  from       from information
//!  @param  fromlen    from information length
//!
//!  @return               none
//!
//!  @brief                Wait for data, pass it to the hci_event_handler
//!                        and update in a global variable that there is
//!                        data to read.
//
//*****************************************************************************
void
hci_wait_for_data(uint8_t *pBuf, uint8_t *from, uint8_t *fromlen);

//*****************************************************************************
//
//!  hci_dispatch_packet
//!
//!  @param         buffer - pointer to the received data buffer
//!                 buffer_size - the size of the passed buffer
//!
//!                 The function triggers Received event/data processing
//!
//!  @param         Pointer to the received data
//!  @return        none
//!
//!  @brief         The function triggers Received event/data processing. It is
//!                       called from the SPI library to receive the data
//
//*****************************************************************************
void
hci_dispatch_packet(uint8_t* buffer, uint16_t buffer_size);

#define M_BSD_RESP_PARAMS_OFFSET(hci_event_hdr)((char *)(hci_event_hdr) + HCI_EVENT_HEADER_SIZE)


typedef struct _bsd_accept_return_t {
  long iSocketDescriptor;
  long iStatus;
  sockaddr tSocketAddress;
} tBsdReturnParams;


typedef struct _bsd_read_return_t {
  long             iSocketDescriptor;
  long             iNumberOfBytes;
  unsigned long   uiFlags;
} tBsdReadReturnParams;

#define BSD_RECV_FROM_FROMLEN_OFFSET  (4)
#define BSD_RECV_FROM_FROM_OFFSET    (16)


typedef struct _bsd_select_return_t {
  long iStatus;
  unsigned long uiRdfd;
  unsigned long uiWrfd;
  unsigned long uiExfd;
} tBsdSelectRecvParams;


typedef struct _bsd_getsockopt_return_t {
  unsigned char ucOptValue[4];
  char iStatus;
} tBsdGetSockOptReturnParams;

typedef struct _bsd_gethostbyname_return_t {
  long retVal;
  long outputAddress;
} tBsdGethostbynameParams;

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __EVENT_HANDLER_H__

