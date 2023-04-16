# Auto-Balancing-Platform-Project
This is the source files of an auto-balancing platform for my Final Year Project. Eventually, this project ended up in the negative result because of the limitation of L298N motor drivers and linear actuators. For some reasons, I used 4 linear actuators to make rotational motions in Roll and Pitch angles of the platform, but it would be better to use servo motors.

## Position Extraction
The position coordinates (x, y) were successfully extracted from the frames. Source code is in `FYP` directory and OpenCV for C++ should be installed externally.

## Serial Communication
This VS project includes the source files for serial communication, but only `sendCommand` in `SerialComm.cpp` works. `readCommand` can receive a character from Arduino using `Serial.print(char)` in ino file, but not string.

## Equipment
* 2 L298N motor drivers
* Arduino Mega 2560
* Logitech C920 Pro
* 4 LA-T8 Type G Linear Actuators (http://www.gomotorworld.com/pd?product_id=64)
* 3D Printed Components in STL
