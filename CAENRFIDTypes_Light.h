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

#ifndef SRC_LIB_CAENRFIDTYPES_LIGHT_H_
#define SRC_LIB_CAENRFIDTYPES_LIGHT_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_ID_LENGTH                           64
#define XPC_LENGTH                              4
#define PC_LENGTH                               2
#define MAX_READPOINT_NAME                      5
#define MAX_LOGICAL_SOURCE_NAME                 30
#define MAX_TID_SIZE                            64
#define MAX_TAG_VALUE_LENGTH                    128
#define MAX_FWREL_LENGTH                        30
#define MAX_SWREL_LENGTH                        6
#define MAX_MODEL_LENGTH                        20
#define MAX_SERIAL_LENGTH                       20

 /*
     Error Codes
 */
typedef enum {
    // -------------------- GENERAL ERROR CODES
    CAENRFID_StatusOK                          =  0,  // Operation completed successfully
    CAENRFID_TooManyClientsError               = -1,  
    CAENRFID_PortError                         = -2,  
    CAENRFID_LibraryError                      = -3,
    CAENRFID_SocketError                       = -4,  
    CAENRFID_InvalidHandleError                = -5,  
    CAENRFID_OutOfMemoryError                  = -6,
    CAENRFID_CommunicationError                = -7,
    CAENRFID_CommunicationTimeOut              = -8,
    CAENRFID_ChannelExist                      = -9,
    CAENRFID_GenericError                      = -10, 
    CAENRFID_InvalidParam                      = -11,  // Invalid parameter error
    CAENRFID_ReaderBusy                        = -12,
    CAENRFID_EOF                               = -13,
    //--------------------- FIRMWARE ERROR CODES (communication specific)
    CAENRFID_ChanNameExistsError               = 100,
    CAENRFID_AddrAlreadyInUseError             = 101,
    CAENRFID_UnknowError                       = 102,
    CAENRFID_BadChannelError                   = 103,
    CAENRFID_InvalidSourceNamError             = 104,
    CAENRFID_InvalidChannelNameError           = 105,
    CAENRFID_TooManyChannelError               = 106,
    CAENRFID_TooManySourceError                = 107,
    CAENRFID_SourceNotInChannelError           = 109,
    CAENRFID_BadTimerValueError                = 110,
    CAENRFID_TriggerNameExistError             = 111,
    CAENRFID_TooManyTriggerError               = 112,
    CAENRFID_BadTriggerError                   = 113,
    CAENRFID_BadAddressError                   = 114,
    CAENRFID_InvalidProtocolError              = 115,
    CAENRFID_BadPortAddressError               = 116,
    CAENRFID_CannotConnectToServerError        = 117,
    CAENRFID_InvalidTriggerNameError           = 118,
    CAENRFID_InvalidTimeError                  = 119,
    CAENRFID_SourceNotFoundError               = 120,
    CAENRFID_TriggerNotFoundError              = 121,
    CAENRFID_ChannelNotFoundError              = 122,
    CAENRFID_BadReadPointError                 = 123,
    CAENRFID_ChannelBusyError                  = 124,
    CAENRFID_TriggerBusyError                  = 125,
    CAENRFID_InternalFilesystemError           = 126,
    CAENRFID_InvalidCommand                    = 127,
    CAENRFID_BadParameterValueError            = 128,
    CAENRFID_NotifyServerNotReadyError         = 129,
    CAENRFID_PowerValueOutofRangeError         = 183,
    CAENRFID_InvalidParameterError             = 200,
    CAENRFID_LogicalSourceDisabledError        = 201,
    CAENRFID_TagNotPresentError                = 202,
    CAENRFID_WritingTagError                   = 203,
    CAENRFID_ReadingTagError                   = 204,
    CAENRFID_BadTagAddressError                = 205,
    CAENRFID_InvalidFunctionError              = 206,
    CAENRFID_SelectUnselectError               = 207,
    CAENRFID_TagLockedError                    = 209,
    CAENRFID_UnsupportedError                  = 210,
    CAENRFID_PowerError                        = 211,
    CAENRFID_NonSpecificError                  = 212,
    CAENRFID_KillTagError                      = 213,
    CAENRFID_ChannelsFullError                 = 214,
    CAENRFID_ErrorMatchReadpoint               = 215
} CAENRFIDErrorCodes;

/*
    Logical Source Parameters Values
*/
typedef enum {
    EPC_C1G2_SESSION_S0     = 0,
    EPC_C1G2_SESSION_S1     = 1,
    EPC_C1G2_SESSION_S2     = 2,
    EPC_C1G2_SESSION_S3     = 3,
    EPC_C1G2_TARGET_A       = 0,
    EPC_C1G2_TARGET_B       = 1,
    EPC_C1G2_SELECTED_YES   = 3,
    EPC_C1G2_SELECTED_NO    = 2,
    EPC_C1G2_ALL_SELECTED   = 0,
    ISO18006B_DESB_ON       = 1,
    ISO18006B_DESB_OFF      = 0,
}CAENRFIDLogicalSourceConstants ;

/*
    Logical Source Parameters
*/
typedef enum {
    CONFIG_READCYCLE            = 0,
    CONFIG_OBSERVEDTHRESHOLD    = 1,
    CONFIG_LOSTTHRESHOLD        = 2,
    CONFIG_G2_Q_VALUE           = 3,
    CONFIG_G2_SESSION           = 4,
    CONFIG_G2_TARGET            = 5,
    CONFIG_G2_SELECTED          = 6,
    CONFIG_ISO18006B_DESB       = 7,
    CONFIG_DWELL_TIME           = 8,
    CONFIG_INV_COUNT            = 10,
    CONFIG_TID_LENGTH           = 13,
    CONFIG_QUIETTIME            = 15
} CAENRFID_SOURCE_Parameter;

/*
    Reader Port Types
*/
typedef enum {
    CAENRFID_RS232   = 0,
   //CAENRFID_RS485  = 1, Obsolete
    CAENRFID_TCP     = 2,
    CAENRFID_USB     = 3
} CAENRFIDPort;

/*
    Reader Serial Port Parity 
*/
typedef enum {
    CAENRS232_Parity_None      = 0,
    CAENRS232_Parity_Odd       = 1,
    CAENRS232_Parity_Even      = 2
} CAENRFID_RS232_Parity;

/*
    Reader Serial Port Flow Control
*/
typedef enum {
    CAENRFID_RS232_FlowControl_XonXoff    = 2,
    CAENRFID_RS232_FlowControl_Hardware   = 1,
    CAENRFID_RS232_FlowControl_None       = 0
}CAENRFID_RS232_FlowControl;

/*
    Reader Protocols
*/
typedef enum {
    CAENRFID_ISO18000_6B   = 0,
    CAENRFID_EPC_C1G1      = 1,
    CAENRFID_ISO18000_6A   = 2,
    CAENRFID_EPC_C1G2      = 3,
    CAENRFID_MULTIPROTOCOL = 4,
    CAENRFID_EPC119        = 5
} CAENRFIDProtocol;

/*
    EPCC1G2 Banks
*/
typedef enum {
    RESERVED   = 0,
    EPC_CAEN        = 1,
    TID        = 2,
    USER       = 3
} CAENRFIDMemBanks;

/*
    Inventory flag bit values
*/
typedef enum {
    RSSI                    = 0x0001,
    FRAMED                  = 0x0002,
    CONTINUOS               = 0x0004,
    COMPACT                 = 0x0008,
    TID_READING             = 0x0010,
    EVENT_TRIGGER           = 0x0020,
    XPC                     = 0x0040,
    PC                      = 0x0100
} CAENRFID_InventoryFlag;

/*
    Antenna Status
*/
typedef enum {
    STATUS_GOOD     = 0,
    STATUS_POOR     = 1,
    STATUS_BAD      = 2
} CAENRFIDReadPointStatus;

/*
    RF Bitrate
*/
typedef enum{
    TX10RX40 = 1,
    TX40RX40 = 2,
    TX40RX160 = 3,
    DSB_ASK_FM0_TX10RX40 = 1,
    DSB_ASK_FM0_TX40RX40 = 2,
    DSB_ASK_FM0_TX40RX160 = 3,
    DSB_ASK_FM0_TX160RX400 = 4,
    DSB_ASK_M2_TX40RX160 = 5,
    PR_ASK_M4_TX40RX250 = 6,
    PR_ASK_M4_TX40RX300 = 7,
    PR_ASK_M2_TX40RX250 = 8,
    DSB_ASK_M4_TX40RX256 = 10,
    PR_ASK_M4_TX40RX320 = 11,
    PR_ASK_FM0_TX40RX640 = 12,
    PR_ASK_M4_TX80RX320 = 13,
    PR_ASK_M4_TX40RX256 = 14,
    PR_ASK_M8_TX40RX256 = 15,
    PR_ASK_M2_TX133RX640 = 16,
    PR_ASK_M2_TX50RX320 = 17,
    PR_ASK_M4_TX50RX320 = 18,
    PR_ASK_M4_TX50RX250 = 19,
    PR_ASK_FM0_TX133RX640 = 20,
    PR_ASK_M2_TX66RX320 = 21,
    PR_ASK_M8_TX50RX160 = 22,
    PR_ASK_M4_TX133RX640 = 23,
    DSB_ASK_FM0_TX160RX640 = 24,
    DSB_ASK_M2_TX160RX640 = 25
}CAENRFID_Bitrate;

/*
    RF regulations
*/
typedef enum{
    ETSI_302208     = 0,
    ETSI_300220     = 1,
    FCC_US          = 2,
    MALAYSIA        = 3,
    JAPAN           = 4,
    KOREA           = 5,
    AUSTRALIA       = 6,
    CHINA           = 7,
    TAIWAN          = 8,
    SINGAPORE       = 9,
    BRAZIL          = 10,
    JAPAN_STD_T106  = 11,
    JAPAN_STD_T107  = 12,
    PERU            = 13,
    SOUTH_AFRICA    = 14,
    CHILE           = 15
}CAENRFIDRFRegulations;

/*
    Antenna Matching Parameters
*/
typedef enum{
    RPCMATCHRFALG =14
}CAENRFIDMatchingParams;

/*
    Tag identification struct
*/
typedef struct CAENRFIDTag_s {
    uint8_t             ID[MAX_ID_LENGTH];
    uint16_t            Length;
    char                LogicalSource[MAX_LOGICAL_SOURCE_NAME];
    char                ReadPoint[MAX_READPOINT_NAME];
    uint32_t            TimeStamp[2];
    CAENRFIDProtocol    Type;
    int16_t             RSSI;
    uint8_t             TID[MAX_TID_SIZE];
    uint16_t            TIDLen;
    uint8_t             XPC[XPC_LENGTH];
    uint8_t             PC[PC_LENGTH];
} CAENRFIDTag;

/*
    Element of a list of tags
*/
typedef struct CAENRFIDTagList_s {
    CAENRFIDTag Tag;
    struct CAENRFIDTagList_s* Next;
} CAENRFIDTagList;

/*
    Inventory Parameters Struct : For internal use only 
*/
typedef struct CAENRFIDInventoryParams_s {
    bool has_RSSI;
    bool has_framed;
    bool has_continuous;
    bool has_compact;
    bool has_TID;
    bool has_event_trigger;
    bool has_XPC;
    bool has_PC;
} CAENRFIDInventoryParams;

/*
    Reader Struct 

    User should initialize the following fields:
    - connect
    - disconnect
    - tx
    - rx
    - clear_rx_data
    - enable_irqs
    - disable_irqs
    
    User should NOT modify the following fields:
     - _port_handle
     - _inventory_params
*/
typedef struct CAENRFIDReader_s {
    /*
    ---------------------------------------------------------------
      _port_handle - An handle that identifies the reader port
      For internal use only : do not modify.
    ---------------------------------------------------------------
    */
    void*   _port_handle;

    /*
    ---------------------------------------------------------------
     connect - Opens a connection with the reader
    ---------------------------------------------------------------
     Parameters:
     [out] port_handle   :   handle to the reader port
     [in]  port_type     :   the type of port 
     [in]  port_params   :   the connection parameters
     ---------------------------------------------------------------
      Returns:
       (0) : Success
      (-1) : Failure
    */
    //int16_t (*connect)(void* *port_handle, int16_t port_type, void* port_params);
      int16_t (*connect)(uint32_t baud_rate);
 

    /*
    ---------------------------------------------------------------
     disconnect - Closes the connection with the reader
    ---------------------------------------------------------------
     Parameters:
     [in]  port_handle   :   handle to the reader port
     ---------------------------------------------------------------
      Returns:
       (0) : Success
      (-1) : Failure
    */
    int16_t (*disconnect)(void* port_handle);

    /*
    ---------------------------------------------------------------
     tx - Transmits data to the reader
    ---------------------------------------------------------------
     Parameters:
     [in]  port_handle   :   handle to the reader port
     [in]  data          :   the bytes data to be sent to the reader
     [in]  len           :   the number of bytes to be sent
     ---------------------------------------------------------------
      Returns:
       (0) : Success
      (-1) : Failure
    */
    int16_t (*tx)(void* port_handle, uint8_t* data, uint32_t len);

    /*
    ---------------------------------------------------------------
     rx - Receives data from the reader
    ---------------------------------------------------------------
     Parameters:
     [in]  port_handle   :   handle to the reader port
     [out] data          :   the data received from the reader
     [in]  len           :   the number of bytes to be received
     [in]  ms_timeout    :   the timeout in milliseconds for 
                             reception to be completed.
     ---------------------------------------------------------------
      Returns:
       (0) : Success
      (-1) : Failure
    */
    int16_t (*rx)(void* port_handle, uint8_t* data, uint32_t len, uint32_t ms_timeout);

    /*
     ---------------------------------------------------------------
     clear_rx_data - Clears data present in the receive buffer, 
                     that is any data sent by the reader but not
                     parsed.
     ---------------------------------------------------------------
     Parameters:
     [in]  port_handle   :   handle to the reader port
     ---------------------------------------------------------------
     Returns:
       (0) : Success
      (-1) : Failure
    */
    int16_t (*clear_rx_data)(void* port_handle);

    /*
     ---------------------------------------------------------------
     enable_irqs - Enables host interrupts not related to tx/rx
                   communication with the reader.
     ---------------------------------------------------------------
     Parameters:
     ---------------------------------------------------------------
     Returns:
    */
    void    (*enable_irqs)(void); 

    /*
     ---------------------------------------------------------------
     disable_irqs - Disables host interrupts not related to tx/rx
                    communication with the reader.
     ---------------------------------------------------------------
     Parameters:
     ---------------------------------------------------------------
     Returns:
    */
    void    (*disable_irqs)(void);

    /*
    ---------------------------------------------------------------
      inventory_params - A struct containing information about
                         reader ongoing framed inventory.
      WARNING : For internal use only - DO NOT MODIFY!!
    ---------------------------------------------------------------
    */
    struct CAENRFIDInventoryParams_s  _inventory_params;

} CAENRFIDReader;




#endif /* SRC_LIB_CAENRFIDTYPES_LIGHT_H_ */
