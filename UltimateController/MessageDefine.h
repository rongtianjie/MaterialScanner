#ifndef MESSAGEDEFINE_H
#define MESSAGEDEFINE_H

#include "CrTypes.h"
#include "CrError.h"
#include "Text.h"
#include <unordered_map>

namespace cli
{
    text get_message_desc(CrInt32u code);

    // Error code category
    const std::unordered_map<CrInt32u, text> map_ERR_CAT
    {
        {SCRSDK::CrError_Generic, CLI_TEXT("Generic")},
        {SCRSDK::CrError_File,    CLI_TEXT("File   ")},
        {SCRSDK::CrError_Connect, CLI_TEXT("Connect")},
        {SCRSDK::CrError_Memory,  CLI_TEXT("Memory ")},
        {SCRSDK::CrError_Api,     CLI_TEXT("Api    ")},
        {SCRSDK::CrError_Polling, CLI_TEXT("Polling")},
        {SCRSDK::CrError_Adaptor, CLI_TEXT("Adaptor")},
        {SCRSDK::CrError_Device,  CLI_TEXT("Device ")},
        {SCRSDK::CrError_Contents,CLI_TEXT("Content")},
    };

    // Error code detail
    const std::unordered_map<CrInt32u, text> map_ERR_DETAIL
    {
        {SCRSDK::CrError_Generic_Unknown, CLI_TEXT("Uncategorized errors")},
        {SCRSDK::CrError_Generic_Notimpl, CLI_TEXT("Not implemented")},
        {SCRSDK::CrError_Generic_Abort, CLI_TEXT("Processing was aborted")},
        {SCRSDK::CrError_Generic_NotSupported, CLI_TEXT("Not supported")},
        {SCRSDK::CrError_Generic_SeriousErrorNotSupported, CLI_TEXT("Serious error not supported")},
        {SCRSDK::CrError_Generic_InvalidHandle, CLI_TEXT("Not valid handle")},
        {SCRSDK::CrError_Generic_InvalidParameter, CLI_TEXT("Invalid parameter")},

        {SCRSDK::CrError_File_Unknown, CLI_TEXT("Unknown file errors")},
        {SCRSDK::CrError_File_IllegalOperation, CLI_TEXT("Illegal operation (e.g., loading without opening)")},
        {SCRSDK::CrError_File_IllegalParameter, CLI_TEXT("Illegal parameter")},
        {SCRSDK::CrError_File_EOF, CLI_TEXT("EOF")},
        {SCRSDK::CrError_File_OutOfRange, CLI_TEXT("Operation, such as seek, is out of range")},
        {SCRSDK::CrError_File_NotFound, CLI_TEXT("File not found")},
        {SCRSDK::CrError_File_DirNotFound, CLI_TEXT("Directory not found")},
        {SCRSDK::CrError_File_AlreadyOpened, CLI_TEXT("Already opened")},
        {SCRSDK::CrError_File_PermissionDenied, CLI_TEXT("No access permission")},
        {SCRSDK::CrError_File_StorageFull, CLI_TEXT("Host storage is full")},
        {SCRSDK::CrError_File_AlreadyExists, CLI_TEXT("Already exists")},
        {SCRSDK::CrError_File_TooManyOpenedFiles, CLI_TEXT("Too many open files")},
        {SCRSDK::CrError_File_ReadOnly, CLI_TEXT("Read-Only file")},
        {SCRSDK::CrError_File_CantOpen, CLI_TEXT("Cannot open")},
        {SCRSDK::CrError_File_CantClose, CLI_TEXT("Cannot close")},
        {SCRSDK::CrError_File_CantDelete, CLI_TEXT("Cannot delete")},
        {SCRSDK::CrError_File_CantRead, CLI_TEXT("Cannot read")},
        {SCRSDK::CrError_File_CantWrite, CLI_TEXT("Cannot write")},
        {SCRSDK::CrError_File_CantCreateDir, CLI_TEXT("Cannot create a directory")},
        {SCRSDK::CrError_File_OperationAbortedByUser, CLI_TEXT("Processing was aborted by user")},
        {SCRSDK::CrError_File_UnsupportedOperation, CLI_TEXT("API not supported for the platform was called")},
        {SCRSDK::CrError_File_NotYetCompleted, CLI_TEXT("Operation is not completed")},
        {SCRSDK::CrError_File_Invalid, CLI_TEXT("The file is no longer valid because the volume for the file was altered")},
        {SCRSDK::CrError_File_StorageNotExist, CLI_TEXT("The specified network resource or device is no longer available")},
        {SCRSDK::CrError_File_SharingViolation, CLI_TEXT("Sharing violation")},
        {SCRSDK::CrError_File_Rotation, CLI_TEXT("Invalid file orientation")},
        {SCRSDK::CrError_File_SameNameFull, CLI_TEXT("Too many same-name files")},

        {SCRSDK::CrError_Connect_Unknown, CLI_TEXT("Other errors classified as connection except below")},
        {SCRSDK::CrError_Connect_Connect, CLI_TEXT("A connection request failed through the USB")},
        {SCRSDK::CrError_Connect_Release, CLI_TEXT("Release failed")},
        {SCRSDK::CrError_Connect_GetProperty, CLI_TEXT("Getting property failed")},
        {SCRSDK::CrError_Connect_SendCommand, CLI_TEXT("Sending command failed")},
        {SCRSDK::CrError_Connect_HandlePlugin, CLI_TEXT("Illegal handle plug-in")},
        {SCRSDK::CrError_Connect_Disconnected, CLI_TEXT("A connection disconnected")},
        {SCRSDK::CrError_Connect_TimeOut, CLI_TEXT("A connection operation timed out")},
        {SCRSDK::CrError_Reconnect_TimeOut, CLI_TEXT("Reconnection operations timed out")},
        {SCRSDK::CrError_Connect_FailRejected, CLI_TEXT("Connection rejected and failed")},
        {SCRSDK::CrError_Connect_FailBusy, CLI_TEXT("Connection failed due to processing in progress")},
        {SCRSDK::CrError_Connect_FailUnspecified, CLI_TEXT("Unspecified connection failure")},
        {SCRSDK::CrError_Connect_Cancel, CLI_TEXT("Connection canceled")},
        {SCRSDK::CrError_Connect_SessionAlreadyOpened, CLI_TEXT("A connection failed because camera was not ready")},
        {SCRSDK::CrError_Connect_ContentsTransfer_NotSupported, CLI_TEXT("This camera does not support content transfer")},

        {SCRSDK::CrError_Memory_Unknown, CLI_TEXT("Unknown memory error")},
        {SCRSDK::CrError_Memory_OutOfMemory, CLI_TEXT("Cannot allocate memory")},
        {SCRSDK::CrError_Memory_InvalidPointer, CLI_TEXT("Invalid pointer")},
        {SCRSDK::CrError_Memory_Insufficient, CLI_TEXT("Allocate memory insufficient")},

        {SCRSDK::CrError_Api_Unknown, CLI_TEXT("Unknown API error")},
        {SCRSDK::CrError_Api_Insufficient, CLI_TEXT("Incorrect parameter")},
        {SCRSDK::CrError_Api_InvalidCalled, CLI_TEXT("Invalid API call")},

        {SCRSDK::CrError_Polling_Unknown, CLI_TEXT("Unknown polling error")},
        {SCRSDK::CrError_Polling_InvalidVal_Intervals, CLI_TEXT("Invalid polling interval setting value")},

        {SCRSDK::CrError_Adaptor_Unknown, CLI_TEXT("Unknown adapter error")},
        {SCRSDK::CrError_Adaptor_InvaildProperty, CLI_TEXT("A property that doesn't exist was used")},
        {SCRSDK::CrError_Adaptor_GetInfo, CLI_TEXT("Getting information failed")},
        {SCRSDK::CrError_Adaptor_Create, CLI_TEXT("Creation failed")},
        {SCRSDK::CrError_Adaptor_SendCommand, CLI_TEXT("Sending command failed")},
        {SCRSDK::CrError_Adaptor_HandlePlugin, CLI_TEXT("Illegal handle plug-in")},
        {SCRSDK::CrError_Adaptor_CreateDevice, CLI_TEXT("Device creation failed")},
        {SCRSDK::CrError_Adaptor_EnumDecvice, CLI_TEXT("Enumeration of device information failed")},
        {SCRSDK::CrError_Adaptor_Reset, CLI_TEXT("Reset failed")},
        {SCRSDK::CrError_Adaptor_Read, CLI_TEXT("Read failed")},
        {SCRSDK::CrError_Adaptor_Phase, CLI_TEXT("Parse failed")},
        {SCRSDK::CrError_Adaptor_DataToWiaItem, CLI_TEXT("Failed to set data as WIA item")},
        {SCRSDK::CrError_Adaptor_DeviceBusy, CLI_TEXT("The setting side is busy")},
        {SCRSDK::CrError_Adaptor_Escape, CLI_TEXT("Escape failed")},

        {SCRSDK::CrError_Contents_Unknown, CLI_TEXT("Unknown content transfer error")},
        {SCRSDK::CrError_Contents_InvalidHandle, CLI_TEXT("Not valid content handle")},
        {SCRSDK::CrError_Contents_DateFolderList_NotRetrieved, CLI_TEXT("Date folder list not acquired")},
        {SCRSDK::CrError_Contents_ContentsList_NotRetrieved, CLI_TEXT("Content handle list not acquired")},
        {SCRSDK::CrError_Contents_Transfer_Unsuccess, CLI_TEXT("Content transfer failed")},
        {SCRSDK::CrError_Contents_Transfer_Cancel,CLI_TEXT("Not transferred due to successful content transfer cancel")},
        {SCRSDK::CrError_Contents_RejectRequest, CLI_TEXT("Rejected request")},

        {SCRSDK::CrError_Device_Unknown, CLI_TEXT("Unknown device error")},
    }; 

} // namespace cli

#endif // !MESSAGEDEFINE_H
