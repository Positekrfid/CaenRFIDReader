/* --COPYRIGHT--,BSD
 * Copyright (c) 2022, CAEN RFID S.R.L.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of CAEN RFID S.R.L. nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "IO_Light.h"


#define STANDARD_RX_MSEC_TMO (5000)
#define FRAMED_RX_MSEC_TMO_FIRST (500)
#define FRAMED_RX_MSEC_TMO_OTHER (5000)

static char * AntName[] = {"Ant0","Ant1","Ant2","Ant3"};
static char * SrcName[] = {"Source_0","Source_1","Source_2","Source_3"};

static uint16_t get_short(uint8_t *buf)
{
    return ((((uint16_t) buf[0] & 0xFF) << 8) | (((uint16_t)buf[1]) & 0xFF));
}

static int16_t set_short(uint16_t value, uint8_t *buf)
{
    buf[0] = value >> 8;
    buf[1] = value & 0xFF;
    return 2;
}

static uint32_t get_long(uint8_t *buf)
{
    return((((uint32_t) buf[0] & 0xFF) << 24) |
           (((uint32_t) buf[1] & 0xFF) << 16) |
           (((uint32_t) buf[2] & 0xFF) <<  8) |
           (((uint32_t) buf[3])& 0xFF));
}

static int16_t set_long(uint32_t value, uint8_t *buf)
{
    buf[0] = value >> 24;
    buf[1] = value >> 16;
    buf[2] = value >> 8;
    buf[3] = value & 0xFF;
    return 4;
}

static float get_float(uint8_t* buf)
{
    float fvalue = 0;
    uint32_t ivalue = (((((uint32_t) buf[0] & 0xFF) << 24) |
                        (((uint32_t) buf[1] & 0xFF) << 16) |
                        (((uint32_t) buf[2] & 0xFF) <<  8) |
                        (((uint32_t) buf[3])& 0xFF)));

    memcpy(&fvalue, &ivalue, sizeof(ivalue));
    return fvalue;
}

static int16_t receiveAVP(CAENRFIDReader* reader, IOBuffer_t* rxbuf, uint16_t ms_tmo)
{
    int16_t len = AVP_HEADLEN, idx = rxbuf->wpos;

    if(len + idx > rxbuf->size) return(-1);
    //Receive AVP header
    if(reader->rx(reader->_port_handle, &rxbuf->memory[idx], len, ms_tmo) != 0) return (-1);
    if(get_short(&rxbuf->memory[idx]) != 0) return (-1);
    len = get_short(&rxbuf->memory[idx + 2]);
    if(len + idx > rxbuf->size) return (-1);
    len -= AVP_HEADLEN;
    if(len < 0) return (-1);
    idx += AVP_HEADLEN;
    //Receive AVP value
    if(reader->rx(reader->_port_handle, &rxbuf->memory[idx], len, ms_tmo) != 0) return (-1);
    rxbuf->wpos = idx + len;

    return (0);
}

void getAntNames(char ** Array[], int16_t* n)
{
    *Array=AntName;
    *n=4;
}

void getSrcNames(char ** Array[], int16_t* n)
{
    *Array=SrcName;
    *n=4;
}

void addHeader(uint16_t CmdID, IOBuffer_t* buf, uint16_t size)
{
    int16_t idx = 0;

    assert(buf->wpos + HEADER_LEN <= buf->size);

    idx += set_short(0x8001, &buf->memory[buf->wpos + idx]);
    idx += set_short(CmdID,  &buf->memory[buf->wpos + idx]);
    idx += set_long(CAEN_VENDOR,& buf->memory[buf->wpos + idx]);
    idx += set_short(size, &buf->memory[buf->wpos + idx]);

    buf->wpos += idx;
}

void addAVP(IOBuffer_t *buf, uint16_t len, uint16_t wtype, void *value)
{
    int16_t idx = 0;

    assert(buf->wpos + AVP_HEADLEN + len <= buf->size);

    idx += set_short(0, &buf->memory[buf->wpos + idx]); /* Reserved bits */
    idx += set_short(AVP_HEADLEN + len, &buf->memory[buf->wpos + idx]);
    idx += set_short(wtype, &buf->memory[buf->wpos + idx]);

    switch (wtype) {
    case AVP_COMMAND:
    case AVP_RESULT_CODE:
    case AVP_LENGTH:
    case AVP_TAGADDRESS:
    case AVP_TAGIDLEN:
    case AVP_BITMASK:
    case AVP_TAGTYPE:
    case AVP_MEMBANK:
    case AVP_RSSI:
    case AVP_G2NSI:
    case AVP_MODULATION:
    case AVP_BOOLEAN:
    case AVP_RFCHANNEL:
    case AVP_RFREGULATION:
        assert(len == sizeof(uint16_t));
        set_short(*(uint16_t *)value, &buf->memory[buf->wpos + idx]);
        break;
    case AVP_PROTOCOL_NAME:
    case AVP_POWER:
    case AVP_G2PWD:
    case AVP_CONFIGPARAMETER:
    case AVP_CONFIGVALUE:
    case AVP_PAYLOAD:
    case AVP_IOREGISTER:
    case AVP_BAUDRATE:
    case AVP_DATABITS:
    case AVP_STOPBITS:
    case AVP_PARITY:
    case AVP_FLOWCTRL:
        assert(len == sizeof(uint32_t));
        set_long(*(uint32_t *)value, &buf->memory[buf->wpos + idx]);
        break;
    case AVP_SOURCE_NAME:
        assert(len <= MAX_LOGICAL_SOURCE_NAME);
        memcpy(&buf->memory[buf->wpos + idx], (char *) value, len);
        break;
    case AVP_READPOINT_NAME:
        assert(len <= MAX_READPOINT_NAME);
        memcpy(&buf->memory[buf->wpos + idx], (char *) value, len);
        break;
    case AVP_TAGID:
        assert(len <= MAX_ID_LENGTH);
        memcpy(&buf->memory[buf->wpos + idx], (uint8_t *) value, len);
        break;
    case AVP_TAG_VALUE:
        assert(len <= MAX_TAG_VALUE_LENGTH);
        memcpy(&buf->memory[buf->wpos + idx], (uint8_t *) value, len);
        break;
    case AVP_SUBCMD:
        assert(len == 1);
        memcpy(&buf->memory[buf->wpos + idx], (uint8_t *)value, len);
        break;
    default:
        assert(0);
    }
    idx += len;
    buf->wpos += idx;
}

int16_t getAVP(IOBuffer_t *buf, uint16_t wtype, void *value)
{
    int16_t idx;
    uint16_t len, rtype;

    if(buf->rpos + AVP_HEADLEN > buf->size) return (-1);

     /* First 2 bytes are reserved */
    idx  = 2;
    len = get_short(&buf->memory[buf->rpos + idx]);
    idx += 2;
    rtype = get_short(&buf->memory[buf->rpos + idx]);
    idx += 2;
    if(rtype != wtype) return (1);
    if(buf->rpos + len > buf->size) return (-1);
    buf->rpos += idx;
    len -= AVP_HEADLEN;
    switch (rtype) {
        case AVP_RESULT_CODE:
        case AVP_LENGTH:
        case AVP_TAGIDLEN:
        case AVP_TAGADDRESS:
        case AVP_TAGTYPE:
        case AVP_COMMAND:
        case AVP_RSSI:
        case AVP_MODULATION:
        case AVP_BOOLEAN:
        case AVP_RFCHANNEL:
        case AVP_RFREGULATION:
        case AVP_BITMASK:
        case AVP_MEMBANK:
            if(len != sizeof(uint16_t)) return (-1);
            *(uint16_t *) value = get_short(&buf->memory[buf->rpos]);
            break;
        case AVP_PROTOCOL_NAME:
        case AVP_POWER_GET:
        case AVP_CONFIGVALUE:
        case AVP_READPOINT_STATUS:
        case AVP_IOREGISTER:
            if(len != sizeof(uint32_t)) return (-1);
            *(uint32_t *) value = get_long(&buf->memory[buf->rpos]);
            break;
        case AVP_TIMESTAMP:
            if(len != 2*sizeof(uint32_t)) return (-1);
            *(uint32_t *) value = get_long(&buf->memory[buf->rpos]);
            *(((uint32_t *) value) + 1) = get_long(&buf->memory[buf->rpos + 4]);
            break;
        case AVP_POWER_VSWR:
            if(len != sizeof(uint32_t)) return (-1);
            *(float *)value = get_float(&buf->memory[buf->rpos]);
            break;
        case AVP_TAGID:
            if(len > MAX_ID_LENGTH) return (-1);
            memcpy((uint8_t *) value, &buf->memory[buf->rpos], len);
            break;
        case AVP_SOURCE_NAME:
            if(len > MAX_LOGICAL_SOURCE_NAME) return (-1);
            memcpy((char *) value, &buf->memory[buf->rpos], len);
            break;
        case AVP_READPOINT_NAME:
            if(len > MAX_READPOINT_NAME) return (-1);
            memcpy((char *) value, &buf->memory[buf->rpos], len);
            break;
        case AVP_TAG_VALUE:
            //if(len > MAX_TAG_VALUE_LENGTH) return (-1); //<--Commented out because some custom commands
                                                          //   can exceed the MAX_TAG_VALUE_LENGTH limit
            memcpy((uint8_t *) value, &buf->memory[buf->rpos], len);
            break;
        case AVP_XPC:
            if(len != XPC_LENGTH) return (-1);
            memcpy((uint8_t *) value, &buf->memory[buf->rpos], len);
            break;
        case AVP_PC:
            if(len != PC_LENGTH) return (-1);
            memcpy((uint8_t *) value, &buf->memory[buf->rpos], len);
            break;
        case AVP_READERINFO:
            if(len > MAX_MODEL_LENGTH + 1 + MAX_SERIAL_LENGTH) return (-1);
            memcpy((char *) value, &buf->memory[buf->rpos], len);
            break;
        case AVP_GETFWRELEASE:
            if(len > MAX_FWREL_LENGTH) return (-1);
            memcpy((uint8_t *)value, &buf->memory[buf->rpos], len);
            break;
        default:
            return (-1);
    }
    buf->rpos += len;
    return (0);
}

int16_t sendReceive(CAENRFIDReader* reader, IOBuffer_t* txbuf, IOBuffer_t* rxbuf)
{
    int16_t tmp = 0;
    uint16_t sentCmdID;
    uint8_t header[HEADER_LEN] = {0};

    sentCmdID = get_short(txbuf->memory + 2);
    reader->clear_rx_data(reader->_port_handle);
    //send command
    //--note : time interval between bytes of command must
    // not exceed the reader timeout value, otherwise reader 
    // will disregard command
    reader->disable_irqs();
    tmp = reader->tx(reader->_port_handle, txbuf->memory, txbuf->size);
    reader->enable_irqs();
    rxbuf->size = 0;
    if(tmp != 0)
    {
        return CAENRFID_CommunicationError;
    }

    //get protocol header
    if(reader->rx(reader->_port_handle, header, HEADER_LEN, STANDARD_RX_MSEC_TMO) != 0)
    {
        return CAENRFID_CommunicationError;
    }

    //verify header
    uint16_t TxVer    = get_short(header);
    uint16_t CmdID    = get_short(header + 2);
    uint32_t VendorID = get_long(header + 4);
    uint16_t Length   = get_short(header + 8);

    if(Length == 0) Length = HEADER_LEN + sizeAVP(AVP_COMMAND, sizeof(uint16_t));
    if((TxVer != 0x0001) ||
       (VendorID != CAEN_VENDOR)||
       (CmdID != sentCmdID) ||
       (Length < HEADER_LEN)
       )
    {
        return CAENRFID_CommunicationError;
    }

    if((rxbuf->memory = realloc(rxbuf->memory, Length)) == NULL)
    {
        return CAENRFID_OutOfMemoryError;
    }
    memcpy(header, &rxbuf->memory[rxbuf->wpos], HEADER_LEN);
    rxbuf->size = Length;
    rxbuf->wpos = HEADER_LEN;
    rxbuf->rpos = 0;
    //get rest of data
    Length -= HEADER_LEN;
    if(Length != 0)
    {
        if(reader->rx(reader->_port_handle, &rxbuf->memory[rxbuf->wpos], Length, STANDARD_RX_MSEC_TMO) != 0)
        {
            rxbuf->size = 0;
            rxbuf->wpos = 0;
            free(rxbuf->memory);
            rxbuf->memory = NULL;
            return CAENRFID_CommunicationError;
        }
        rxbuf->wpos += Length;
    }

    return CAENRFID_StatusOK;
}

int16_t sendAbort(CAENRFIDReader* reader)
{
    uint8_t abort = UART_ABORT;

    if(reader->tx(reader->_port_handle, &abort, sizeof(abort)) != 0) return CAENRFID_CommunicationError;
    return CAENRFID_StatusOK;
}

int16_t receiveFramedTag(CAENRFIDReader* reader, bool* has_tag, CAENRFIDTag* Tag,
                         bool* has_result_code)
{
    int16_t ret = CAENRFID_LibraryError, pos;
    uint16_t type;
    uint8_t buf[AVP_HEADLEN + MAX_ID_LENGTH];  //getting AVPs one by one, AVP_ID is the largest we're expecting
    IOBuffer_t rxbuf;
    uint32_t tmo = FRAMED_RX_MSEC_TMO_FIRST;
    bool nextAVP = true;

    enum {
     STATE_FIRST_AVP_RECEIVED = 0,
     STATE_GET_SOURCE,
     STATE_GET_READPOINT,
     STATE_GET_TIMESTAMP,
     STATE_GET_TYPE,
     STATE_GET_ID_LENGTH,
     STATE_GET_ID,
     STATE_GET_RSSI,
     STATE_GET_TID_LENGTH,
     STATE_GET_TID,
     STATE_GET_XPC,
     STATE_GET_PC,
     STATE_GET_RESULT,
     STATE_TAG_DONE,
     STATE_EXIT_DONE,
    } state = STATE_FIRST_AVP_RECEIVED;

    rxbuf.size = sizeof(buf);
    rxbuf.memory = buf;

    *has_tag = false;
    *has_result_code = false;
    while(1)
    {
        if(nextAVP)
        {
            rxbuf.rpos = 0;
            rxbuf.wpos = 0;
            if(receiveAVP(reader, &rxbuf, tmo) != 0)
            {
                nextAVP = false;
                if(state == STATE_FIRST_AVP_RECEIVED)
                {
                    ret = CAENRFID_StatusOK;
                }
                else
                {
                    ret = CAENRFID_CommunicationError;
                }
                state = STATE_EXIT_DONE;
            }
        }

        switch(state)
        {
        case STATE_FIRST_AVP_RECEIVED:
            tmo = FRAMED_RX_MSEC_TMO_OTHER;
            if(reader->_inventory_params.has_compact) state = STATE_GET_ID;
            else state = STATE_GET_SOURCE;
            nextAVP = false;
            break;
        case STATE_GET_SOURCE:
            if(getAVP(&rxbuf, AVP_SOURCE_NAME, Tag->LogicalSource) != 0)
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
                break;
            }
            nextAVP = true;
            state = STATE_GET_READPOINT;
            break;
        case STATE_GET_READPOINT:
            if(getAVP(&rxbuf, AVP_READPOINT_NAME, Tag->ReadPoint) != 0)
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
                break;
            }
            state = STATE_GET_TIMESTAMP;
            break;
        case STATE_GET_TIMESTAMP:
            if(getAVP(&rxbuf, AVP_TIMESTAMP, Tag->TimeStamp) != 0)
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
                break;
            }
            state = STATE_GET_TYPE;
            break;
        case STATE_GET_TYPE:
            if(getAVP(&rxbuf, AVP_TAGTYPE, &type) != 0)
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
                break;
            }
            Tag->Type = (CAENRFIDProtocol) type;
            state = STATE_GET_ID_LENGTH;
            break;
        case STATE_GET_ID_LENGTH:
            if(getAVP(&rxbuf, AVP_TAGIDLEN, &Tag->Length) != 0)
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
                break;
            }
            else if(Tag->Length > MAX_ID_LENGTH)
            {
                nextAVP = false;
                ret = CAENRFID_LibraryError;
                state = STATE_EXIT_DONE;
                break;
            }
            state = STATE_GET_ID;
            break;
        case STATE_GET_ID:
            if(reader->_inventory_params.has_compact)
            {
                pos = rxbuf.rpos;
                if(getAVP(&rxbuf, AVP_TAGID, Tag->ID) != 0)
                {
                    nextAVP = false;
                    state = STATE_GET_RESULT;
                    break;
                }
                Tag->Length = (rxbuf.rpos - pos) - AVP_HEADLEN;
            }
            else
            {
                if(getAVP(&rxbuf, AVP_TAGID, Tag->ID) != 0)
                {
                    nextAVP = false;
                    state = STATE_GET_RESULT;
                    break;
                }
            }
            if(reader->_inventory_params.has_RSSI) state = STATE_GET_RSSI;
            else if(reader->_inventory_params.has_TID) state = STATE_GET_TID_LENGTH;
            else if(reader->_inventory_params.has_XPC) state = STATE_GET_XPC;
            else if(reader->_inventory_params.has_PC) state = STATE_GET_PC;
            else
            {
                nextAVP = false;
                state = STATE_TAG_DONE;
                break;
            }
            nextAVP = true;
            break;
        case STATE_GET_RESULT:
            if(getAVP(&rxbuf, AVP_RESULT_CODE,  &ret) != 0)
            {
                ret = CAENRFID_LibraryError;
            }
            else
            {
                *has_result_code = true;
            }
            nextAVP = false;
            state = STATE_EXIT_DONE;
            break;
        case STATE_TAG_DONE:
            *has_tag = true;
            ret = CAENRFID_StatusOK;
        case STATE_EXIT_DONE:
            return (ret);
        case STATE_GET_RSSI:
            if(getAVP(&rxbuf, AVP_RSSI, &Tag->RSSI) != 0)
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
                break;
            }
            if(reader->_inventory_params.has_TID) state = STATE_GET_TID_LENGTH;
            else if(reader->_inventory_params.has_XPC) state = STATE_GET_XPC;
            else if(reader->_inventory_params.has_PC) state = STATE_GET_PC;
            else
            {
                nextAVP = false;
                state = STATE_TAG_DONE;
            }
            break;
        case STATE_GET_TID_LENGTH:
            if(getAVP(&rxbuf, AVP_LENGTH, &Tag->TIDLen) != 0) 
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
                break;
            }
            else if(Tag->TIDLen > MAX_TID_SIZE)
            {
                nextAVP = false;
                ret = CAENRFID_LibraryError;
                state = STATE_EXIT_DONE;
                break;
            }
            if(Tag->TIDLen > 0) state = STATE_GET_TID;
            else if(reader->_inventory_params.has_XPC) state = STATE_GET_XPC;
            else if(reader->_inventory_params.has_PC) state = STATE_GET_PC;
            else
            {
                nextAVP = false;
                state = STATE_TAG_DONE;
            }
            break;
        case STATE_GET_TID:
            if(getAVP(&rxbuf, AVP_TAG_VALUE, Tag->TID) != 0)
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
                break;
            }
            if(reader->_inventory_params.has_XPC) state = STATE_GET_XPC;
            else if(reader->_inventory_params.has_PC) state = STATE_GET_PC;
            else
            {
                nextAVP = false;
                state = STATE_TAG_DONE;
            }
            break;
        case STATE_GET_XPC:
            if(getAVP(&rxbuf, AVP_XPC, Tag->XPC) != 0)
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
                break;
            }
            if(reader->_inventory_params.has_PC) state = STATE_GET_PC;
            else
            {
                nextAVP = false;
                state = STATE_TAG_DONE;
            }
            break;
        case STATE_GET_PC:
            if(getAVP(&rxbuf, AVP_PC, Tag->PC) != 0)
            {
                nextAVP = false;
                state = STATE_GET_RESULT;
            }
            else
            {
                nextAVP = false;
                state = STATE_TAG_DONE;
            }
            break;
        default:
            return CAENRFID_LibraryError;
        }
    }
}