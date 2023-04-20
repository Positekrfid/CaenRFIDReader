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

#include <stdlib.h>
#include <string.h>
#include "CAENRFIDTypes_Light.h"
#include "IO_Light.h"


static uint16_t _cmdID = 0;

CAENRFIDErrorCodes CAENRFID_Connect(CAENRFIDReader* reader,
                                    CAENRFIDPort PortType,
                                    void* PortParams)
{
    if(reader->connect(&reader->_port_handle, (int16_t) PortType, PortParams) != 0) return CAENRFID_PortError;
    return CAENRFID_StatusOK;
}

CAENRFIDErrorCodes CAENRFID_Disconnect(CAENRFIDReader* reader)
{
    if(reader->disconnect(reader->_port_handle) != 0) return CAENRFID_PortError;
    return CAENRFID_StatusOK;
}

CAENRFIDErrorCodes CAENRFID_GetReadPoints(CAENRFIDReader* reader,
                                          char** Antennas[],
                                          int16_t* numAnt)
{
    (void) reader;
    getAntNames(Antennas, numAnt);
    return CAENRFID_StatusOK;
}

CAENRFIDErrorCodes CAENRFID_GetSourceNames(CAENRFIDReader* reader,
                                           char** Sources[],
                                           int16_t* numSrc)
{
    (void) reader;
    getSrcNames(Sources, numSrc);
    return CAENRFID_StatusOK;
}

CAENRFIDErrorCodes CAENRFID_GetFirmwareRelease(CAENRFIDReader* reader,
                                               char *FWRel)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_GETFWRELEASE;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_GETFWRELEASE, FWRel) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE,  &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes) result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetReaderInfo(CAENRFIDReader* reader,
                                          char* Model,
                                          char* SerialNum)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp, idx;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    char rInfo[MAX_MODEL_LENGTH + 1 + MAX_SERIAL_LENGTH];

    cmd = CMD_GETRDRINFO;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd)  != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_READERINFO, rInfo)  < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code)  != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    if((idx = (int16_t)strcspn(rInfo, " ")) != 0)
    {
        strncpy(Model, rInfo, idx);
        Model[idx] = 0;
        strncpy(SerialNum, &rInfo[idx + 1], strlen(rInfo) - idx);

    }

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetProtocol(CAENRFIDReader* reader,
                                        CAENRFIDProtocol Proto)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint32_t protocol = Proto;

    cmd = CMD_SETPROTOCOL;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_PROTOCOL_NAME, sizeof(protocol));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, sizeof(protocol), AVP_PROTOCOL_NAME, &protocol);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetProtocol(CAENRFIDReader* reader,
                                        CAENRFIDProtocol *Proto)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint32_t protocol;

    cmd = CMD_GETPROTOCOL;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if((tmp = getAVP(&rxtxbuf, AVP_PROTOCOL_NAME, &protocol)) < 0) goto exit_done;
    else if(tmp == 0) *Proto = (CAENRFIDProtocol) protocol;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetPower(CAENRFIDReader* reader,
                                     uint32_t Power)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_SETPOWER;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_POWER, sizeof(Power));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, sizeof(Power), AVP_POWER, &Power);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetPower(CAENRFIDReader* reader,
                                     uint32_t *Power)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_GETPOWER;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_POWER_GET, Power) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes) result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_AddReadPoint(CAENRFIDReader* reader,
                                         char *SourceName,
                                         char *ReadPoint)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_ADDREADPOINT;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(SourceName) + 1);
    rxtxbuf.size += sizeAVP(AVP_READPOINT_NAME, strlen(ReadPoint) + 1);

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(SourceName) + 1, AVP_SOURCE_NAME, SourceName);
    addAVP(&rxtxbuf, (uint16_t)strlen(ReadPoint) + 1, AVP_READPOINT_NAME, ReadPoint);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes) result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_RemoveReadPoint(CAENRFIDReader* reader,
                                            char *SourceName,
                                            char *ReadPoint)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_REMREADPOINT;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(SourceName) + 1);
    rxtxbuf.size += sizeAVP(AVP_READPOINT_NAME, strlen(ReadPoint) + 1);

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(SourceName) + 1, AVP_SOURCE_NAME, SourceName);
    addAVP(&rxtxbuf, (uint16_t)strlen(ReadPoint) + 1, AVP_READPOINT_NAME, ReadPoint);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_isReadPointPresent(CAENRFIDReader* reader,
                                               char* ReadPoint,
                                               char* SourceName,
                                               uint16_t* isPresent)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_CHECKRPINSRC;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_READPOINT_NAME, strlen(ReadPoint) + 1);
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(SourceName) + 1);

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(ReadPoint) + 1, AVP_READPOINT_NAME, ReadPoint);
    addAVP(&rxtxbuf, (uint16_t)strlen(SourceName) + 1, AVP_SOURCE_NAME, SourceName);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_BOOLEAN, isPresent) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetReadPointStatus(CAENRFIDReader* reader,
                                               char* ReadPoint,
                                               CAENRFIDReadPointStatus* Status)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint32_t status;

    cmd = CMD_CHECKANTENNA;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_READPOINT_NAME, strlen(ReadPoint) + 1);

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(ReadPoint) + 1, AVP_READPOINT_NAME, ReadPoint);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if((tmp = getAVP(&rxtxbuf, AVP_READPOINT_STATUS, &status)) < 0) goto exit_done;
    else if(tmp == 0) *Status = (CAENRFIDReadPointStatus) status;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_MatchReadPointImpedance(CAENRFIDReader* reader,
                                                    char* ReadPoint,
                                                    float* value)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = { 0 };
    uint32_t param = RPCMATCHRFALG, param_value = 0;

    cmd = CMD_MATCHRFIMPEDANCE;
    //build request
    rxtxbuf.size = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_READPOINT_NAME, strlen(ReadPoint) + 1);
    rxtxbuf.size += sizeAVP(AVP_CONFIGPARAMETER, sizeof(param));
    rxtxbuf.size += sizeAVP(AVP_CONFIGVALUE, sizeof(param_value));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(ReadPoint) + 1, AVP_READPOINT_NAME, ReadPoint);
    addAVP(&rxtxbuf, sizeof(param), AVP_CONFIGPARAMETER, &param);
    addAVP(&rxtxbuf, sizeof(param_value), AVP_CONFIGVALUE, &param_value);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes)tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_POWER_VSWR, value) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetSourceConfiguration(CAENRFIDReader* reader,
                                                   char* SourceName,
                                                   CAENRFID_SOURCE_Parameter Parameter,
                                                   uint32_t Value)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint32_t parameter = Parameter;

    cmd = CMD_SETSRCCONF;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(SourceName) + 1);
    rxtxbuf.size += sizeAVP(AVP_CONFIGPARAMETER, sizeof(parameter));
    rxtxbuf.size += sizeAVP(AVP_CONFIGVALUE, sizeof(Value));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(SourceName) + 1, AVP_SOURCE_NAME, SourceName);
    addAVP(&rxtxbuf, sizeof(parameter), AVP_CONFIGPARAMETER, &parameter);
    addAVP(&rxtxbuf, sizeof(Value), AVP_CONFIGVALUE, &Value);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetSourceConfiguration(CAENRFIDReader* reader,
                                                   char* SourceName,
                                                   CAENRFID_SOURCE_Parameter Parameter,
                                                   uint32_t* Value)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint32_t parameter = Parameter;

    cmd = CMD_GETSRCCONF;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(SourceName) + 1);
    rxtxbuf.size += sizeAVP(AVP_CONFIGPARAMETER, sizeof(parameter));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(SourceName) + 1, AVP_SOURCE_NAME, SourceName);
    addAVP(&rxtxbuf, sizeof(parameter), AVP_CONFIGPARAMETER, &parameter);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_CONFIGVALUE, Value) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetRS232(CAENRFIDReader* reader,
                                     uint32_t Baudrate,
                                     uint32_t DataBits,
                                     uint32_t StopBits,
                                     CAENRFID_RS232_Parity Parity,
                                     CAENRFID_RS232_FlowControl FlowControl)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint32_t parity = Parity, flowctrl = FlowControl;

    cmd = CMD_SETRS232;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_BAUDRATE, sizeof(Baudrate));
    rxtxbuf.size += sizeAVP(AVP_DATABITS, sizeof(DataBits));
    rxtxbuf.size += sizeAVP(AVP_STOPBITS, sizeof(StopBits));
    rxtxbuf.size += sizeAVP(AVP_PARITY, sizeof(parity));
    rxtxbuf.size += sizeAVP(AVP_FLOWCTRL, sizeof(flowctrl));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, sizeof(Baudrate), AVP_BAUDRATE, &Baudrate);
    addAVP(&rxtxbuf, sizeof(DataBits), AVP_DATABITS, &DataBits);
    addAVP(&rxtxbuf, sizeof(StopBits), AVP_STOPBITS, &StopBits);
    addAVP(&rxtxbuf, sizeof(parity), AVP_PARITY, &parity);
    addAVP(&rxtxbuf, sizeof(flowctrl), AVP_FLOWCTRL, &flowctrl);
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetBitrate(CAENRFIDReader* reader,
                                       CAENRFID_Bitrate Bitrate)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint16_t bitrate = Bitrate;

    cmd = CMD_SETRFLINKPROFILE;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_MODULATION, sizeof(bitrate));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, sizeof(bitrate), AVP_MODULATION, &bitrate);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetBitrate(CAENRFIDReader* reader,
                                       CAENRFID_Bitrate* Bitrate)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint16_t bitrate;

    cmd = CMD_GETRFLINKPROFILE;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if((tmp = getAVP(&rxtxbuf, AVP_MODULATION, &bitrate)) < 0) goto exit_done;
    else if(tmp == 0) *Bitrate = (CAENRFID_Bitrate) bitrate;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetFHSSMode(CAENRFIDReader* reader,
                                        uint16_t FHSSMode)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_SETFHMODE;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_BOOLEAN, sizeof(FHSSMode));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, sizeof(FHSSMode), AVP_BOOLEAN, &FHSSMode);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetFHSSMode(CAENRFIDReader* reader,
                                        uint16_t* FHSSMode)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_GETFHMODE;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_BOOLEAN, FHSSMode) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetRFChannel(CAENRFIDReader* reader,
                                         uint16_t RFChannel)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_SETCHANNEL;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_RFCHANNEL, sizeof(RFChannel));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, sizeof(RFChannel), AVP_RFCHANNEL, &RFChannel);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetRFChannel(CAENRFIDReader* reader,
                                         uint16_t* RFChannel)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_GETCHANNEL;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RFCHANNEL, RFChannel) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetRFRegulation(CAENRFIDReader* reader,
                                            CAENRFIDRFRegulations RFRegulation)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint16_t regulation = RFRegulation;

    cmd = CMD_SETRFREGULATION;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_RFREGULATION, sizeof(regulation));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, sizeof(regulation), AVP_RFREGULATION, &regulation);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetRFRegulation(CAENRFIDReader* reader,
                                            CAENRFIDRFRegulations *RFRegulation)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    uint16_t regulation;

    cmd = CMD_GETRFREGULATION;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if((tmp = getAVP(&rxtxbuf, AVP_RFREGULATION, &regulation)) < 0) goto exit_done;
    else if(tmp == 0) *RFRegulation = (CAENRFIDRFRegulations) regulation;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetIO(CAENRFIDReader* reader,
                                  uint32_t IORegister)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_SETIO;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_IOREGISTER, sizeof(IORegister));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, sizeof(IORegister), AVP_IOREGISTER, &IORegister);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetIO(CAENRFIDReader* reader,
                                  uint32_t* IORegister)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_GETIO;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_IOREGISTER, IORegister) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_SetIODirection(CAENRFIDReader* reader,
                                           uint32_t IODirection)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_SETIODIR;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_IOREGISTER, sizeof(IODirection));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, sizeof(IODirection), AVP_IOREGISTER, &IODirection);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetIODirection(CAENRFIDReader* reader,
                                           uint32_t* IODirection)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_GETIODIR;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf))!= 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_IOREGISTER, IODirection) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_ReadTagData_EPC_C1G2(CAENRFIDReader* reader,
                                                 CAENRFIDTag* Tag,
                                                 uint16_t Bank,
                                                 uint16_t ByteAddress,
                                                 uint16_t ByteLength,
                                                 uint8_t* Data,
                                                 uint32_t AccessPassword)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_G2READ;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(Tag->LogicalSource) + 1);
    rxtxbuf.size += sizeAVP(AVP_TAGIDLEN, sizeof(Tag->Length));
    rxtxbuf.size += sizeAVP(AVP_TAGID, Tag->Length);
    rxtxbuf.size += sizeAVP(AVP_MEMBANK, sizeof(Bank));
    rxtxbuf.size += sizeAVP(AVP_TAGADDRESS, sizeof(ByteAddress));
    rxtxbuf.size += sizeAVP(AVP_LENGTH, sizeof(ByteLength));
    if(AccessPassword != 0)
    {
        rxtxbuf.size += sizeAVP(AVP_G2PWD, sizeof(AccessPassword));
    }

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(Tag->LogicalSource) + 1, AVP_SOURCE_NAME, Tag->LogicalSource);
    addAVP(&rxtxbuf, sizeof(Tag->Length), AVP_TAGIDLEN, &Tag->Length);
    addAVP(&rxtxbuf, Tag->Length, AVP_TAGID, Tag->ID);
    addAVP(&rxtxbuf, sizeof(Bank), AVP_MEMBANK, &Bank);
    addAVP(&rxtxbuf, sizeof(ByteAddress), AVP_TAGADDRESS, &ByteAddress);
    addAVP(&rxtxbuf, sizeof(ByteLength), AVP_LENGTH, &ByteLength);
    if(AccessPassword != 0)
    {
        addAVP(&rxtxbuf, sizeof(AccessPassword), AVP_G2PWD, &AccessPassword);
    }
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_TAG_VALUE, Data) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_WriteTagData_EPC_C1G2(CAENRFIDReader* reader,
                                                  CAENRFIDTag* Tag,
                                                  uint16_t Bank,
                                                  uint16_t ByteAddress,
                                                  uint16_t ByteLength,
                                                  uint8_t* Data,
                                                  uint32_t AccessPassword)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_G2WRITE;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(Tag->LogicalSource) + 1);
    rxtxbuf.size += sizeAVP(AVP_TAGIDLEN, sizeof(Tag->Length));
    rxtxbuf.size += sizeAVP(AVP_TAGID, Tag->Length);
    rxtxbuf.size += sizeAVP(AVP_MEMBANK, sizeof(Bank));
    rxtxbuf.size += sizeAVP(AVP_TAGADDRESS, sizeof(ByteAddress));
    rxtxbuf.size += sizeAVP(AVP_LENGTH, sizeof(ByteLength));
    rxtxbuf.size += sizeAVP(AVP_TAG_VALUE, ByteLength);
    if(AccessPassword != 0)
    {
        rxtxbuf.size += sizeAVP(AVP_G2PWD, sizeof(AccessPassword));
    }

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(Tag->LogicalSource) + 1, AVP_SOURCE_NAME, Tag->LogicalSource);
    addAVP(&rxtxbuf, sizeof(Tag->Length), AVP_TAGIDLEN, &Tag->Length);
    addAVP(&rxtxbuf, Tag->Length, AVP_TAGID, Tag->ID);
    addAVP(&rxtxbuf, sizeof(Bank), AVP_MEMBANK, &Bank);
    addAVP(&rxtxbuf, sizeof(ByteAddress), AVP_TAGADDRESS, &ByteAddress);
    addAVP(&rxtxbuf, sizeof(ByteLength), AVP_LENGTH, &ByteLength);
    addAVP(&rxtxbuf, ByteLength, AVP_TAG_VALUE, Data);
    if(AccessPassword != 0)
    {
        addAVP(&rxtxbuf, sizeof(AccessPassword), AVP_G2PWD, &AccessPassword);
    }
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_LockTag_EPC_C1G2(CAENRFIDReader* reader,
                                             CAENRFIDTag *Tag,
                                             uint32_t Payload,
                                             uint32_t AccessPassword)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_G2LOCK;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(Tag->LogicalSource) + 1);
    rxtxbuf.size += sizeAVP(AVP_TAGIDLEN, sizeof(Tag->Length));
    rxtxbuf.size += sizeAVP(AVP_TAGID, Tag->Length);
    rxtxbuf.size += sizeAVP(AVP_PAYLOAD, sizeof(Payload));
    if(AccessPassword != 0)
    {
        rxtxbuf.size += sizeAVP(AVP_G2PWD, sizeof(AccessPassword));
    }

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(Tag->LogicalSource) + 1, AVP_SOURCE_NAME, Tag->LogicalSource);
    addAVP(&rxtxbuf, sizeof(Tag->Length), AVP_TAGIDLEN, &Tag->Length);
    addAVP(&rxtxbuf, Tag->Length, AVP_TAGID, Tag->ID);
    addAVP(&rxtxbuf, sizeof(Payload), AVP_PAYLOAD, &Payload);
    if(AccessPassword != 0)
    {
        addAVP(&rxtxbuf, sizeof(AccessPassword), AVP_G2PWD, &AccessPassword);
    }
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_KillTag_EPC_C1G2(CAENRFIDReader* reader,
                                             CAENRFIDTag *Tag,
                                             uint32_t Password)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_G2KILL;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(Tag->LogicalSource) + 1);
    rxtxbuf.size += sizeAVP(AVP_TAGIDLEN, sizeof(Tag->Length));
    rxtxbuf.size += sizeAVP(AVP_TAGID, Tag->Length);
    rxtxbuf.size += sizeAVP(AVP_G2PWD, sizeof(Password));

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(Tag->LogicalSource) + 1, AVP_SOURCE_NAME, Tag->LogicalSource);
    addAVP(&rxtxbuf, sizeof(Tag->Length), AVP_TAGIDLEN, &Tag->Length);
    addAVP(&rxtxbuf, Tag->Length, AVP_TAGID, Tag->ID);
    addAVP(&rxtxbuf, sizeof(Password), AVP_G2PWD, &Password);

    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_ProgramID_EPC_C1G2(CAENRFIDReader* reader,
                                               CAENRFIDTag *Tag,
                                               uint16_t nsi,
                                               uint32_t AccessPassword)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};

    cmd = CMD_G2PROGRAMID;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(Tag->LogicalSource) + 1);
    rxtxbuf.size += sizeAVP(AVP_TAGIDLEN, sizeof(Tag->Length));
    rxtxbuf.size += sizeAVP(AVP_TAGID, Tag->Length);
    rxtxbuf.size += sizeAVP(AVP_G2NSI, sizeof(nsi));
    if(AccessPassword != 0)
    {
        rxtxbuf.size += sizeAVP(AVP_G2PWD, sizeof(AccessPassword));
    }

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(Tag->LogicalSource) + 1, AVP_SOURCE_NAME, Tag->LogicalSource);
    addAVP(&rxtxbuf, sizeof(Tag->Length), AVP_TAGIDLEN, &Tag->Length);
    addAVP(&rxtxbuf, Tag->Length, AVP_TAGID, Tag->ID);
    addAVP(&rxtxbuf, sizeof(nsi), AVP_G2NSI, &nsi);
    if(AccessPassword != 0)
    {
        addAVP(&rxtxbuf, sizeof(AccessPassword), AVP_G2PWD, &AccessPassword);
    }
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_CustomCommand_EPC_C1G2(CAENRFIDReader* reader,
                                                   CAENRFIDTag *Tag,
                                                   uint8_t SubCmd,
                                                   uint16_t TxLen,
                                                   uint8_t *Data,
                                                   uint16_t RxLen,
                                                   uint32_t AccessPassword,
                                                   uint8_t *TRData)
{
    CAENRFIDErrorCodes ret = CAENRFID_CommunicationError;
    uint16_t  cmd;
    int16_t tmp;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = { 0 };

    cmd = CMD_G2CUSTOM;
    //build request
    rxtxbuf.size = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    if(Tag != NULL)
    {
        rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(Tag->LogicalSource) + 1);
        rxtxbuf.size += sizeAVP(AVP_TAGIDLEN, sizeof(Tag->Length));
        rxtxbuf.size += sizeAVP(AVP_TAGID, Tag->Length);
    }
    rxtxbuf.size += sizeAVP(AVP_SUBCMD, sizeof(SubCmd));
    if(TxLen != 0)
    {
        rxtxbuf.size += sizeAVP(AVP_LENGTH, sizeof(TxLen));
        rxtxbuf.size += sizeAVP(AVP_TAG_VALUE, TxLen);
    }
    rxtxbuf.size += sizeAVP(AVP_LENGTH, sizeof(RxLen));
    if(AccessPassword != 0)
    {
        rxtxbuf.size += sizeAVP(AVP_G2PWD, sizeof(AccessPassword));
    }

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    if(Tag != NULL)
    {
        addAVP(&rxtxbuf, (uint16_t)strlen(Tag->LogicalSource) + 1, AVP_SOURCE_NAME, Tag->LogicalSource);
        addAVP(&rxtxbuf, sizeof(Tag->Length), AVP_TAGIDLEN, &Tag->Length);
        addAVP(&rxtxbuf, Tag->Length, AVP_TAGID, Tag->ID);
    }
    addAVP(&rxtxbuf, 1, AVP_SUBCMD, &SubCmd);
    if(TxLen != 0)
    {
        addAVP(&rxtxbuf, sizeof(TxLen), AVP_LENGTH, &TxLen);
        addAVP(&rxtxbuf, TxLen, AVP_TAG_VALUE, Data);
    }
    addAVP(&rxtxbuf, sizeof(RxLen), AVP_LENGTH, &RxLen);
    if(AccessPassword != 0)
    {
        addAVP(&rxtxbuf, sizeof(AccessPassword), AVP_G2PWD, &AccessPassword);
    }
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes)tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd) != 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_TAG_VALUE, TRData) < 0) goto exit_done;
    if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
    ret = (CAENRFIDErrorCodes)result_code;

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_InventoryTag(CAENRFIDReader* reader,
                                         char* SourceName,
                                         uint16_t Bank,
                                         uint16_t MaskBitAddress,
                                         uint16_t MaskBitLength,
                                         uint8_t* Mask,
                                         uint16_t MaskLen,
                                         uint16_t flag,
                                         CAENRFIDTagList** TagList,
                                         uint16_t* Size)
{
    CAENRFIDErrorCodes ret = CAENRFID_LibraryError;
    uint16_t  cmd;
    int16_t tmp, pos;
    uint16_t result_code;
    IOBuffer_t rxtxbuf = {0};
    bool has_RSSI = false, has_compact = false, has_TID = false,
         has_XPC = false, has_PC = false, has_event_trigger = false,
         has_framed = false, has_continuous = false, has_mask = false;
    CAENRFIDTagList* list_el = NULL;
    uint16_t type = 0;

    flag &= 0x017f;
    if((flag & RSSI) == RSSI) has_RSSI =1;
    if((flag & FRAMED) == FRAMED) has_framed = 1;
    if((flag & CONTINUOS) == CONTINUOS) has_continuous = 1;
    if((flag & COMPACT) == COMPACT) has_compact = 1;
    if((flag & TID_READING) == TID_READING) has_TID = 1;
    if((flag & EVENT_TRIGGER) == EVENT_TRIGGER) has_event_trigger = 1;
    if((flag & XPC) == 0x40) has_XPC = 1;
    if((flag & PC) == 0x100) has_PC = 1;

    if(has_continuous != has_framed) return CAENRFID_InvalidParam;
    if(has_event_trigger && !has_framed) return CAENRFID_InvalidParam;

    if(MaskBitLength > 0 && Mask != NULL)
    {
        if(MaskBitLength/8 + ((MaskBitLength%8 != 0) ? 1 : 0) > MaskLen) return CAENRFID_InvalidParam;
        has_mask = true;
    }

    cmd = CMD_INVENTORY;
    //build request
    rxtxbuf.size  = HEADER_LEN;
    rxtxbuf.size += sizeAVP(AVP_COMMAND, sizeof(cmd));
    rxtxbuf.size += sizeAVP(AVP_SOURCE_NAME, strlen(SourceName) + 1);
    if(has_mask)
    {
        rxtxbuf.size += sizeAVP(AVP_MEMBANK, sizeof(Bank));
        rxtxbuf.size += sizeAVP(AVP_LENGTH, sizeof(MaskBitLength));
        rxtxbuf.size += sizeAVP(AVP_TAGID, MaskLen);
        rxtxbuf.size += sizeAVP(AVP_TAGADDRESS, sizeof(MaskBitAddress));
    }
    if(flag != 0)
    {
        rxtxbuf.size += sizeAVP(AVP_BITMASK, sizeof(flag));
    }

    if((rxtxbuf.memory = malloc(rxtxbuf.size)) == NULL) return CAENRFID_OutOfMemoryError;

    addHeader(_cmdID++, &rxtxbuf, rxtxbuf.size);
    addAVP(&rxtxbuf, sizeof(cmd), AVP_COMMAND, &cmd);
    addAVP(&rxtxbuf, (uint16_t)strlen(SourceName) + 1, AVP_SOURCE_NAME, SourceName);
    if(has_mask)
    {
        addAVP(&rxtxbuf, sizeof(Bank), AVP_MEMBANK, &Bank);
        addAVP(&rxtxbuf, sizeof(MaskBitLength), AVP_LENGTH, &MaskBitLength);
        addAVP(&rxtxbuf, MaskLen, AVP_TAGID, Mask);
        addAVP(&rxtxbuf, sizeof(MaskBitAddress), AVP_TAGADDRESS, &MaskBitAddress);
    }
    if(flag != 0)
    {
        addAVP(&rxtxbuf, sizeof(flag), AVP_BITMASK, &flag);
    }
    //send command and get reply
    if((tmp = sendReceive(reader, &rxtxbuf, &rxtxbuf)) != 0)
    {
        ret = (CAENRFIDErrorCodes) tmp;
        goto exit_done;
    }
    //extract data
    rxtxbuf.rpos = HEADER_LEN;
    if(getAVP(&rxtxbuf, AVP_COMMAND, &cmd)  != 0) goto exit_done;
    if(!has_framed)
    {
        *Size = 0;
        while(1)
        {
            if((list_el = malloc(sizeof(CAENRFIDTagList))) == NULL)
            {
                ret = CAENRFID_OutOfMemoryError;
                break;
            }
           list_el->Next = *TagList;
           memset(&list_el->Tag, 0, sizeof(list_el->Tag));
           if(!has_compact)
           {
               if(getAVP(&rxtxbuf, AVP_SOURCE_NAME, list_el->Tag.LogicalSource) != 0) break;
               if(getAVP(&rxtxbuf, AVP_READPOINT_NAME, list_el->Tag.ReadPoint) != 0) break;
               if(getAVP(&rxtxbuf, AVP_TIMESTAMP, list_el->Tag.TimeStamp) != 0) break;
               if(getAVP(&rxtxbuf, AVP_TAGTYPE, &type) != 0) break;
               list_el->Tag.Type = (CAENRFIDProtocol) type;
               if(getAVP(&rxtxbuf, AVP_TAGIDLEN, &(list_el->Tag.Length)) != 0) break;
               if(getAVP(&rxtxbuf, AVP_TAGID, list_el->Tag.ID) != 0) break;
           }
           else
           {
               pos = rxtxbuf.rpos;
               if(getAVP(&rxtxbuf, AVP_TAGID, list_el->Tag.ID) != 0) break;
               list_el->Tag.Length = (rxtxbuf.rpos - pos) - AVP_HEADLEN;
           }
           if(has_RSSI)
           {
               if(getAVP(&rxtxbuf, AVP_RSSI, &(list_el->Tag.RSSI))  != 0) break;
           }
           if(has_TID)
           {
               if(getAVP(&rxtxbuf, AVP_LENGTH, &(list_el->Tag.TIDLen))  != 0) break;
               if(list_el->Tag.TIDLen)
               {
                   if(getAVP(&rxtxbuf, AVP_TAG_VALUE, list_el->Tag.TID)  != 0) break;
               }
           }
           if(has_XPC)
           {
               if(getAVP(&rxtxbuf, AVP_XPC, list_el->Tag.XPC)  != 0) break;
           }
           if(has_PC)
           {
               if(getAVP(&rxtxbuf, AVP_PC, list_el->Tag.PC)  != 0) break;
           }
           *TagList = list_el;
           (*Size)++;
        }
        // if exited from previous loop, then last tag has not been linked to
        // TagList yet and must be removed.
        free(list_el);
        if(getAVP(&rxtxbuf, AVP_RESULT_CODE, &result_code) != 0) goto exit_done;
        ret = (CAENRFIDErrorCodes)result_code;
    }
    else
    {
        reader->_inventory_params.has_RSSI = has_RSSI;
        reader->_inventory_params.has_framed = has_framed;
        reader->_inventory_params.has_continuous = has_continuous;
        reader->_inventory_params.has_compact = has_compact;
        reader->_inventory_params.has_TID = has_TID;
        reader->_inventory_params.has_event_trigger = has_event_trigger;
        reader->_inventory_params.has_XPC = has_XPC;
        reader->_inventory_params.has_PC = has_PC;
        ret = CAENRFID_StatusOK;
    }

    exit_done:
    if(rxtxbuf.memory) free(rxtxbuf.memory);
    return (ret);
}

CAENRFIDErrorCodes CAENRFID_GetFramedTag(CAENRFIDReader* reader,
                                         bool* has_tag,
                                         CAENRFIDTag* Tag,
                                         bool* has_result_code)
{
    return (CAENRFIDErrorCodes) receiveFramedTag(reader, has_tag, Tag, has_result_code);
}

CAENRFIDErrorCodes CAENRFID_InventoryAbort(CAENRFIDReader* reader)
{
    return (CAENRFIDErrorCodes) sendAbort(reader);
}