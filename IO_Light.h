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

#ifndef SRC_IO_LIGHT_H_
#define SRC_IO_LIGHT_H_

#include "CAENRFIDTypes_Light.h"
#include "Protocol_Light.h"

typedef struct IOBuffer
{
    uint8_t         *memory;
    uint16_t         size;
    int16_t          rpos;
    int16_t          wpos;
} IOBuffer_t;

#define sizeAVP(avptype, len) (AVP_HEADLEN + (uint16_t)(len))

void getAntNames(char ** Array[], int16_t* n);
void getSrcNames(char ** Array[], int16_t* n);
void addHeader(uint16_t CmdID, IOBuffer_t* buf, uint16_t size);
void addAVP(IOBuffer_t *buf, uint16_t len, uint16_t wtype, void *value);
int16_t getAVP(IOBuffer_t *buf, uint16_t wtype, void *value);
int16_t sendReceive(CAENRFIDReader* reader, IOBuffer_t* txbuf, IOBuffer_t* rxbuf);
int16_t sendAbort(CAENRFIDReader* reader);
int16_t receiveFramedTag(CAENRFIDReader* reader, bool* has_tag, CAENRFIDTag* Tag,
                         bool* has_result_code);


#endif /* SRC_IO_LIGHT_H_ */