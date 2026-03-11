#include <Servo.h>

const int LEFT_MOTOR_PIN1      = 5;
const int LEFT_MOTOR_PIN2      = 6;
const int LEFT_MOTOR_SPEED_PIN = 3;

const int RIGHT_MOTOR_PIN1      = 9;
const int RIGHT_MOTOR_PIN2      = 10;
const int RIGHT_MOTOR_SPEED_PIN = 4;

const int SERVO_TOP_PIN    = 7;
const int SERVO_BOTTOM_PIN = 8;

const int TRIG_PIN = 11;
const int ECHO_PIN = 12;

const int SPEED_LEVELS[]  = {0, 26, 51, 77, 102, 128, 153, 179, 204, 230, 255};
const int NUM_SPEED_LEVELS = 11;

Servo servoTop;
Servo servoBottom;

int currentSpeed  = 255;
int topAngle      = 90;
int bottomAngle   = 90;

void setup() {
  pinMode(LEFT_MOTOR_PIN1,      OUTPUT);
  pinMode(LEFT_MOTOR_PIN2,      OUTPUT);
  pinMode(LEFT_MOTOR_SPEED_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1,     OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2,     OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED_PIN,OUTPUT);
  pinMode(TRIG_PIN,             OUTPUT);
  pinMode(ECHO_PIN,             INPUT);

  servoTop.attach(SERVO_TOP_PIN);
  servoBottom.attach(SERVO_BOTTOM_PIN);
  servoTop.write(topAngle);
  servoBottom.write(bottomAngle);

  Serial.begin(9600);
  Serial.println("R2R Rescue Robot Ready.");
  Serial.println("Commands: w=forward, s=backward, a=left, d=right, 0=stop");
  Serial.println("         m=measure distance");
  Serial.println("         1-9=speed (10%-90%), f=full speed");
  Serial.println("         t <0-180>=top servo, b <0-180>=bottom servo");
}

void loop() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  if (cmd.length() == 0) return;

  if      (cmd == "w") { moveForward();  }
  else if (cmd == "s") { moveBackward(); }
  else if (cmd == "a") { turnLeft();     }
  else if (cmd == "d") { turnRight();    }
  else if (cmd == "0") { stopMotors();   }
  else if (cmd == "m") { measureDistance(); }
  else if (cmd == "f") { setSpeed(255); Serial.println("Speed: full"); }
  else if (cmd.length() == 1 && cmd[0] >= '1' && cmd[0] <= '9') {
    int level = cmd[0] - '0';
    setSpeed(SPEED_LEVELS[level]);
    Serial.print("Speed: ");
    Serial.print(level * 10);
    Serial.println("%");
  }
  else if (cmd.startsWith("t ")) { setServo(servoTop,    topAngle,    cmd, "Top");    }
  else if (cmd.startsWith("b ")) { setServo(servoBottom, bottomAngle, cmd, "Bottom"); }
  else {
    Serial.println("Unknown command.");
  }
}

void setSpeed(int speed) {
  currentSpeed = constrain(speed, 0, 255);
}

void setMotors(int leftDir1, int leftDir2, int rightDir1, int rightDir2) {
  analogWrite(LEFT_MOTOR_SPEED_PIN,  currentSpeed);
  analogWrite(RIGHT_MOTOR_SPEED_PIN, currentSpeed);
  digitalWrite(LEFT_MOTOR_PIN1,  leftDir1);
  digitalWrite(LEFT_MOTOR_PIN2,  leftDir2);
  digitalWrite(RIGHT_MOTOR_PIN1, rightDir1);
  digitalWrite(RIGHT_MOTOR_PIN2, rightDir2);
}

void moveForward()  { setMotors(HIGH, LOW,  HIGH, LOW);  Serial.println("Forward.");  }
void moveBackward() { setMotors(LOW,  HIGH, LOW,  HIGH); Serial.println("Backward."); }
void turnLeft()     { setMotors(LOW,  HIGH, HIGH, LOW);  Serial.println("Left.");     }
void turnRight()    { setMotors(HIGH, LOW,  LOW,  HIGH); Serial.println("Right.");    }

void stopMotors() {
  analogWrite(LEFT_MOTOR_SPEED_PIN,  0);
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 0);
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  Serial.println("Stopped.");
}

void measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    Serial.println("Distance: out of range");
    return;
  }

  float distance = duration * 0.0343 / 2.0;
  Serial.print("Distance: ");
  Serial.print(distance, 1);
  Serial.println(" cm");
}

void setServo(Servo &servo, int &angle, String cmd, const char *name) {
  int requested = cmd.substring(2).toInt();
  if (requested < 0 || requested > 180) {
    Serial.println("Angle must be 0-180.");
    return;
  }
  angle = requested;
  servo.write(angle);
  Serial.print(name);
  Serial.print(" servo: ");
  Serial.print(angle);
  Serial.println(" deg");
}
