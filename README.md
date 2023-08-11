# megasquirt_can_aux_boards
This repository contains code and documentation for expanding the functionality of a MegaSquirt Engine Control Unit (ECU) using microcontrollers. The programs were developed while modifying a 1988 Toyota MR2 but can be adapted for other vehicles as well.  

## Purpose
The project aims to enhance the capabilities of the MegaSquirt ECU by integrating two microcontrollers with specific purposes:  

1. First Microcontroller:  
    - The first microcontroller is responsible for collecting sensor readings from various sources, including:
        - Exhaust Gas Temperature (EGT) probes
        - Fuel pressure sensor
        - Accelerometer
    - These sensor readings are then transmitted to the MegaSquirt ECU using the Controller Area Network (CAN) bus. By providing additional sensor data, this microcontroller enables the ECU to make more informed decisions regarding engine performance and optimization.
    - This run on an Arduino Mega 2560 but could also be implement on a number of other boards.

1. Second Microcontroller
    - The second microcontroller focuses on controlling the on/off functionality of a supercharger. Its main tasks include:
        - Determining the control strategy mode based on a switch on the vehicle's dashboard
        - Receiving digital signals from the MegaSquirt ECU that signal different engine operating conditions
        - Sending a digital signal back to the ECU to indicate the current state of the supercharger (on or off)
    - By integrating this microcontroller, the ECU gains the ability to dynamically change the supercharger control strategy, optimizing engine performance based on real-time conditions and driver preferences.
    - This run on an Arduino Nano but could also be implement on a number of other boards.


## Repository Structure
The repository is organized into two different PlatformIO projects as follows:
- [CAN_sensors/](CAN_sensors/): contains files for microcontroller 1
    - [lib/](CAN_sensors/lib/): contains necessary libraries
    - [src/](CAN_sensors/src/): Main code for the first microcontroller
- [Supercharger_Controller/](Supercharger_Controller/): contains files for microcontroller 2
    - [src/](Supercharger_Controller/src/): Main code for the second microcontroller


<details>
<summary> Contributing</summary>  
Contributions to this project are welcome. If you find any issues or have suggestions for improvements, please open an issue or submit a pull request on the GitHub repository.
</details>

<details>
<summary> License </summary>
This project is licensed under the GNU General Public License. Feel free to use, modify, and distribute the code for personal or commercial purposes.
</details>

<details>
<summary>Acknowledgements</summary>
Special thanks to the developers of the libraries and frameworks used in this project for their contributions to the open-source community.
</details>