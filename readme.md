# Kinetic Platform
Kinetic Platform is a modular infrastructure for handling rotational motion of objects. Developed for the living lab @ Random Studio in fall 2024.

## Contents
- kineticPlatformFinal.toe: Annotated touchdesigner file handling multiple users (from lidar via tether), and controlling multiple platforms. Also contains examples and sketches from specific experiments with the Kinetic Platform.
- PlatformSoftwareMCU: The final microcontroller code running on the platforms. One version spins continuously, where the second version doesn't cross 0° for setups with cables.
- Examples: Resetting with encoder, Bare minimum running a motor, receive tether on MCU
- Archive: Leftover sketches from testing different motor drivers.
- Ressources: Technical drawings, schematics, photos.


## How to start it
- Turn on the power supply and set to 24V 1.2-4amps (each platform draws at most 1.2A)
- Launch tether broker (docker), and Lidar scripts (lidar, 2dconsolidation, tracking wiz)
- Open touchdesigner and connect controllers via the serial node (might need to readjust port, and switch on/off)
- Adjust bias and user-select as needed.

## Notes on assembled parts and wiring
**The Physical Platform**
Is a simple rotational plate supported by ball bearings. The assembly is modular to allow change of gearing ratio - current setup is a Gear Ratio of 6 (120/20 teeth). Smaller gear will yield less higher speed with less torque, and vice versa.

**The Motor and Driver**
Is a Nema23 1.8A motor, paired with the Trinamic TMC2209 Driver. As per standard configuration the motor runs 1/8th microstepping in stealthchop mode - meaning its quiet and precice in operation. This can be configured from the microcontroller as needed. The driver is set to Vref of 1.6 to avoid running to warm. With higher loads on the platform, readjustment (or cooling) might be needed.

**Microcontroller (Arduino slave)**
Runs a simple script that moves the stepper motor according to a position (float degrees), receieved over serial. Later versions are based on accelstepper library, meaning it automatically calculates speed and acceleration relative to the distance it has to travel.

**AS5600**
Is an absolute encoder (magnetic) which helps the slave to figure out where the platform is. This is needed as accelstepper operates on relative position, and as such the encoder serves for the platform to "zero" itself on startup with absolute position, or potentially as needed in operation.

**Touchdesigner**
Serves as the brain of the current setup. It takes input from Lidar sensors via Tether, and triangulates spatial data relative to platform (angle and distance). This is forwarded to the arduino, currently via serial


<img width="989" alt="Wiring" src="https://github.com/user-attachments/assets/fc3165e6-2d4f-4251-80be-4736ecee467c">


*Useful Links and further documentation:*

https://www.tinytronics.nl/en/mechanics-and-actuators/motors/stepper-motors/stepper-motor-1.8n.m-2.8a-nema23
https://www.tinytronics.nl/en/mechanics-and-actuators/motor-controllers-and-drivers/stepper-motor-controllers-and-drivers/tmc2209-motor-driver-module-v4.0
https://www.analog.com/media/en/technical-documentation/data-sheets/TMC2209_datasheet_rev1.09.pdf
https://wiki.fysetc.com/Silent2209/

