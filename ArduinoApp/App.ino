#define ENXA 8 //m1
#define ENXB 5 //m4
#define ENYA 3 //m3
#define ENYB 2 //m2

#define INX1 52 //m1
#define INX2 53
#define INX3 44 //m4
#define INX4 45

#define INY1 28 //m3
#define INY2 29
#define INY3 22 //m2
#define INY4 23

int x = 0;
int y = 0;
int error_x = 0;
int error_y = 0;

long integral_x = 0;
long integral_y = 0;

int deriv_x = 0;
int deriv_y = 0;

const float Kp = 0.8;
const float Ki = 0.01;
const float Kd = 30;
const float Ka = 0.5;

unsigned long last_ms = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.setTimeout(100);
  Serial.begin(115200);
  pinMode(ENXA, OUTPUT);
  pinMode(ENXB, OUTPUT);
  pinMode(ENYA, OUTPUT);
  pinMode(ENYB, OUTPUT);
  pinMode(INX1, OUTPUT);
  pinMode(INX2, OUTPUT);
  pinMode(INX3, OUTPUT);
  pinMode(INX4, OUTPUT);
  pinMode(INY1, OUTPUT);
  pinMode(INY2, OUTPUT);
  pinMode(INY3, OUTPUT);
  pinMode(INY4, OUTPUT);
  analogWrite(ENXA, 160);
  analogWrite(ENXB, 160);
  analogWrite(ENYA, 160);
  analogWrite(ENYB, 160);

  turn_on_all_acts();
  drop_all_acts();
  delay(1400);
  stop_all_acts();
  
  calibrate_ball_position();
}

void set_pitch_direction (int position_x) {
  if (position_x > 0) {
    digitalWrite(INX1, HIGH);
    digitalWrite(INX2, LOW);
    digitalWrite(INX3, LOW);
    digitalWrite(INX4, HIGH);
  } else if (position_x < 0) {
    digitalWrite(INX1, LOW);
    digitalWrite(INX2, HIGH);
    digitalWrite(INX3, HIGH);
    digitalWrite(INX4, LOW);
  } else {
    digitalWrite(INX1, LOW);
    digitalWrite(INX2, LOW);
    digitalWrite(INX3, LOW);
    digitalWrite(INX4, LOW);
  }
}

void set_roll_direction (int position_y) {
  if (position_y > 0) {
    digitalWrite(INY1, HIGH);
    digitalWrite(INY2, LOW);
    digitalWrite(INY3, LOW);
    digitalWrite(INY4, HIGH);
  } else if (position_y < 0) {
    digitalWrite(INY1, LOW);
    digitalWrite(INY2, HIGH);
    digitalWrite(INY3, HIGH);
    digitalWrite(INY4, LOW);
  } else {
    digitalWrite(INY1, LOW);
    digitalWrite(INY2, LOW);
    digitalWrite(INY3, LOW);
    digitalWrite(INY4, LOW);
  }
}

void stop_all_acts () {
  set_pitch_direction(0);
  set_roll_direction(0);
}

void rotate_pitch (int velocity) {
  analogWrite(ENXA, velocity);
  analogWrite(ENXB, velocity);
}

void rotate_roll (int velocity) {
  analogWrite(ENYA, velocity);
  analogWrite(ENYB, velocity);
}

void turn_on_all_acts () {
  analogWrite(ENXA, 255);
  analogWrite(ENXB, 255);
  analogWrite(ENYA, 255);
  analogWrite(ENYB, 255);
}

void raise_all_acts () {
  digitalWrite(INX1, HIGH);
  digitalWrite(INX2, LOW);
  digitalWrite(INX3, HIGH);
  digitalWrite(INX4, LOW);
  digitalWrite(INY1, HIGH);
  digitalWrite(INY2, LOW);
  digitalWrite(INY3, HIGH);
  digitalWrite(INY4, LOW);
}

void drop_all_acts () {
  digitalWrite(INX1, LOW);
  digitalWrite(INX2, HIGH);
  digitalWrite(INX3, LOW);
  digitalWrite(INX4, HIGH);
  digitalWrite(INY1, LOW);
  digitalWrite(INY2, HIGH);
  digitalWrite(INY3, LOW);
  digitalWrite(INY4, HIGH);
}

void set_acts_positions_initial() {
  drop_all_acts();
  turn_on_all_acts();
  delay(1400);

  raise_all_acts();
  delay(600);
  stop_all_acts();
}


bool get_ball_position () {
  bool has_ball = false;
  String buffer;
  char store[5];
  int store_index = 0;
  {
    buffer = Serial.readStringUntil('\n');

    for(int i=0; i<buffer.length(); i++) {
      store[store_index] = buffer[i];
      if (buffer[i] == 'x') {
        store[store_index] = 0;
        x = atoi(store);
        store_index = 0;
      } else if (buffer[i] == 'y') {
        store[store_index] = 0;
        y = atoi(store);
        has_ball = true;
        break;
      } else {
        store_index++;
      } 
    }
  }

  return has_ball;
}

void calibrate_ball_position() {
  int loop = 0;
  bool has_ball = false;
  set_acts_positions_initial();
  /*while (1) {
    if (Serial.available()) {
      has_ball = get_ball_position();
      if (has_ball) {
        set_pitch_direction(x);
        set_roll_direction(y);
        rotate_pitch(255);
        rotate_roll(255);
        loop++;
      } else {
        stop_all_acts();
      }

      if (loop == 100) {
        break;
      } 
    }
  }

  turn_on_all_acts();
  drop_all_acts();
  delay(1400);

  x = 0;
  y = 0;

  set_acts_positions_initial();*/
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    
    unsigned long current_ms = millis();
    unsigned long dt = current_ms - last_ms;
    if (dt <= 0) {
      dt = 1;
    }
    
    get_ball_position();
    integral_x += x;
    integral_y += y;

    deriv_x = (x - error_x)/dt;
    deriv_y = (y - error_y)/dt;

    error_x = x;
    error_y = y;


    int pid_x = Kp * x + Kd * deriv_x;
    int pid_y = Kp * y * deriv_y;

    if (pid_x < 0) {
      pid_x *= -1;
    }

    if (pid_y < 0) {
      pid_y *= -1;
    }

    if (pid_x > 255) {
      pid_x = 255;
    }

    if (pid_y > 255) {
      pid_y = 255;
    }
    set_pitch_direction(x);
    set_roll_direction(y);
    rotate_pitch(pid_x);
    rotate_roll(pid_y);
    
    last_ms = current_ms;
  }
}
