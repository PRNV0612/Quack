#include <iostream>
#include <Windows.h>
#include <setupapi.h>
#include <winusb.h>
#include <usbioctl.h>

#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "Winusb.lib")

bool IsBadUSB(const WINUSB_INTERFACE_HANDLE& winusbHandle)
{
    // Check if the HID device has a specific identifier indicating BadUSB
    // i am assuming BadUSB has a vendor ID of 0xDEAD and product ID of 0xBEEF
    USB_DEVICE_DESCRIPTOR deviceDescriptor;
    if (WinUsb_GetDescriptor(winusbHandle, USB_DEVICE_DESCRIPTOR_TYPE, 0, 0, (PBYTE)&deviceDescriptor, sizeof(deviceDescriptor), NULL))
    {
        if (deviceDescriptor.idVendor == 0xDEAD && deviceDescriptor.idProduct == 0xBEEF)
            return true;
    }
    return false;
}

void MonitorUSBDevices()
{
    while (true)
    {
        // Get the device information set for all HID devices
        HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if (deviceInfoSet == INVALID_HANDLE_VALUE)
        {
            std::cout << "Failed to get device information set." << std::endl;
            continue;
        }

        SP_DEVICE_INTERFACE_DATA deviceInterfaceData = { sizeof(SP_DEVICE_INTERFACE_DATA) };
        DWORD deviceIndex = 0;
        while (SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, deviceIndex, &deviceInterfaceData))
        {
            DWORD requiredSize = 0;
            // Get buffer size for the required input device drivers
            SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

            // Allocate memory for the device interface detail
            PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetail = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(new char[requiredSize]);
            deviceInterfaceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            // Retrieve the device interface detail
            if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceInterfaceDetail, requiredSize, NULL, NULL))
            {
                HANDLE deviceHandle = CreateFile(deviceInterfaceDetail->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                if (deviceHandle != INVALID_HANDLE_VALUE)
                {
                    WINUSB_INTERFACE_HANDLE winusbHandle;
                    if (WinUsb_Initialize(deviceHandle, &winusbHandle))
                    {
                        // Check if the HID device is a BadUSB
                        if (IsBadUSB(winusbHandle))
                        {
                            std::cout << "Warning: BadUSB detected! Disabling incoming data from associated USB devices." << std::endl;
                            // Disable incoming data from associated HID devices by setting the feature report to zero
                            UCHAR report[1] = { 0 };
                            ULONG bytesTransferred;
                            if (!WinUsb_SetFeatureReport(winusbHandle, 0, report, sizeof(report), &bytesTransferred))
                            {
                                std::cout << "Failed to set feature report." << std::endl;
                            }
                        }
                    }
                    else
                    {
                        std::cout << "Failed to initialize WinUSB interface." << std::endl;
                    }

                    // Close the device handle
                    CloseHandle(deviceHandle);
                }
                else
                {
                    std::cout << "Failed to open device." << std::endl;
                }
            }
            else
            {
                std::cout << "Failed to retrieve device interface detail." << std::endl;
            }

            delete[] reinterpret_cast<char*>(deviceInterfaceDetail);
            deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            deviceIndex++;
        }

        // Clean up
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
        
        // Sleep for a certain duration before checking the USB devices again
        Sleep(5000);
    }
}

int main() {
    // Create a Daemon process to monitor the USB devices in the background
    HANDLE hThread = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MonitorUSBDevices), NULL, 0, NULL);
    if (hThread == NULL) {
        std::cout << "Failed to create monitoring thread." << std::endl;
        return 1;
    }

    // Run the main program in the background
    while (true) {
        // Sleep to prevent excessive CPU usage
        Sleep(1000);
    }

    // Wait for the monitoring thread to finish
    WaitForSingleObject(hThread, INFINITE);
    return 0;
}
