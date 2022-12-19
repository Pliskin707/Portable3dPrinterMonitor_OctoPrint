# Portable3dPrinterMonitor_OctoPrint
A small portable device displaying the status of the current 3D print job

## Demo Timelapse Video

[![TimelapseVideoYT](http://img.youtube.com/vi/iqBG1OuChHk/0.jpg)](http://www.youtube.com/watch?v=iqBG1OuChHk)

## Schematic
![Breadboard](/assets/OctoPrint_Monitor_bb.png)

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

Edit the values so they match your wifi network where your octoprint installation is connected. For example:
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

*TODO*

### Initial flash via USB cable

*TODO*

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

*TODO*

[^1]: The build process may fail the first 1-2 tries, during which Platform IO will download and install required libraries. Just try rebuilding a few times, eventually the build will succeed.
