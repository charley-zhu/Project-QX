/* Copyright (C) Micro-Epsilon Messtechnik GmbH & Co. KG
 *
 * This file is part of MEDAQLib Software Development Kit.
 * All Rights Reserved.
 *
 * THIS SOURCE FILE IS THE PROPERTY OF MICRO-EPSILON MESSTECHNIK AND
 * IS NOT TO BE RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE
 * EXPRESSED WRITTEN CONSENT OF MICRO-EPSILON MESSTECHNIK.
 *
 * Contact information:
 *    mailto:info@micro-epsilon.de
 *    http://www.micro-epsilon.de
 *
 */

#pragma once
#include <windows.h>

#ifdef MEDAQLIB_EXPORTS
#define MEDAQLIB_API
#else
#define MEDAQLIB_API __declspec(dllimport)
#endif

/******************************************************************************
                            Data type declaration
BOOL      if a four byte integer type which can be
          FALSE (0) or TRUE (!=0)                                (int)
DWORD     is a four byte unsigned integer type                   (unsigned long)
DWORD_PTR is a four byte unsigned integer type on Win32 systems  (unsigned long)
          or a eight byte unsigned integer type on Win64 systems (unsigned __int64)
WORD      is a two byte unsigned integer type                    (unsigned short)
BYTE      is an one byte unsigned integer type                   (unsigned char)
LPCSTR    is a pointer to a constant character string            (const char *)
LPSTR     is a pointer to a character string                     (char *)
LPCWSTR   is a pointer to a constant unicode string              (const short *)
LPWSTR    is a pointer to a unicode string                       (short *)
LPCTSTR   is a pointer to a constant ANSI string                 (const char *)
          or unicode string (depending on defined UNICODE)       (const short *)
LPTSTR    is a pointer to a ANSI string                          (char *)
          or unicode string (depending on defined UNICODE)       (short *)
ERR_CODE  is an enumeration of error codes                       (int)
int       is a four byte signed integer type
double    is an eight byte floating point type
float     is a four byte floating point type
*         function parameters with * are pointers to the type. They are used to 
          return information from the function. In some languages it is called
          "call by reference"

FALSE     Defined as zero (0)
TRUE      Defined as nonzero (1)

MEDAQLIB_API is defined as __declspec(dllimport) when using the
driver. When linking with MEDAQLib.lib all funktions with this
specifier are found in the library.

WINAPI is defined as __stdcall, which defines the calling convention.
The arguments are submitted from right to left to the function. 
They are written as value (call by value) onto the stack (unless they 
are referenced as a pointer). The called function reads the arguments 
from the stack.
******************************************************************************/

/******************************************************************************
                       Resolve windows specific defines
For some parsers (like LabView import DLL tool), this defines might be helpful
******************************************************************************/
#if defined (MEDAQLIB_WINDOWS_SPECIFIC)
#define WINAPI    __stdcall
#define DWORD     unsigned long
#ifdef _WIN64
#define DWORD_PTR unsigned __int64
#else
#define DWORD_PTR unsigned long
#endif
#define BOOL      int
#define LPCSTR    const char *
#define LPCWSTR   const short *
#define LPSTR     char *
#define LPWSTR    short *

#ifndef _UNICODE
#define LPTSTR    LPSTR
#define LPCTSTR   LPCSTR
#else
#define LPTSTR    LPWSTR
#define LPCTSTR   LPCWSTR
#endif // _UNICODE
#endif

/******************************************************************************
                              Enumerations
In function calls enumerations can be used textual or as number
******************************************************************************/
typedef enum
	{
	NO_SENSOR=            0, // Dummy, only for internal use
	SENSOR_ILR110x_115x= 19, // optoNCDT ILR
	SENSOR_ILR118x=      20, // optoNCDT ILR
	SENSOR_ILR1191=      21, // optoNCDT ILR
	SENSOR_ILD1302=      24, // optoNCDT
	SENSOR_ILD1320=      41, // optoNCDT
	SENSOR_ILD1401=       1, // optoNCDT
	SENSOR_ILD1402=      23, // optoNCDT
	SENSOR_ILD1420=      42, // optoNCDT
	SENSOR_ILD1700=       2, // optoNCDT
	SENSOR_ILD2200=       5, // optoNCDT
	SENSOR_ILD2300=      29, // optoNCDT
	SENSOR_IFD2401=      12, // confocalDT
	SENSOR_IFD2431=      13, // confocalDT
	SENSOR_IFD2445=      39, // confocalDT
	SENSOR_IFD2451=      30, // confocalDT
	SENSOR_IFD2461=      44, // confocalDT
	SENSOR_IFD2471=      26, // confocalDT
	SENSOR_ODC1202=      25, // optoCONTROL
	SENSOR_ODC2500=       8, // optoCONTROL
	SENSOR_ODC2520=      37, // optoCONTROL
	SENSOR_ODC2600=       9, // optoCONTROL
	SENSOR_LLT27xx=      31, // scanCONTROL+gapCONTROL, only for SensorFinder functionality, OpenSensor will fail
	SENSOR_DT3100=       28, // eddyNCDT
	SENSOR_DT6100=       16, // capaNCDT
	SENSOR_DT6120=       40, // capaNCDT
	CONTROLLER_DT6200=   33, // capaNCDT
	CONTROLLER_KSS6380=  18, // capaNCDT
	CONTROLLER_DT6500=   15, // capaNCDT
	SENSOR_ON_MEBUS=     43, // Generic sensor with MEbus protocol support, only for internal use
	ENCODER_IF2004=      10, // deprecated, use PCI_CARD_IF2004 instead
	PCI_CARD_IF2004=     10, // PCI card IF2004
	PCI_CARD_IF2008=     22, // PCI card IF2008
	CONTROLLER_CSP2008=  32, // Universal controller
	ETH_IF1032=          34, // Interface module Ethernet/EtherCAT
	USB_ADAPTER_IF2004=  36, // IF2004 USB adapter (4 x RS422)
	CONTROLLER_CBOX=     38, // External C-Box controller
	SENSOR_ACS7000=      35, // colorCONTROL
	NUMBER_OF_SENSORS=   44,
	} ME_SENSOR;

typedef enum
	{
	ERR_NOERROR= 0,
	ERR_FUNCTION_NOT_SUPPORTED= -1,
	ERR_CANNOT_OPEN= -2,
	ERR_NOT_OPEN= -3,
	ERR_APPLYING_PARAMS= -4,
	ERR_SEND_CMD_TO_SENSOR= -5,
	ERR_CLEARUNG_BUFFER= -6,
	ERR_HW_COMMUNICATION= -7,
	ERR_TIMEOUT_READING_FROM_SENSOR= -8,
	ERR_READING_SENSOR_DATA= -9,
	ERR_INTERFACE_NOT_SUPPORTED= -10,
	ERR_ALREADY_OPEN= -11,
	ERR_CANNOT_CREATE_INTERFACE= -12,
	ERR_NO_SENSORDATA_AVAILABLE= -13,
	ERR_UNKNOWN_SENSOR_COMMAND= -14,
	ERR_UNKNOWN_SENSOR_ANSWER= -15,
	ERR_SENSOR_ANSWER_ERROR= -16,
	ERR_SENSOR_ANSWER_TOO_SHORT= -17,
	ERR_WRONG_PARAMETER= -18,
	ERR_NOMEMORY= -19,
	ERR_NO_ANSWER_RECEIVED= -20,
	ERR_SENSOR_ANSWER_DOES_NOT_MATCH_COMMAND= -21,
	ERR_BAUDRATE_TOO_LOW= -22,
	ERR_OVERFLOW= -23,
	ERR_INSTANCE_NOT_EXIST= -24,
	ERR_NOT_FOUND= -25,
	ERR_WARNING= -26,
	ERR_SENSOR_ANSWER_WARNING= -27,
	} ERR_CODE;

/******************************************************************************
                              Function declaration
******************************************************************************/
#ifdef __cplusplus
extern "C"	// all functions are exported with "C" linkage (C decorated)
	{
#endif // __cplusplus
/* CreateSensorInstance
   Create an instance of the specified sensor driver and returns an index 
   greater 0. If the function fails, 0 is returned.                          */
	MEDAQLIB_API DWORD WINAPI CreateSensorInstance    (ME_SENSOR sensor);
	MEDAQLIB_API DWORD WINAPI CreateSensorInstByName  (LPCSTR sensorName);
	MEDAQLIB_API DWORD WINAPI CreateSensorInstByNameU (LPCWSTR sensorName);

/* ReleaseSensorInstance
   Frees the sensor driver instance previously created by CreateSensorInstance.
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI ReleaseSensorInstance (DWORD instanceHandle);

/* SetParameter
   Many different parameters can be specified for the sensors and interfaces.
	 For normalisation purpose they can be set and buffered with SetParameter 
	 functions. All other functions use the parameters to operate. 
	 If binary data containing binary zero "\0" should be written, the
	 function SetParameterBinary must be used and the complete length
	 of the binary data (in bytes) must be set.
	 See the manual for a list of parameters used in which function and for which 
	 sensor or interface.
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI SetParameterInt        (DWORD instanceHandle, LPCSTR paramName, int          paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetParameterDWORD_PTR  (DWORD instanceHandle, LPCSTR paramName, DWORD_PTR    paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetParameterDouble     (DWORD instanceHandle, LPCSTR paramName, double       paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetParameterString     (DWORD instanceHandle, LPCSTR paramName, LPCSTR       paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetParameterBinary     (DWORD instanceHandle, LPCSTR paramName, const char  *paramValue, DWORD len);
	MEDAQLIB_API ERR_CODE WINAPI SetParameters          (DWORD instanceHandle, LPCSTR parameterList);

	// Unicode versions
	MEDAQLIB_API ERR_CODE WINAPI SetParameterIntU       (DWORD instanceHandle, LPCWSTR paramName, int         paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetParameterDWORD_PTRU (DWORD instanceHandle, LPCWSTR paramName, DWORD_PTR   paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetParameterDoubleU    (DWORD instanceHandle, LPCWSTR paramName, double      paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetParameterStringU    (DWORD instanceHandle, LPCWSTR paramName, LPCWSTR     paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetParameterBinaryU    (DWORD instanceHandle, LPCWSTR paramName, const char *paramValue, DWORD len);
	MEDAQLIB_API ERR_CODE WINAPI SetParametersU         (DWORD instanceHandle, LPCWSTR parameterList);

/* GetParameter
   Results from any functions are stored and can be retrieved with GetParameter.
	 Values are stored in the second parameter (call by reference). When 
	 retrieving a string value, a string with enough space to hold the result 
	 must be passed. The length of the string must be specified in the length
	 parameter. The length of the result string is returned in the length 
	 parameter too. If the string parameter is not specified (null pointer) the
	 required length is returned in the parameter length.
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI GetParameterInt        (DWORD instanceHandle, LPCSTR paramName, int         *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI GetParameterDWORD_PTR  (DWORD instanceHandle, LPCSTR paramName, DWORD_PTR   *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI GetParameterDouble     (DWORD instanceHandle, LPCSTR paramName, double      *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI GetParameterString     (DWORD instanceHandle, LPCSTR paramName, LPSTR        paramValue, DWORD *maxLen);
	MEDAQLIB_API ERR_CODE WINAPI GetParameterBinary     (DWORD instanceHandle, LPCSTR paramName, char        *paramValue, DWORD *maxLen);
	MEDAQLIB_API ERR_CODE WINAPI GetParameters          (DWORD instanceHandle, LPSTR  parameterList,                      DWORD *maxLen);

	// Unicode versions
	MEDAQLIB_API ERR_CODE WINAPI GetParameterIntU       (DWORD instanceHandle, LPCWSTR paramName, int        *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI GetParameterDWORD_PTRU (DWORD instanceHandle, LPCWSTR paramName, DWORD_PTR  *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI GetParameterDoubleU    (DWORD instanceHandle, LPCWSTR paramName, double     *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI GetParameterStringU    (DWORD instanceHandle, LPCWSTR paramName, LPWSTR      paramValue, DWORD *maxLen);
	MEDAQLIB_API ERR_CODE WINAPI GetParameterBinaryU    (DWORD instanceHandle, LPCWSTR paramName, char       *paramValue, DWORD *maxLen);
	MEDAQLIB_API ERR_CODE WINAPI GetParametersU         (DWORD instanceHandle, LPWSTR  parameterList,                     DWORD *maxLen);

/* ClearAllParameters
   Before building a new sensor command the internal buffer can be cleared to ensure
	 that no old parameters affects the new command.
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI ClearAllParameters (DWORD instanceHandle);

/* OpenSensor
   Establish a connection to the sensor using the interface and parameters 
	 specified at SetParameter. 
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI OpenSensor (DWORD instanceHandle);

/* CloseSensor
   Close the connection to the connected sensor. 
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI CloseSensor (DWORD instanceHandle);

/* SensorCommand
   Send a command to the sensor and retrievs the answer. Both, command and
	 answer can be set/read with Set- and GetParameter.
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI SensorCommand (DWORD instanceHandle);

/* DataAvail
   Check if data from Sensor is available and returns the number of values.
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI DataAvail (DWORD instanceHandle, int *avail);

/* TransferData
   Retrievs specified amount of values from sensor and return the raw data 
	 in rawData (if not null) and the scaled data in scaledData (if not null). 
	 The first data in the buffer is returned and then removed from the buffer.
	 The actual data read is stored in variable read (if not null).
	 The second version stores the a timestamp of the first (oldest) value in the
	 array (if not null). It is in milli seconds starting at 01.01.1970 01:00.
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI TransferData (DWORD instanceHandle, int *rawData, double *scaledData, int maxValues, int *read);
	MEDAQLIB_API ERR_CODE WINAPI TransferDataTS (DWORD instanceHandle, int *rawData, double *scaledData, int maxValues, int *read, double *timestamp);

/* Poll
   Retrievs specified amount of values from sensor (max. one frame) and 
	 return the raw data in rawData (if not null) and the scaled data in 
	 scaledData (if not null). The latest values are returned, but no data is 
	 discarded in the sensor instance class (TransferData can be called 
	 independently from this).
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI Poll (DWORD instanceHandle, int *rawData, double *scaledData, int maxValues);

/* GetError
   If an error had occured, the error text can be retrieved with GetError. The
	 text is stored in errText limited to length of maxLen. 
	 Returns the number of the error returned in errText.                      */
	MEDAQLIB_API ERR_CODE WINAPI GetError (DWORD instanceHandle, LPSTR errText, DWORD maxLen);
	// Unicode version
	MEDAQLIB_API ERR_CODE WINAPI GetErrorU (DWORD instanceHandle, LPWSTR errText, DWORD maxLen);

/* GetDLLVersion
   Retrievs the version of the MEDAQLib dll. The version is stored in 
	 versionStr and is limited to length of maxLen (should be at least 64 bytes). 
	 Returns ERR_NOERROR on success, otherwise an error return value.          */
	MEDAQLIB_API ERR_CODE WINAPI GetDLLVersion (LPSTR versionStr, DWORD maxLen);
	// Unicode version
	MEDAQLIB_API ERR_CODE WINAPI GetDLLVersionU (LPWSTR versionStr, DWORD maxLen);

/* EnableLogging
	 Wrapper functions for a set of SetParameterXXX to enable logging.
	 This usage of this functions makes the code shorter and more readable.    */
	MEDAQLIB_API ERR_CODE WINAPI EnableLogging (DWORD instanceHandle, BOOL enableLogging, int logType, int logLevel, LPCSTR logFile, BOOL logAppend, BOOL logFlush, int logSplitSize);
	// Unicode version
	MEDAQLIB_API ERR_CODE WINAPI EnableLoggingU (DWORD instanceHandle, BOOL enableLogging, int logType, int logLevel, LPCWSTR logFile, BOOL logAppend, BOOL logFlush, int logSplitSize);

/* LogToFile
	 User function to allow an application to add lines to MEDAQLib log file.  
	 Because of variable argument list, only calling convention __cdecl is possible. */
	MEDAQLIB_API ERR_CODE __cdecl LogToFile (DWORD instanceHandle, int logLevel, LPCSTR location, LPCSTR message, ...);
	// Unicode version
	MEDAQLIB_API ERR_CODE __cdecl LogToFileU (DWORD instanceHandle, int logLevel, LPCWSTR location, LPCWSTR message, ...);

/* OpenSensorXXX
	 Wrapper functions for a set of SetParameterXXX and OpenSensor.
	 This usage of this functions makes the code shorter and more readable.    */
	MEDAQLIB_API ERR_CODE WINAPI OpenSensorRS232       (DWORD instanceHandle, LPCSTR port);
	MEDAQLIB_API ERR_CODE WINAPI OpenSensorIF2004      (DWORD instanceHandle, int cardInstance, int channelNumber);
	MEDAQLIB_API ERR_CODE WINAPI OpenSensorIF2004_USB  (DWORD instanceHandle, int deviceInstance, LPCSTR serialNumber, LPCSTR port, int channelNumber);
	MEDAQLIB_API ERR_CODE WINAPI OpenSensorIF2008      (DWORD instanceHandle, int cardInstance, int channelNumber);
	MEDAQLIB_API ERR_CODE WINAPI OpenSensorTCPIP       (DWORD instanceHandle, LPCSTR remoteAddr);
	MEDAQLIB_API ERR_CODE WINAPI OpenSensorWinUSB      (DWORD instanceHandle, int deviceInstance);
	// Unicode version
	MEDAQLIB_API ERR_CODE WINAPI OpenSensorRS232U      (DWORD instanceHandle, LPCWSTR port);
	MEDAQLIB_API ERR_CODE WINAPI OpenSensorIF2004_USBU (DWORD instanceHandle, int deviceInstance, LPCWSTR serialNumber, LPCWSTR port, int channelNumber);
	MEDAQLIB_API ERR_CODE WINAPI OpenSensorTCPIPU      (DWORD instanceHandle, LPCWSTR remoteAddr);

/* ExecSCmd
	 Wrapper functions for a set of Set/GetParameterXXX and SensorCommand.
	 This usage of this functions makes the code shorter and more readable.    */
	MEDAQLIB_API ERR_CODE WINAPI ExecSCmd          (DWORD instanceHandle, LPCSTR sensorCommand);
	MEDAQLIB_API ERR_CODE WINAPI SetIntExecSCmd    (DWORD instanceHandle, LPCSTR sensorCommand, LPCSTR paramName, int     paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetDoubleExecSCmd (DWORD instanceHandle, LPCSTR sensorCommand, LPCSTR paramName, double  paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetStringExecSCmd (DWORD instanceHandle, LPCSTR sensorCommand, LPCSTR paramName, LPCSTR  paramValue);
	MEDAQLIB_API ERR_CODE WINAPI ExecSCmdGetInt    (DWORD instanceHandle, LPCSTR sensorCommand, LPCSTR paramName, int    *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI ExecSCmdGetDouble (DWORD instanceHandle, LPCSTR sensorCommand, LPCSTR paramName, double *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI ExecSCmdGetString (DWORD instanceHandle, LPCSTR sensorCommand, LPCSTR paramName, LPSTR   paramValue, DWORD *maxLen);
	// Unicode version
	MEDAQLIB_API ERR_CODE WINAPI ExecSCmdU          (DWORD instanceHandle, LPCWSTR sensorCommand);
	MEDAQLIB_API ERR_CODE WINAPI SetIntExecSCmdU    (DWORD instanceHandle, LPCWSTR sensorCommand, LPCWSTR paramName, int     paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetDoubleExecSCmdU (DWORD instanceHandle, LPCWSTR sensorCommand, LPCWSTR paramName, double  paramValue);
	MEDAQLIB_API ERR_CODE WINAPI SetStringExecSCmdU (DWORD instanceHandle, LPCWSTR sensorCommand, LPCWSTR paramName, LPCWSTR paramValue);
	MEDAQLIB_API ERR_CODE WINAPI ExecSCmdGetIntU    (DWORD instanceHandle, LPCWSTR sensorCommand, LPCWSTR paramName, int    *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI ExecSCmdGetDoubleU (DWORD instanceHandle, LPCWSTR sensorCommand, LPCWSTR paramName, double *paramValue);
	MEDAQLIB_API ERR_CODE WINAPI ExecSCmdGetStringU (DWORD instanceHandle, LPCWSTR sensorCommand, LPCWSTR paramName, LPWSTR  paramValue, DWORD *maxLen);

#ifdef __cplusplus
	}
#endif // __cplusplus
