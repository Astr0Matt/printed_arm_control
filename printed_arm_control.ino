// Test utilisation servomoteur SG90 //
#include <SoftwareSerial.h>
#include <Servo.h>
#define CTRL_BASE  A0
#define CTRL_LEFT  A1
#define CTRL_RIGHT  A2

#define PIN_BASE  9
#define PIN_LEFT  10
#define PIN_RIGHT 11
#define DEBUG_OPT 0
// fil marron => masse
// fil rouge => vdd
// fil orange => pwm
Servo servo_base;  // crÃ©e lâ€™objet pour contrÃ´ler le servomoteur
Servo servo_left;
Servo servo_right;

int bluetoothTx = 3;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 2;  // RX-I pin of bluetooth mate, Arduino D3
//PORT COM 12
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

int pince_close = 50; 
int pince_open = 140; 

int led =13;
char dataFromBt;
int start_ms = 0;
int end_ms = 0;
int state = 0;

int pos_base = 0, pos_left, pos_right, pos_hand;    // variable to store the servo position 
int val_base, val_left, val_right, val_hand; // variable to store potentiometer 

int previous_pos_base = 0;
void setup(){
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  
//#if DEBUG_OPT
  Serial.begin(9600);
  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600
//#endif

  servo_base.attach(PIN_BASE);  // utilise la broche 9 pour le contrÃ´le du servomoteur
  servo_left.attach(PIN_LEFT);
  servo_right.attach(PIN_RIGHT);
  start_ms = millis();
  servo_right.write(100);
  servo_left.write(45);
  move_down();
}
 
void close_finger() {
    servo_base.write(50);
    state = 0;
}


void open_finger() {
    servo_base.write(140);
    state = 1;
}

void hand() {
  end_ms = millis();
  if (end_ms - start_ms >= 1000) {
    start_ms = millis();
    if(state)
      close_finger();
    else
      open_finger();
  }
}

void move_down() {
    servo_base.write(10);
    state = 0;
}


void move_up() {
    servo_base.write(140);
    state = 1;
}


void arm() {
  previous_pos_base = pos_base;
  end_ms = millis();
  if (end_ms - start_ms >= 100) {
    start_ms = millis();
    val_base = analogRead(CTRL_BASE);      
    pos_base = map(val_base, 0, 1023, 0, 179); 
    servo_base.write(93+(pos_base-previous_pos_base));              // tell servo to go to position in variable 'pos' 
    Serial.println(pos_base);
  }
}

int deg[3] = {0, 0, 0};
int rotation = 0;
int poto_mode = 1;
int cpt = 0;
int down, up, left, right;
void loop() { 
  previous_pos_base = pos_base;
  end_ms = millis();
  
  if (end_ms - start_ms >= 100 && poto_mode == 1) {
    start_ms = millis();          

    pinMode(A0, INPUT);
    val_base = analogRead(A0);
    pos_base = map(val_base, 0, 1023, 0, 179); 
    servo_base.write(pos_base);    

    pinMode(A1, INPUT);
    val_left = analogRead(A1);      
    pos_left = map(val_left, 0, 1023, 0, 179); 
    servo_left.write(pos_left); 
    
    pinMode(A2, INPUT);
    val_right = analogRead(A2);      
    pos_right = map(val_right, 0, 1023, 0, 179); 
    servo_right.write(pos_right); 
    
    pinMode(A3, INPUT);
    val_hand = analogRead(A3);      
    pos_hand = map(val_hand, 0, 1023, 0, 179);

    
#if DEBUG_OPT        
    Serial.print("A0 :");
    Serial.println(pos_base);
      
    Serial.print("A1 :");
    Serial.println(pos_left);

    Serial.print("A2 :");
    Serial.println(pos_right);
      
    Serial.print("A3 :");
    Serial.println(pos_hand);
#endif    
  }
  if(bluetooth.available()) {
    dataFromBt = (byte) bluetooth.read();
    servo_base.write((int)dataFromBt); 
    Serial.println((byte)dataFromBt);
    bluetooth.print((int)dataFromBt);
    byte temp = dataFromBt;

  }  
} 






/*
 * if(dataFromBt == 0x20) {
      switch (cpt) {
        case 2: rotation = deg[0]; break;
        case 3: rotation = deg[0]*10 + deg[1]; break;
        case 4: rotation = deg[0]*100 + deg[1]*10 + deg[2]; break;
        default: rotation = 0;
      }
      Serial.print("Move to ");
      Serial.print(rotation);
      Serial.print("°");
      
      if (down) {
        servo_base.write(rotation); 
      } else if(up) {
        servo_right.write(rotation); 
      } else if(left) {
        servo_left.write(rotation); 
      } else if(right) {
        servo_right.write(rotation); 
      }
      
      cpt = 0;
      Serial.println();
    } else if (dataFromBt == 0x70) {
      Serial.println("Poto mode enabled");
      poto_mode = 1;
      cpt = 0;
    } else if (dataFromBt == 0x62) {
      Serial.println("Poto mode disabled");
      poto_mode = 0;
      cpt = 0;
    } else {
      
      switch (cpt) {
        case 0: 
          cpt = 1;
          switch (dataFromBt) {
            case 0x64 : 
              down = 1;
              up   = 0;
              left = 0;
              right = 0;
            break;
            case 0x75 : 
              down = 0;
              up   = 1;
              left = 0;
              right = 0;
            break;
            case 0x6C : 
              down = 0;
              up   = 0;
              left = 1;
              right = 0;
            break;
            case 0x72 : 
              down = 0;
              up   = 0;
              left = 0;
              right = 1;
            break;
            default: cpt = 0;
          }
        break;
        case 1: 
          cpt = 2;
          deg[0] = ((int)dataFromBt-48);
        break;
        case 2: 
          deg[1] = ((int)dataFromBt-48);
          cpt = 3;
        break;
        case 3: 
          deg[2] = ((int)dataFromBt-48);
          cpt = 4;  
        break;
      }
    }
 * 
 * 
 * /
 */
