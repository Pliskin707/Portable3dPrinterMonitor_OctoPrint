# Portable3dPrinterMonitor_OctoPrint
A small portable device displaying the status of the current 3D print job

## Demo Timelapse Video

[![TimelapseVideoYT](http://img.youtube.com/vi/iqBG1OuChHk/0.jpg)](http://www.youtube.com/watch?v=iqBG1OuChHk)

## Schematic
![Breadboard](/assets/OctoPrint_Monitor_bb.png)

# Installation

## Required Software

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

Hit `CTRL + S` to save the file. Make sure it is named `wifiauth2.h` and located in the same folder as the [platformio.ini](/platformio.ini).

## Build

*TODO*[^1]

## Flash

*TODO*

If you have more than one serial device connected to your computer, the flash tool might choose the wrong one and the flash process will fail. To solve this you can either unplug all other serial devices or check your device manager for the correct COM port and add it to the `initial_flash` environment of the [platformio.ini](/platformio.ini), i.e. like this:
```INI
[env:initial_flash]
upload_port = COM4
lib_deps = 
	${env.lib_deps}
build_flags = 
	${env.build_flags}
	-D DEBUG_PRINT
```


[^1]: The build process may fail the first 1-2 tries, during which Platform IO will download and install required libraries. Just try rebuilding a few times, eventually the build will succeed.
