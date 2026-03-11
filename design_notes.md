# Design Notes

## Motor Control

Differential drive was chosen over Ackermann steering for zero-radius turning capability inside 420mm maze cells. Point turns (opposite wheel directions) allow the robot to rotate on its own axis, which is necessary when the robot needs to reverse direction within a single cell.

PWM speed control uses Arduino `analogWrite()` on pins 3 and 4 (both hardware PWM-capable on the Uno). Speed is stored as a module-level variable (`currentSpeed`) so all movement functions inherit the last set speed without needing it passed as a parameter.

## Ultrasonic Sensing

The HC-SR04 operates by timing a reflected 40kHz pulse. A 10µs HIGH pulse on TRIG causes the sensor to emit 8 ultrasonic bursts; ECHO goes HIGH for the duration of the return pulse. Distance = (pulse duration × speed of sound) / 2.

Speed of sound used: 0.0343 cm/µs (at ~20°C). A 30ms timeout on `pulseIn` caps the measurable range at approximately 5 metres and prevents the loop from hanging when no object is detected.

## Servo Arm

Two servo motors control the gripper arm on two axes:
- **Top servo:** rotational scan (used for victim detection sweep)
- **Bottom servo:** vertical actuation (lower to ground for victim pickup, raise to transport)

Both servos are validated to 0–180° before writing. The Arduino `Servo` library handles PWM signal generation (1000–2000µs pulse width) independently of the main motor PWM.

## Serial Interface

Commands are read as full strings terminated by newline (`\n`). `readStringUntil('\n')` blocks only until the newline arrives, keeping latency low for single-character movement commands. `cmd.trim()` strips carriage returns from Windows Serial Monitor clients that send `\r\n`.

## Known Constraints

- The Arduino Uno has 6 hardware PWM pins (3, 5, 6, 9, 10, 11). The original design had a pin conflict between the right motor speed (pin 11) and the ultrasonic TRIG (pin 11). This was resolved by moving right motor PWM to pin 4, which uses software PWM via the `analogWrite()` timer — sufficient for DC motor speed control but not suitable for servo signals.
- `turnLeft()` and `turnRight()` implement point turns (zero-radius). Wider arcs would require asymmetric PWM values between wheels — not implemented as the maze geometry made point turns preferable.
