#define LED_R 8
#define LED_G 10
#define LED_B 11
#define LDR A1
#define IN1 7
#define IN2 6
#define IN3 5
#define IN4 4
#define SNSRL 3
#define SNSRR 2

bool state = HIGH;

void H_bridge_handle(int value){
  switch(value){
    case 0: 
      //Parado
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,LOW);
      digitalWrite(IN3,LOW);
      digitalWrite(IN4,LOW);
      break;
    case 1:
      //Frente
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      digitalWrite(IN3,LOW);
      digitalWrite(IN4,HIGH);
      break;
    case 2:
      //Esquerda
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,LOW);
      digitalWrite(IN3,LOW);
      digitalWrite(IN4,HIGH);
      break;
    case 3:
      //Direita
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      digitalWrite(IN3,LOW);
      digitalWrite(IN4,LOW);
      break;
    case 4:
      //Trás
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH);
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      break;
  }
}

void Line_sensor_handle(){
  int handle = 0;
  if((digitalRead(SNSRL) == LOW)&&(digitalRead(SNSRR) == LOW)){
    //Condição de parado
    handle= 0;
  }
  else if ((digitalRead(SNSRL) == HIGH)&&(digitalRead(SNSRR) == HIGH)){
    //Condição de ir para frente
    handle = 1;
  }
  else if ((digitalRead(SNSRL) == LOW)&&(digitalRead(SNSRR) == HIGH)){
    //Condição de ir para a esquerda
    handle = 2;
  }
  else if ((digitalRead(SNSRL) == HIGH)&&(digitalRead(SNSRR) == LOW)){
    //Condição de ir para a direita
    handle = 3;
  }


//   H_bridge_handle(handle);
// }

void LED_select(){
    //Array para guardar o valor do LDR
    int LEDvalues[4] = {0,0,0,0}; //Treshold, LED_R, LED_G, LED_B
    
    //Ler a tensão basal
    LEDvalues[0] = analogRead(LDR);
    
    //Leitura do LED Vermelho
    digitalWrite(LED_R,HIGH);
    delay(10);
    LEDvalues[1] = analogRead(LDR);
    digitalWrite(LED_R,LOW);

    //Leitura do LED Vermelho
    digitalWrite(LED_G,HIGH);
    delay(10);
    LEDvalues[2] = analogRead(LDR);
    digitalWrite(LED_G,LOW);
    
    //Leitura do LED Vermelho
    digitalWrite(LED_B,HIGH);
    delay(10);
    LEDvalues[3] = analogRead(LDR);
    digitalWrite(LED_B,LOW);

    //Escrever na serial
    Serial.print("Treshold: ");
    Serial.println(LEDvalues[0]);
    Serial.print("LED Vermelho: ");
    Serial.println(LEDvalues[1]-LEDvalues[0]);
    Serial.print("LED Verde: ");
    Serial.println(LEDvalues[2]-LEDvalues[0]);
    Serial.print("LED Azul: ");
    Serial.println(LEDvalues[3]-LEDvalues[0]);
    if (LEDvalues[1]-LEDvalues[0] > 150){
      Serial.println("Vermelho!");
    }
    if (LEDvalues[2]-LEDvalues[0]> 200){
      Serial.println("Verde!");
    }
    if (LEDvalues[3]-LEDvalues[0] > 120){
      Serial.println("Azul!");
    }
    Serial.println("-------------------------");
    state = digitalRead(BURST_BTN);    
  } 
  if (digitalRead(CALIB_R) == 0){
    digitalWrite(LED_R,HIGH);
  }
  else{
    digitalWrite(LED_R,LOW);
  }
  if (digitalRead(CALIB_G) == 0){
    digitalWrite(LED_G,HIGH);
  }
  else{
    digitalWrite(LED_G,LOW);
  }
  if (digitalRead(CALIB_B) == 0){
    digitalWrite(LED_B,HIGH);
  }
  else{
    digitalWrite(LED_B,LOW);
  }

void setup() {
  // put your setup code here, to run once:
  //Configurar as saidas
  for(int i=4; i<=11; i++){
    pinMode(i,OUTPUT);
    digitalWrite(i,LOW);
  }
  //Configurar as entradas
  pinMode(SNSRL,INPUT);
  pinMode(SNSRR,INPUT);
  //pinMode(BURST_BTN,INPUT_PULLUP);
  //pinMode(CALIB_R,INPUT_PULLUP);
  //pinMode(CALIB_G,INPUT_PULLUP);
  //pinMode(CALIB_B,INPUT_PULLUP);
  //Configuracao da serial
  Serial.begin(9600);
  Serial.println("Starting...");
  Serial.println("-------------------------");
}

void loop() {
  // put your main code here, to run repeatedly:
  //if ((digitalRead(BURST_BTN) != state)&&(digitalRead(BURST_BTN) == LOW))
  //state = digitalRead(BURST_BTN);
  Line_sensor_handle();
  LED_select();
  // if (Serial.available() > 0){
  //   String read = Serial.readStringUntil('\n');
  //   Serial.println("ECHO: "+ read);
  //   int HBridgeValue = read.toInt();
  //   H_bridge_handle(HBridgeValue);
  // }
}
