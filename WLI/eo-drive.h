#ifndef _EO_DRIVE_H_
#define _EO_DRIVE_H_

#define	EO_SUCCESS			 0
#define EO_GENERAL_ERROR		-1
#define	EO_DEV_ERROR			-2
#define	EO_DEV_NOT_ATTACHED		-3
#define	EO_ARGUMENT_ERROR		-6
#define	EO_INVALID_HANDLE		-8


#ifdef __cplusplus
	extern"C" {
#endif

#define EO_API __declspec(dllimport)

EO_API int EO_GetHandleBySerial(short serial);
EO_API int EO_InitHandle();
EO_API int EO_InitAllHandles();
EO_API int EO_GetAllHandles(int *handles, int size);
EO_API int EO_NumberOfCurrentHandles();
EO_API void EO_ReleaseHandle(int handle);
EO_API void EO_ReleaseAllHandles();

EO_API int EO_Move(int handle, double position);
EO_API int EO_GetMaxCommand(int handle, double *maxCommand);
EO_API int EO_GetCommandPosition(int handle, double *position);
EO_API int EO_GetSerialNumber(int handle, int *serial);

#ifdef __cplusplus
	}
#endif

#endif
