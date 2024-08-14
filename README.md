# STM32WBA-BLE-Weight-Scale

* The STM32WBA-BLE-Weight-Scale demonstrating Bluetooth® SIG [Weight Scale Profile 1.0](https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=293525) example, based on STM32CubeWBA v1.3.1
 
## Hardware Needed

  * This example runs on STM32WBAxx devices.
  * Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK). 

## User's Guide

1) Run this Bluetooth® Low Energy Weight Scale project where the STM32WBA will advertise

2) Use the ANDROID/IOS ST BLE Sensor application or ST BLE Toolbox application to connect with this Bluetooth® Low Energy [Weight Scale Profile 1.0](https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=293525) (WSS_XX where XX is the 2 last digit of the Bluetooth® Device Address)
  
   Scan on ANDROID/IOS:
   <div align="center">
       <img src="Utilities/Media/WSS_Toolbox.jpg" alt="image" width="300" height="auto">
   </div>

   After Connection and Selecting the [Weight Scale Service 1.0](https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=293526) on ANDROID/IOS you will see the following Characteristics:
   <div align="center">
       <img src="Utilities/Media/WSS_Toolbox1.jpg" alt="image" width="300" height="auto">
       <img src="Utilities/Media/WSS_Toolbox2.jpg" alt="image" width="300" height="auto">
     </div>
	
3) Use terminal programs like Tera Term to see the logs of each board via the onboard ST-Link. (115200/8/1/n)

   <div align="center">
       <img src="Utilities/Media/WSS_terminal_log.jpg" alt="image" width="1000" height="auto">
   </div>


## Troubleshooting

**Caution** : Issues and the pull-requests are **not supported** to submit problems or suggestions related to the software delivered in this repository. The STM32WBA-BLE-Weight-Scale example is being delivered as-is, and not necessarily supported by ST.

**For any other question** related to the product, the hardware performance or characteristics, the tools, the environment, you can submit it to the **ST Community** on the STM32 MCUs related [page](https://community.st.com/s/topic/0TO0X000000BSqSWAW/stm32-mcus).