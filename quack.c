#include <stdio.h>
#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>  
#include <devguid.h>
#include <regstr.h>

int main() {
    GUID hidGuid;
    HDEVINFO deviceInfoSet;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData;
    DWORD bufferSize = 0;
    DWORD memberIndex = 0;
    HANDLE deviceHandle;

    // Define the HID device GUID
    HidD_GetHidGuid(&hidGuid);

    // Get a handle to all devices that are part of the HID class
    deviceInfoSet = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    while (SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &hidGuid, memberIndex, &deviceInterfaceData)) {
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &bufferSize, NULL);

        deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(bufferSize);
        deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceInterfaceDetailData, bufferSize, NULL, NULL)) {
            // Open the device using the device path
            deviceHandle = CreateFile(deviceInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
            
            if (deviceHandle != INVALID_HANDLE_VALUE) {
                // Check if the device has a specific Vendor ID and Product ID
                HIDD_ATTRIBUTES hidAttributes;
                hidAttributes.Size = sizeof(HIDD_ATTRIBUTES);
                
                if (HidD_GetAttributes(deviceHandle, &hidAttributes)) {
                    if (hidAttributes.VendorID == 0x1234 && hidAttributes.ProductID == 0x5678) {
                        printf("Rubber ducky detected!\n");
                        // Take appropriate action when a rubber ducky is detected
                    }
                }
                CloseHandle(deviceHandle);
            }
        }

        free(deviceInterfaceDetailData);
        memberIndex++;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    return 0;
}
