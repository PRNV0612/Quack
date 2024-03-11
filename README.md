# HID Device Detection for Rubber Ducky

This was part of a research i was working on on USB HIDs and the potential security concerns regarding the same.

This C program detects Human Interface Devices (HID) on Windows and checks if the device has a specific Vendor ID and Product ID, indicative of a "Rubber Ducky." The code uses the SetupAPI to enumerate HID devices and opens each device to check its attributes.

## Prerequisites

- Windows operating system
- Compiler capable of building C programs (e.g., Visual Studio, MinGW, etc.)

## Dependencies

- Windows API headers: windows.h, setupapi.h, devguid.h, regstr.h
- HID-specific header: hidsdi.h

## Usage

1. Clone the repository or download the source code.
2. Open the project in your preferred C compiler environment.
3. Ensure that the necessary dependencies are available in your compiler's include path.
4. Build and run the program.

## Acknowledgments

- Inspired by the need to detect specific HID devices such as "Rubber Ducky."
- Thanks to the Windows API for providing the necessary functions for device enumeration and interaction.

