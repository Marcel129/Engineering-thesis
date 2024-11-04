# Cart - engineering thesis

### About
The cart design was developed as part of an engineering thesis for the Robocik Scientific Club at Wrocław University of Science and Technology. It is intended to transport a robot that the organization is developing. The project includes the design, construction, and programming of a remote-controlled transport platform. The vehicle model was created in CAD software. The overall structure of the vehicle was defined, along with the platform’s drive system and the method for transmitting power to the axles. Essential sensors and electronic modules required for the robot's proper functioning were selected, and their connections and power supply were planned. Finally, a control application was developed, and the completed vehicle was tested.

Design Requirements:
- Minimum load capacity: 30 kg
- Minimum operating time on a single charge: 0.5 hours
- Maximum speed: 5 km/h
- Wireless control
- Ability to perform an immediate emergency stop

### Skills Acquired:

- 3D modeling in SolidWorks
- Embedded systems design, including sensor, actuator, and driver selection
- Low-level programming, with expertise in UART, ADC, PWM, I2C, and timers
- GUI application development using the Qt library

FOTO

### Mechanical design
The 3D model of the vehicle can be viewed by downloading the Cart.html file from the "Resources" folder and opening it in a web browser. CAD model was prepared with SolidWorks software.

The platform, measuring 70x50x35 cm, is equipped with two DC motors with planetary gearboxes to drive the wheels and a DC motor with a worm gear for steering the front axle. Power is transmitted to the wheels via a belt drive. The load-bearing structure is made of chipboard. Mounting components were 3D printed, while rotating parts (such as wheel axles) were machined from aluminum.

The cart underwent load capacity testing, during which it was found to operate effectively with a load of up to 75 kg.
FOTO

### Electrical design
##### Power board:
The power board is responsible for preparing the voltage supply for both the logic and operational hardware components. The logic section is powered by a "small" 2S 18650 cell pack, while the operational section uses a larger 4S3P pack. The "large" pack provides 12V for the steering motor and the full pack voltage ~16,8V for the drive motors.

Power Circuit Protections:

- Reverse polarity protection for the logic section power supply (using a bridge rectifier).
- Overcurrent protection (fuse) for both high and low voltage circuits.

##### Control Board:
This board serves as a shield for the STM32F411 Nucleo development board. It manages data reading from sensors, controls the motors, and handles communication with the main control computer and the control box. Communication with the main computer is done through the HC-05 Bluetooth module.

Sensors on the cart:

- Incremental encoders: Measure the rotational speed of the drive motors.
- Absolute encoder AS5600: Measures the rotation angle of the steering axle.
- Current sensors: Measure the current drawn by the drive motors.
- Voltage dividers: Measure the supply voltages for the logic and operational sections.

PHOTO
PHOTO
PHOTO
PHOTO
PHOTO
### Control application
The control application was developed using the Qt library.
FOTO