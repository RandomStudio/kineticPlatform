<img width="989" alt="Wiring" src="https://github.com/RandomStudio/kineticPlatform/blob/main/Ressources/2.png">

# Kinetic Platform
Kinetic Platform is a modular infrastructure for handling rotational motion of objects. 
Developed for the living lab, Random Studio, in collaboration with Thomas Kaufmanas in fall 2024.

## Contents
- kineticPlatformFinal.toe: Annotated touchdesigner file.
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

**Microcontroller (MCU slave)**
Runs a simple script that moves the stepper motor according to a position (float degrees), receieved over serial. Uses the accelstepper library, meaning it automatically calculates speed and acceleration relative to the distance it has to travel. These can be reconfigured as needed. Serial was favored for data-speed.

**AS5600**
Is available to mount beneath the platform with space for a magnet on the driving axis. As accelstepper keeps track of steps in relative position, this is not needed as long as the platform is manually zeroed before powering the MCU.

**Touchdesigner**
Serves as the brain of the current setup. It takes input from Lidar sensors via Tether, and triangulates spatial data relative to users and platforms (angle). Also contains examples and sketches from specific experiments with the Kinetic Platform.




<img width="989" alt="Wiring" src="https://github.com/RandomStudio/kineticPlatform/blob/main/Ressources/Wiring%20ESP.png">
<img width="989" alt="Wiring" src="https://github.com/RandomStudio/kineticPlatform/blob/main/Ressources/Base%20version%20data%20flow.png">



*Useful Links and further documentation:*

https://www.tinytronics.nl/en/mechanics-and-actuators/motors/stepper-motors/stepper-motor-1.8n.m-2.8a-nema23
https://www.tinytronics.nl/en/mechanics-and-actuators/motor-controllers-and-drivers/stepper-motor-controllers-and-drivers/tmc2209-motor-driver-module-v4.0
https://www.analog.com/media/en/technical-documentation/data-sheets/TMC2209_datasheet_rev1.09.pdf
https://wiki.fysetc.com/Silent2209/

