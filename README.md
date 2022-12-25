# Portable3dPrinterMonitor_OctoPrint
A small portable device displaying the status of the current 3D print job

## Demo Timelapse Video

[![TimelapseVideoYT](http://img.youtube.com/vi/iqBG1OuChHk/0.jpg)](http://www.youtube.com/watch?v=iqBG1OuChHk)

# Installation

## Required Software

Octoprint is required for the monitor to retreive the print job status. You may choose to integrate octoprint to your slicer, i.e. for Cura there is [this plugin](https://marketplace.ultimaker.com/app/cura/plugins/fieldofview/OctoPrintPlugin).

PlatformIO is required to configure and flash the Wemos controller. It is not required during operation. 

### Octoprint

Download and install octoprint for your raspberry pi (see [install guide](https://octoprint.org/download/))

### Platform IO

* Download and install the [VS Code IDE](https://code.visualstudio.com/download)
* Install Platform IO for VSCode (see [install guide](https://platformio.org/install/ide?install=vscode)). 
* (opt) If you want to use github, I recommend installing the [GitLens extension](https://marketplace.visualstudio.com/items?itemName=eamodio.gitlens), though this is not required for the project
* Clone this Repo or download it as ZIP and extract it

# Prepare the µC

Open VS Code and select `File > Open Folder`. Select the project folder containing the [platformio.ini](/platformio.ini).

## Connectivity

Hit `CTRL + N` to create a new file. Paste the following content:
```C
#pragma once

const char* ssid = "<YourSSIDhere>";
const char* password = "<YourPasswordHere>";
const char* octoapi = "<YourOctoprintApiKey>";
```

Edit the values so they match your wifi network where your octoprint installation is connected. It is *recommended* to create a new API key for this monitor. To do so follow these instructions:
1. open the web interface of your OctoPrint Server
2. log in and open the settings (:wrench: symbol near the top right corner)
3. select the `Application Keys` topic and generate a new API key
4. copy the new API key
5. hit `Save`
6. paste the API key to the newly created file

The result should look something like this:
```C
#pragma once

const char* ssid = "MyPrintWifi";
const char* password = "L3ngthB34tsC0mp1exi7y";
const char* octoapi = "0123456789ABCDEF0123456789ABCDEF";
```

Hit `CTRL + S` to save the file. Make sure it is named `wifiauth2.h` and is located in the same folder as the [platformio.ini](/platformio.ini).

## Build

On the icon bar at the bottom of the IDE select the checkmark (it should display `PlatformIO: Build` on mouse-over) to start the build[^1] process. It should look something like this:

![image](https://user-images.githubusercontent.com/85575336/207655047-cb578531-9dff-48c4-b49b-46d382a37d0c.png)

## Flash

There are two ways to get the firmware to your device. The first way, flashing via USB cable, is required only once and enables the second: over-the-air updates. 

### Initial flash via USB cable

Connect your Wemos with your PC via usb cable. If required, install the driver (windows should be able to do this automatically).  
Select the `env:initial_flash` environment by clicking the environment icon (marked red). Wait for PlatformIO to finish all operations (the green part disappears).

![image](https://user-images.githubusercontent.com/85575336/209475650-969ee28f-d81f-423e-af4b-3b23f024a62e.png)

Select the :arrow_right: next to the checkmark symbol to start the upload.

If you have more than one serial device connected to your computer, the flash tool might choose the wrong one and the flash process will fail. To solve this you can either unplug all other serial devices or check your device manager for the correct COM port and add it to the `initial_flash` environment of the [platformio.ini](/platformio.ini), i.e. like this:
```INI
[env:initial_flash]
upload_port = COM4
lib_deps = 
	${env.lib_deps}
build_flags = 
	${env.build_flags}
```

### Updating via Wifi

:warning: The device *must* have been flashed via USB, otherwise the update will fail!

Power your device on and wait for it to connect to your configured wifi. Select the `env:over_the_air` environment and wait for PlatformIO to finish all operations. Press the :arrow_right: to start the update. The device will reboot after the update. 

## Hardware

The following components are required:
- Wemos D1
- SSD1306 OLED screen with a 128x64 pixels resolution (the firmware is optimized for the dual-color displays, but works perfectly fine with the monochrome displays)
- 4x wire
- a USB cable to connect the Wemos to your PC (either micro USB or USB-C, depending on your Wemos)

Solder the display to the Wemos or connect both parts with a bread board as shown below.

:warning: Some displays have different pinouts than the one shown in the picture.

![Breadboard](/assets/OctoPrint_Monitor_bb.png)
![Hardware1](/assets/20221225_OctoPrintMonitor_0001.jpg)
![Hardware2](/assets/20221225_OctoPrintMonitor_0002.jpg)

### Case

There is a simple slide-in case available:

[f3d (Fusion360)](/assets/OctoCase.f3d)  
[3mf (3D manufacturing format)](/assets/OctoCase.3mf)

The following print settings are *recommended*:
- Nozzle: 0.4 mm
- Layer heigth: 0.08 mm
- Orientation: standing on the side so the largest opening faces upwards
- Supports: only within the "window" for the OLED screen

![CaseEmpty](/assets/20221225_OctoPrintMonitor_0003.jpg)
![CaseFull](/assets/20221225_OctoPrintMonitor_0004.jpg)

[^1]: The build process may fail the first 1-2 tries, during which Platform IO will download and install required libraries. Just try rebuilding a few times, eventually the build will succeed.
