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

#ifndef SRC_PROTOCOL_LIGHT_H_
#define SRC_PROTOCOL_LIGHT_H_

#include "CAENRFIDTypes_Light.h"


#define CAEN_VENDOR            (21336)
#define HEADER_LEN                (10)
#define AVP_HEADLEN               ( 6)

#define UART_ABORT              (0xAB)

/*
    ---------------------------------------------------------------
    AVP Attributes definition
    ---------------------------------------------------------------
*/
#define AVP_COMMAND                         (0x01)  // 1
#define AVP_RESULT_CODE                     (0x02)  // 2
#define AVP_EVTTYPE                         (0x0E)  // 14
#define AVP_TAGIDLEN                        (0x0F)  // 15
#define AVP_TIMESTAMP                       (0x10)  // 16
#define AVP_TAGID                           (0x11)  // 17
#define AVP_TAGTYPE                         (0x12)  // 18 
#define AVP_CHANNEL_NAME                    (0x1E)  // 30
#define AVP_CHANNEL_ADDRESS                 (0x1F)  // 31
#define AVP_TRIGGER_NAME                    (0x20)  // 32
#define AVP_TRIGGER_TYPE                    (0x21)  // 33
#define AVP_READPOINT_NAME                  (0x22)  // 34
#define AVP_TAG_VALUE                       (0x4D)  // 77
#define AVP_TAGADDRESS                      (0x4E)  // 78
#define AVP_TESTMODE                        (0x4F)  // 79
#define AVP_LENGTH                          (0x50)  // 80
#define AVP_MODULATION                      (0x51)  // 81
#define AVP_POWER_GET                       (0x52)  // 82
#define AVP_POWER_VSWR                      (0x53)  // 83
#define AVP_PROTOCOL_NAME                   (0x54)  // 84
#define AVP_READPOINT                       (0x55)  // 85
#define AVP_READPOINT_STATUS                (0x56)  // 86
#define AVP_BOOLEAN                         (0x57)  // 87
#define AVP_IPADDRESS                       (0x58)  // 88
#define AVP_IPNETMASK                       (0x59)  // 89
#define AVP_IPGATEWAY                       (0x5A)  // 90
#define AVP_DE_SBENA                        (0x5B)  // 91
#define AVP_GETFWRELEASE                    (0x5C)  // 92
#define AVP_EXBIT_STATUS                    (0x5D)  // 93
#define AVP_G1PWD                           (0x5E)  // 94
#define AVP_RFONOFF                         (0x5F)  // 95
#define AVP_BAUDRATE                        (0x60)  // 96 
#define AVP_DATABITS                        (0x61)  // 97 
#define AVP_STOPBITS                        (0x62)  // 98 
#define AVP_PARITY                          (0x63)  // 99 
#define AVP_FLOWCTRL                        (0x64)  // 100 
#define AVP_DATETIME                        (0x65)  // 101 
#define AVP_SELUNSEL_OP                     (0x66)  // 102 
#define AVP_BITMASK                         (0x67)  // 103 
#define AVP_IOREGISTER                      (0x69)  // 105 
#define AVP_CONFIGPARAMETER                 (0x6A)  // 106 
#define AVP_CONFIGVALUE                     (0x6B)  // 107 
#define AVP_EVENTMODE                       (0x6E)  // 110 
#define AVP_MEMBANK                         (0x71)  // 113 
#define AVP_PAYLOAD                         (0x72)  // 114 
#define AVP_G2PWD                           (0x73)  // 115 
#define AVP_G2NSI                           (0x74)  // 116 
#define AVP_G2Q                             (0x75)  // 117 
#define AVP_READERINFO                      (0x76)  // 118 
#define AVP_RFREGULATION                    (0x77)  // 119 
#define AVP_RFCHANNEL                       (0x78)  // 120 
#define AVP_SUBCMD                          (0x79)  // 121 
#define AVP_RSSI                            (0x7A)  // 122 
#define AVP_OPTION                          (0x7B)  // 123
#define AVP_XPC                             (0x7C)  // 124 
#define AVP_PC                              (0x7D)  // 125 
#define AVP_PHASE                           (0x7E)  // 126 
#define AVP_TERMINAL_TYPE                   (0x7F)  // 127 
#define AVP_BATTERY_LEVEL                   (0x80)  // 128
#define AVP_LONG_LENGTH                     (0x81)  // 129
#define AVP_LONG_ADDRESS                    (0x82)  // 130 
#define AVP_UINT16                          (0x83)  // 131 
#define AVP_POWER                           (0x96)  // 150
#define AVP_SOURCE_NAME                     (0xFB)  // 251

/*
    ---------------------------------------------------------------
    AVP Command Value definitions
    ---------------------------------------------------------------
*/
#define CMD_RAWREADID                       (0x12)
#define CMD_INVENTORY                       (0x13)
#define CMD_ADDREADPOINT                    (0x5F)
#define CMD_REMREADPOINT                    (0x60)
#define CMD_SETPOWER                        (0x64)
#define CMD_READTAG                         (0x6E)
#define CMD_WRITETAG                        (0x6F)
#define CMD_LOCKTAG                         (0x70)
#define CMD_SETRFLINKPROFILE                (0x72)
#define CMD_GETPOWER                        (0x73)
#define CMD_SETPROTOCOL                     (0x74)
#define CMD_CHECKANTENNA                    (0x76)
#define CMD_CHECKRPINSRC                    (0x78)
#define CMD_GETPROTOCOL                     (0x79)
#define CMD_SETDESB                         (0x7B)
#define CMD_GETFWRELEASE                    (0x7C)
#define CMD_GETDESB                         (0x7D)
#define CMD_G1PROGRAMID                     (0x7E)
#define CMD_G1KILLTAG                       (0x7F)
#define CMD_RFONOFF                         (0x80)
#define CMD_GETRFLINKPROFILE                (0x81)
#define CMD_BLOCKWRITETAG                   (0x82)
#define CMD_SETRS232                        (0x83)
#define CMD_SETDATETIME                     (0x84)
#define CMD_GROUPSELUNSEL                   (0x85)
#define CMD_GETIO                           (0x86)
#define CMD_SETIO                           (0x87)
#define CMD_SETIODIR                        (0x88)
#define CMD_GETIODIR                        (0x89)
#define CMD_SETSRCCONF                      (0x8A)
#define CMD_GETSRCCONF                      (0x8B)
#define CMD_GETEVENTMODE                    (0x92)
#define CMD_E119PROGRAMID                   (0x94)
#define CMD_G2PROGRAMID                     (0x95)
#define CMD_G2READ                          (0x96)
#define CMD_G2WRITE                         (0x97)
#define CMD_G2LOCK                          (0x98)
#define CMD_G2KILL                          (0x99)
#define CMD_G2QUERY                         (0x9A)
#define CMD_G2SETQ                          (0x9B)
#define CMD_G2GETQ                          (0x9C)
#define CMD_G2QUERYACK                      (0x9D)
#define CMD_GETRDRINFO                      (0x9E)
#define CMD_SETFHMODE                       (0x9F)
#define CMD_GETFHMODE                       (0xA0)
#define CMD_SETRFREGULATION                 (0xA1)
#define CMD_GETRFREGULATION                 (0xA2)
#define CMD_SETCHANNEL                      (0xA3)
#define CMD_GETCHANNEL                      (0xA4)
#define CMD_G2RESETSESSION                  (0xA5)
#define CMD_G1SCROLLALLID                   (0xA6)
#define CMD_GETCHANNELDATA                  (0xA7) 
#define CMD_G2CUSTOM                        (0xA8)
#define CMD_GETCHANSTS                      (0xA9)  
#define CMD_GETRFCHANSTS                    (0xAA)
#define CMD_SETEXTLBT                       (0xAB)
#define CMD_SETADMINPWD                     (0xAC)
#define CMD_LOGIN                           (0xAD)
#define CMD_LOGOUT                          (0xAE)
#define CMD_GETBUFFEREDDATA                 (0xB0)
#define CMD_LOCKBLOCKPERMALOCK              (0xB1)  
#define CMD_READBLOCKPERMALOCK              (0xB2)  
#define CMD_SAVE_SETTINGS                   (0xB3)
#define CMD_MATCHRFIMPEDANCE                (0xB4)
#define CMD_GETBATTERYLEVEL                 (0xB6)
#define CMD_GETBUFFERSIZE                   (0xB7)
#define CMD_CLEARBUFFER                     (0xB8)
#define CMD_G2UNTRACEABLE                   (0xB9)
#define CMD_G2AUTHENTICATE                  (0xBA)
#define CMD_SETDAC                          (0xBB)
#define CMD_GETADC                          (0xBC)
#define CMD_SETREADPOINTPOWER               (0xBD)
#define CMD_GETREADPOINTPOWER               (0xBE)
#define CMD_G2BLOCKWRITE                    (0xBF)
#define CMD_G2BLOCKPROGRAMID                (0xC0)

#endif /* SRC_PROTOCOL_LIGHT_H_ */