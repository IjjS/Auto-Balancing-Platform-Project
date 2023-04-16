# Auto-Balancing-Platform-Project
This is the source files of an auto-balancing platform for my Final Year Project. Eventually, this project ended up in the negative result because of the limitation of L298N motor drivers and linear actuators. For some reasons, I used 4 linear actuators to make rotational motions in Roll and Pitch angles of the platform, but it would be better to use servo motors.
## Position Extraction
The position coordinates (x, y) were successfully extracted from the frames. OpenCV for C++ should be installed externally.
## Serial Communication
This VS project includes the source files for serial communication, but only `sendCommand` in `SerialComm.cpp` works. `readCommand` can receive a character from Arduino using `Serial.print(char)` in ino file, but not string.
