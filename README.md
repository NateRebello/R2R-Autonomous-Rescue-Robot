# R2R-Autonomous-Rescue-Robot
Embedded control system for a small-form-factor rescue robot built for the UNSW DESN1000 Robots to the Rescue design challenge. The robot navigates a maze, traverses rough terrain, detects objects via ultrasonic sensing, and retrieves a target victim object; all under operator control via a serial interface.


## Hardware

| Component | Detail |
|---|---|
| Microcontroller | Arduino Uno |
| Drive motors | 2× DC motors (differential drive) |
| Actuation | 2× servo motors (gripper arm — top/bottom axis) |
| Sensing | HC-SR04 ultrasonic sensor |
| Interface | USB serial (9600 baud) |
| Power | On-board battery pack |
| Form factor | ≤ 250mm diameter × 250mm height |
| Build cost | < AUD $130 |

### Pin Map

| Pin | Function |
|---|---|
| 3 | Left motor PWM speed |
| 4 | Right motor PWM speed |
| 5 | Left motor direction 1 |
| 6 | Left motor direction 2 |
| 7 | Top servo signal |
| 8 | Bottom servo signal |
| 9 | Right motor direction 1 |
| 10 | Right motor direction 2 |
| 11 | Ultrasonic TRIG |
| 12 | Ultrasonic ECHO |

---

## Software Architecture

```
r2r_robot.ino
├── setup()           – pin init, servo home positions, serial init
├── loop()            – serial command parser (non-blocking)
├── setMotors()       – unified motor driver (direction + current speed)
├── moveForward()     – differential drive forward
├── moveBackward()    – differential drive reverse
├── turnLeft()        – left wheel reverse, right wheel forward (point turn)
├── turnRight()       – right wheel reverse, left wheel forward (point turn)
├── stopMotors()      – coast stop, all PWM to 0
├── measureDistance() – HC-SR04 pulse timing, timeout-safe
└── setServo()        – validated servo write with bounds check
```

The control loop is **non-blocking** — `Serial.readStringUntil()` only executes when bytes are available, keeping the loop responsive. Ultrasonic reads use a 30ms `pulseIn` timeout to avoid hanging on out-of-range measurements.

---

## Serial Command Reference

Open Serial Monitor at **9600 baud, newline line ending**.

| Command | Action |
|---|---|
| `w` | Move forward |
| `s` | Move backward |
| `a` | Turn left (point turn) |
| `d` | Turn right (point turn) |
| `0` | Stop all motors |
| `m` | Measure and report distance (cm) |
| `1`–`9` | Set speed to 10%–90% duty cycle |
| `f` | Full speed (100%) |
| `t <angle>` | Set top servo to angle (0–180°) |
| `b <angle>` | Set bottom servo to angle (0–180°) |

**Example session:**
```
> f
Speed: full
> w
Forward.
> m
Distance: 18.4 cm
> 0
Stopped.
> t 45
Top servo: 45 deg
> b 120
Bottom servo: 120 deg
```

---

## Key Design Decisions

**Differential drive (point turns):** Left and right motors are independently reversible. Turning commands spin wheels in opposite directions, enabling zero-radius turns — essential for navigating 420mm maze cells.

**Speed abstraction via `setMotors()`:** All movement functions delegate to a single `setMotors()` helper, so speed changes propagate uniformly without duplicated `analogWrite` calls across every movement function.

**Ultrasonic timeout guard:** `pulseIn(ECHO_PIN, HIGH, 30000)` returns 0 if no echo is received within 30ms (~5m range). The code catches this and prints `"out of range"` rather than reporting a garbage distance — important for reliable blind navigation.

**Servo bounds validation:** `setServo()` enforces 0–180° limits and passes the servo object by reference, eliminating duplicated validation logic for top and bottom servos.

---

## Challenge Specifications

| Constraint | Value |
|---|---|
| Max diameter (fully extended) | 250mm |
| Max weight (inc. batteries) | 1000g |
| Max build cost | AUD $130 |
| Maze cell size | 420mm × 420mm |
| Maze wall height | 300mm |
| Rough terrain step range | 6mm – 30mm |
| Microcontroller | Arduino Uno |
| Interface | USB serial / optional Bluetooth |

---

## Repo Structure

```
r2r-rescue-robot/
├── src/
│   └── r2r_robot.ino      # Main Arduino sketch
├── docs/
│   └── design_notes.md    # Design decisions and hardware notes
├── media/                 # Photos and demo videos
└── README.md
```

---

## Setup

1. Open `src/r2r_robot.ino` in the [Arduino IDE](https://www.arduino.cc/en/software)
2. Select board: **Arduino Uno**
3. Select the correct COM port
4. Upload to board
5. Open Serial Monitor at **9600 baud** with **Newline** line ending
