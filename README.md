# Octoprint-PanTilt-Nano-Firmware

Arduino Nano firmware to be used with the [OctoPrint-PanTilt-Nano](https://github.com/c-devine/OctoPrint-PanTilt-Nano) project.

## Snapshots

<img src="https://raw.githubusercontent.com/c-devine/OctoPrint-PanTilt-Nano-Firmware/snapshots/assets/img/webcam.png?raw=true" width="320" height="240">
<img src="https://raw.githubusercontent.com/c-devine/OctoPrint-PanTilt-Nano-Firmware/snapshots/assets/img/nano.png?raw=true" width="320" height="240">
<img src="https://raw.githubusercontent.com/c-devine/OctoPrint-PanTilt-Nano-Firmware/snapshots/assets/img/harness.png?raw=true" width="320" height="240">
<img src="https://raw.githubusercontent.com/c-devine/OctoPrint-PanTilt-Nano-Firmware/snapshots/assets/img/case.png?raw=true" width="320" height="240">


## Setup

avrdude -patmega328p -carduino -PCOMX -b57600 -D -Uflash:w:PanTilt-Nano-1.0.0.hex:i 


