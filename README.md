# Auto-Balancing-Platform-Project
This is the source files of an auto-balancing platform for my Final Year Project. Eventually, this project ended up in the negative result because of the limitation of L298N motor drivers and linear actuators. For some reasons, I used 4 linear actuators to make rotational motions in Roll and Pitch angles of the platform, but it would be better to use servo motors.

* Coordinate System of My Case
![image](https://user-images.githubusercontent.com/110002292/233013223-252ed1dd-552b-4750-a155-9413dd28ac22.png)

* Block Diagram
![image](https://user-images.githubusercontent.com/110002292/233013729-c4cd19a5-1a20-4a59-9e43-c5f9a45b7dec.png)

## Position Extraction
The position coordinates (x, y) were successfully extracted from the frames. Source code is in `FYP` directory and OpenCV for C++ should be installed externally.
![find contours and draw circle](https://user-images.githubusercontent.com/110002292/233011908-95575c74-7f10-49b8-8bfa-3640488c9a81.PNG)

## Serial Communication
This VS project includes the source files for serial communication, but only `sendCommand` in `SerialComm.cpp` works. `readCommand` can receive a character from Arduino using `Serial.print(char)` in ino file, but not string.

## Equipment
* 2 L298N motor drivers
* Arduino Mega 2560
* Logitech C920 Pro
* 4 LA-T8 Type G Linear Actuators (http://www.gomotorworld.com/pd?product_id=64)
* 3D Printed Components in STL
* 12V Battery
