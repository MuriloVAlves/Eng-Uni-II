#define LED_R 80
#define LED_G 0
#define LED_B 0
#define LDR A1
#define SNSRL 3
#define SNSRR 2
#define MOTOR_UPTIME 100
#define DUTY_CYCLE  10
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
#define ENA 11
#define ENB 10
#define SNSRL 3
#define SNSRR 2
#define PWM_HIGH  150
#define PWM_LOW 50
#define PWM_MED 80
#define PWM_FREQ  400
#define SSID_REDE  "Marcos_001"
#define SENHA_REDE  "Segredo123"

//Variaveis globais
unsigned long agora = millis();
unsigned int anguloServo = 90;

//Configuracoes para o WiFi
IPAddress local_IP(192,168,1,22);
IPAddress gateway(192,168,1,5);
IPAddress subnet(255,255,255,0);

//Configuracoes para o server
WebServer server(80);
WebSocketsServer Socket = WebSocketsServer(81);

bool state = HIGH;
int active_H_bridge = 1;

static const char PROGMEM INDEX_HTML[] = R"rawliteral()rawliteral";

void H_bridge_handle(int value){
  if (active_H_bridge == 1){
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
      analogWrite(ENA,PWM_MED);
      analogWrite(ENB,PWM_MED);
      break;
    case 2:
      //Esquerda
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH);
      digitalWrite(IN3,LOW);
      digitalWrite(IN4,HIGH);
      analogWrite(ENA,PWM_LOW);
      analogWrite(ENB,PWM_LOW);
      break;
    case 3:
      //Direita
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      analogWrite(ENA,PWM_LOW);
      analogWrite(ENB,PWM_LOW);
      break;
    case 4:
      //Trás
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH);
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      analogWrite(ENA,PWM_MED);
      analogWrite(ENB,PWM_MED);
      break;
    }
    //Configura o PWM
  //ledcWrite(0, PWM_HIGH);
  //ledcWrite(1, PWM_HIGH);
}
else{
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
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
    handle = 3;
  }
  else if ((digitalRead(SNSRL) == HIGH)&&(digitalRead(SNSRR) == LOW)){
    //Condição de ir para a direita
    handle = 2;
  }
  // Serial.print("Handle: ");
  // Serial.println((handle));
  H_bridge_handle(handle);
}
void Blink(){
  digitalWrite(LED_BUILTIN,HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN,LOW);
  delay(200);
}

void LED_select(){
    //Array para guardar o valor do LDR
    int LEDvalues[4] = {0,0,0,0}; //Treshold, LED_R, LED_G, LED_B
    
    //Ler a tensão basal
    LEDvalues[0] = analogRead(LDR);
    
    //Leitura do LED Vermelho
    digitalWrite(LED_R,HIGH);
    delay(1);
    LEDvalues[1] = analogRead(LDR);
    digitalWrite(LED_R,LOW);

    //Leitura do LED Vermelho
    digitalWrite(LED_G,HIGH);
    delay(1);
    LEDvalues[2] = analogRead(LDR);
    digitalWrite(LED_G,LOW);
    
    //Leitura do LED Vermelho
    digitalWrite(LED_B,HIGH);
    delay(1);
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
      H_bridge_handle(0);
      Blink();
    }
    if (LEDvalues[2]-LEDvalues[0]> 200){
      Serial.println("Verde!");
      H_bridge_handle(0);
      Blink();
      Blink();
    }
    if (LEDvalues[3]-LEDvalues[0] > 120){
      Serial.println("Azul!");
      H_bridge_handle(0);
      Blink();
      Blink();
      Blink();
    }
    Serial.println("-------------------------");
}
//Funcao para simbolizar o status do led
void changeStatus(){
  StatusLED = !StatusLED;
  digitalWrite(LED,StatusLED);
  RelayOutput();
}
void statusHigh(){
  StatusLED = HIGH;
  digitalWrite(LED,StatusLED);
  RelayOutput();
}

void statusLow(){
  StatusLED = LOW;
  digitalWrite(LED,StatusLED);
  RelayOutput();
}
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length){
  switch(type){
    case WStype_DISCONNECTED:
      Serial.println("Cliente desconectado");
      Blink(3,300);
      break;
      
    case WStype_CONNECTED:
      Serial.println("Cliente conectado");
      Blink(3,300);
      BroadcastStatus();
      break;
      
    case WStype_TEXT:
      String resposta = "";
      for (int i = 0; i < length; i++){
        resposta += (char)payload[i];
      }
      Serial.println(resposta);
      if (resposta == "Ligar"){
        statusHigh();
        BroadcastStatus();
      }
      else if (resposta == "Desligar"){
        statusLow();
        BroadcastStatus();
      }
      else{
        anguloServo = resposta.toInt();
        meuServo.write(anguloServo); 
        agora = millis();
      }
      break;
  }
}
void BroadcastStatus(){
      String str = "";
      switch(StatusLED){
        case HIGH:
        str = "Ligado";
        break;
        case LOW:
        str = "Desligado";
        break;
      }
      //Colocar o angulo do servo para o broadcast
      str += ";"+ String(anguloServo);
      int str_len = str.length()+1;
      char char_arr[str_len];
      str.toCharArray(char_arr,str_len);
      Socket.broadcastTXT(char_arr);
}
void setup() {
  // put your setup code here, to run once:
  //Configurar as saidas
  pinMode(LED_R,OUTPUT);
  digitalWrite(LED_R,LOW);
  pinMode(LED_G,OUTPUT);
  digitalWrite(LED_G,LOW);
  pinMode(LED_B,OUTPUT);
  digitalWrite(LED_B,LOW);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  //Configurar as entradas
  pinMode(SNSRL,INPUT);
  pinMode(SNSRR,INPUT);
  pinMode(LDR,INPUT);
  //pinMode(BURST_BTN,INPUT_PULLUP);
  //pinMode(CALIB_R,INPUT_PULLUP);
  //pinMode(CALIB_G,INPUT_PULLUP);
  //pinMode(CALIB_B,INPUT_PULLUP);
  //Configuracao do PWM para os motores
  //ledcAttachPin(ENA, 0);
  //ledcAttachPin(ENB, 1);
  //ledcSetup(0, PWM_FREQ, 8);
  //ledcSetup(1, PWM_FREQ, 8);
  //Configuracao da serial
  Serial.begin(9600);
  Serial.println("Starting...");
  Serial.println("-------------------------");
  
  //Inicializando o WiFi
  Serial.print("Configurando o ponto de acesso... ");
  Serial.println(WiFi.softAPConfig(local_IP,gateway,subnet) ? "Pronto": "Falhou");

  Serial.print("Criando ponto de acesso... ");
  Serial.println(WiFi.softAP(SSID_REDE,SENHA_REDE)? "Pronto": "Falhou");

  Serial.print("Endereco de IP - ");
  Serial.println(WiFi.softAPIP());

  //Para conectar a um roteador
  //WiFi.begin(SSID_REDE,SENHA_REDE);  
  // Serial.print("Conectando...");
  //   while(WiFi.status() != WL_CONNECTED){
  //     Serial.print('.');
  //     delay(500);
  //     changeStatus();
  //   }
  //  Serial.println("");
  //  Serial.print("Conectado com o IP: ");
  //  Serial.println(WiFi.localIP());
  //  statusLow();

   
  server.on("/", []() {
    server.send(200,"text/html",INDEX_HTML);
  });

  server.begin();
  Socket.begin();
  Socket.onEvent(webSocketEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  //if ((digitalRead(BURST_BTN) != state)&&(digitalRead(BURST_BTN) == LOW))
  //state = digitalRead(BURST_BTN);
  Line_sensor_handle();
  //LED_select();
  // if (Serial.available() > 0){
  //   String read = Serial.readStringUntil('\n');
  //   Serial.println("ECHO: "+ read);
  //   int HBridgeValue = read.toInt();
  //   H_bridge_handle(HBridgeValue);
  // }
}
