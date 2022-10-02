#include<ndis.h>
#include<ntddk.h>
#include<fwpmk.h>
#include<malloc.h>

#define INITGUID

#include<guiddef.h>
#include<fwpmu.h>
#include<fwpsk.h>
#include<rpc.h>


#pragma comment(lib,"Rpcrt4.lib")

PDEVICE_OBJECT DeviceObject = NULL;
HANDLE EngineHandle = NULL;
UINT32 RegCalloutId = 0, AddCalloutId;
UINT64 filterid = 0;

void Notify_Process_Creation(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo);


VOID Unload(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	PsSetCreateProcessNotifyRoutineEx(Notify_Process_Creation, TRUE);

	IoDeleteDevice(DeviceObject);
	KdPrint(("Unload \r\n"));


}





void Notify_Process_Creation(PEPROCESS Process,HANDLE ProcessId,PPS_CREATE_NOTIFY_INFO CreateInfo){

	UNREFERENCED_PARAMETER(Process);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(CreateInfo);
	
	
	if (CreateInfo) {
		
		if (CreateInfo->CommandLine != NULL) {
			KdPrint(("Process Create >> %S \n ", CreateInfo->CommandLine->Buffer));
			KdPrint(("Process Create >> %S \n ", CreateInfo->ImageFileName->Buffer));
			if (CreateInfo->FileObject != NULL) {
				//KdPrint(("Process fILENAME >> %hs \n ", CreateInfo->FileObject->FileName.Buffer));
				//KdPrint(("Process fILENAME >> %d \n ", CreateInfo->FileObject->FileName.Length));

				
				int length = CreateInfo->FileObject->FileName.Length;
				char* str = (char*)ExAllocatePoolWithTag(PagedPool, length, 'TAG1');
				::memcpy(str, CreateInfo->FileObject->FileName.Buffer, length);
				KdPrint(("%S",str));
				/*
				for (int i = 0; i < length; i++) {
					KdPrint(("Process fILENAME >> %c  ", CreateInfo->FileObject->FileName.Buffer[i]));
					//KdPrint(("  "));
				}*/
				KdPrint(("\n  "));
			}
		


		}
		//KdPrint(("Process ImageFileName >> %s \n ", CreateInfo->ImageFileName));
		KdPrint(("Process ID is %d \n ",HandleToLong(ProcessId)));

	}
	
	/*else {


		KdPrint(("Process Removed\n"));
		if (CreateInfo->CommandLine != NULL) {
			KdPrint(("Process Create >> %s \n ", CreateInfo->CommandLine));


		}
		//KdPrint(("Process ImageFileName >> %s \n ", CreateInfo->ImageFileName));
		KdPrint(("Process ID is %d \n ", HandleToLong(ProcessId)));



	}*/
	
}




extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	DriverObject->DriverUnload = Unload;
	UNREFERENCED_PARAMETER(RegistryPath);

	KdPrint(("Hello"));
	status = IoCreateDevice(DriverObject, 0, NULL, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	KdPrint(("Hello2"));
	if (!NT_SUCCESS(status)) {
		return status;

	}
	KdPrint(("Hello3"));
	KdPrint(("Creatingg device SUCCESS \r\n"));
	status = PsSetCreateProcessNotifyRoutineEx(Notify_Process_Creation,FALSE);
	KdPrint(("Hello4"));
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(DeviceObject);

	}

	return status;
}
