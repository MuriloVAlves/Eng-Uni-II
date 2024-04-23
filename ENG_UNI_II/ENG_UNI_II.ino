#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

#define LED  2
#define LED_R 5
#define LED_G 18
#define LED_B 19
#define LDR 32
#define MOTOR_UPTIME 100
#define DUTY_CYCLE  10
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENA 12
#define ENB 13
#define SNSRL 35
#define SNSRR 34
#define PWM_HIGH  150
#define PWM_LOW 50
#define PWM_MED 80
#define PWM_FREQ  980
#define SSID_REDE  "Marcos_001"
#define SENHA_REDE  "Segredo123"

//Variaveis globais
int active_H_bridge = 1;
bool autonomous_control = HIGH;
int H_bridge_broadcast_value = 0;

//Configuracoes para o WiFi
IPAddress local_IP(192,168,1,22);
IPAddress gateway(192,168,1,5);
IPAddress subnet(255,255,255,0);

//Configuracoes para o server
WebServer server(80);
WebSocketsServer Socket = WebSocketsServer(81);

static String PROGMEM INDEX_HTML = "<!DOCTYPE html> <html> <head> <title>ESP 32 CONTROL SYSTEM</title> <style type='text/CSS'> #header{ background-color: #2f4468; border-radius: 10px; padding: 5px; width: 100%; overflow: hidden; height: 20%; } .titulo{ font-family: Brush Script MT, Brush Script Std, cursive; text-align: center; font-size: 30px; /*color: #c9a959;*/ color: white; } body{ background-color: #f4f2ef; background-size: 100% 100%; overflow: hidden; } .wrapper{ width: 80%; padding: 5%; /*display: grid; grid-template-columns: 70% 30%;*/ margin: auto; } #powerbtn{ border-radius: 5px; text-shadow: 5px; border: 0px; padding-top: 10px; padding-bottom: 10px; font-size: 15px; width: 100%; height: 10em; } .OFF{ background-color: #2f4468; color: white; } .OFF:hover{ background-color: #e4850f; } .ON{ background-color: #e4850f; color: white; } .center{ display: flex; justify-content: center; align-items: center; height: 100%; /*border: 3px solid green;*/ } .slidecontainer { position: absolute; width: 100%; /* Width of the outside container */ } .slider { -webkit-appearance: none; appearance: none; width: 80%; height: 10em; background: #d3d3d3; outline: none; opacity: 0.7; -webkit-transition: .2s; transition: opacity .2s; } .slider:hover { opacity: 1; } .slider::-webkit-slider-thumb{ -webkit-appearance: none; appearance: none; width: 20em; height: 20em; border-radius: 50%; background: #2076ac; cursor: pointer; } .spacer{ padding-top: 10%; } .angle{ display: flex; justify-content: center; align-items: center; height: 100%; padding: 2%; font-size: 20px; } .angle { height: 10em; font-size: 20px; } .button { background-color: #2f4468; border: none; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 18px; margin: 6px 3px; cursor: pointer; -webkit-touch-callout: none; -webkit-user-select: none; -khtml-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; -webkit-tap-highlight-color: rgba(0,0,0,0); } .button:hover{ background-color: #e4850f; } .button:active{ background-color:#0dde26; } .btntable{ display: flex; justify-content: center; } </style> </head> <body> <header id='header'> <div class='titulo'> <h1>Controle</h1> </div> </header> <div class='wrapper'> <div class='center'> <input type='button' value='Ligar' id='powerbtn' class='OFF'> </div> <div class = 'spacer'></div> <div class = 'btntable'> <table> <tr><td colspan='3' align='center'><button class='button' id='fwbtn'>Forward</button></td></tr> <tr><td align='center'><button class='button' id='leftbtn'>Left</button></td><td align='center'><button class='button' id='stopbtn'>Stop</button></td><td align='center'><button class='button' id='rightbtn'>Right</button></td></tr> <tr><td colspan='3' align='center'><button class='button' id='bkwdbtn'>Backward</button></td></tr> </table> </div> </div> <footer> </footer> <script> var Socket; var botao = document.getElementById('powerbtn'); botao.addEventListener('click', powerChanged); var fwrdbtn = document.getElementById('fwbtn'); fwrdbtn.addEventListener('mousedown',function(){toggleCheckbox('forward');}); fwrdbtn.addEventListener('touchstart',function(){toggleCheckbox('forward');}); fwrdbtn.addEventListener('mouseup',function(){toggleCheckbox('stop');}); fwrdbtn.addEventListener('touchend',function(){toggleCheckbox('stop');}); var leftbtn = document.getElementById('leftbtn'); leftbtn.addEventListener('mousedown',function(){toggleCheckbox('left');}); leftbtn.addEventListener('touchstart',function(){toggleCheckbox('left');}); leftbtn.addEventListener('mouseup',function(){toggleCheckbox('stop');}); leftbtn.addEventListener('touchend',function(){toggleCheckbox('stop');}); var stopbtn = document.getElementById('stopbtn'); stopbtn.addEventListener('mousedown',function(){toggleCheckbox('stop');}); stopbtn.addEventListener('touchstart',function(){toggleCheckbox('stop');}); var rightbtn = document.getElementById('rightbtn'); rightbtn.addEventListener('mousedown',function(){toggleCheckbox('right');}); rightbtn.addEventListener('touchstart',function(){toggleCheckbox('right');}); rightbtn.addEventListener('mouseup',function(){toggleCheckbox('stop');}); rightbtn.addEventListener('touchend',function(){toggleCheckbox('stop');}); var bkwdbtn = document.getElementById('bkwdbtn'); bkwdbtn.addEventListener('mousedown',function(){toggleCheckbox('backward');}); bkwdbtn.addEventListener('touchstart',function(){toggleCheckbox('backward');}); bkwdbtn.addEventListener('mouseup',function(){toggleCheckbox('stop');}); bkwdbtn.addEventListener('touchend',function(){toggleCheckbox('stop');}); function startWebSocket(){ Socket = new WebSocket('ws://'+window.location.hostname+':81/'); Socket.onmessage = function(event){ processarComando(event); } }; function processarComando(event) { console.log(event.data); let dados = event.data.split(';'); switch (dados[0]){ case 'Ligado': botao.className = 'ON'; botao.value = 'Desligar'; break; case 'Desligado': botao.className = 'OFF'; botao.value = 'Ligar'; break; } let handleESP32 = dados[1]; console.log(handleESP32); }; function powerChanged(){ console.log(botao.value); handleSocketSend(botao.value); if(botao.value == 'Ligar'){ botao.className = 'ON'; botao.value = 'Desligar'; } else{ botao.className = 'OFF'; botao.value = 'Ligar'; } }; function handleSocketSend(valor){ Socket.send(valor); }; window.onload = function(event){ startWebSocket(); }; function toggleCheckbox(x) { let H_Bridge_Handle = 0; console.log(x); switch(x){ case 'forward': H_Bridge_Handle = 1; break; case 'left': H_Bridge_Handle = 2; break; case 'right': H_Bridge_Handle = 3; break; case 'backward': H_Bridge_Handle = 4; break; default: H_Bridge_Handle = 0; break; }; handleSocketSend(H_Bridge_Handle); }; </script> </body> </html>";

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
      ledcWrite(0,PWM_MED);
      ledcWrite(1,PWM_MED);
      break;
    case 2:
      //Esquerda
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH);
      digitalWrite(IN3,LOW);
      digitalWrite(IN4,HIGH);
      ledcWrite(0,PWM_LOW);
      ledcWrite(1,PWM_LOW);
      break;
    case 3:
      //Direita
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      ledcWrite(0,PWM_LOW);
      ledcWrite(1,PWM_LOW);
      break;
    case 4:
      //Trás
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH);
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      ledcWrite(0,PWM_MED);
      ledcWrite(1,PWM_MED);
      break;
    }
    //Configura o PWM
  // ledcWrite(0, PWM_HIGH);
  // ledcWrite(1, PWM_HIGH);
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
void Blink(int vezes, int tempo){
  for(int i = 0; i < 2*vezes; i++){
    changeStatus();
    delay(tempo/(2*vezes));
  }
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
      Blink(1,300);
    }
    if (LEDvalues[2]-LEDvalues[0]> 200){
      Serial.println("Verde!");
      H_bridge_handle(0);
      Blink(2,300);
    }
    if (LEDvalues[3]-LEDvalues[0] > 120){
      Serial.println("Azul!");
      H_bridge_handle(0);
      Blink(3,300);
    }
    Serial.println("-------------------------");
}
//Funcao para simbolizar o status do led
void changeStatus(){
  autonomous_control = !autonomous_control;
  digitalWrite(LED,autonomous_control);
  BroadcastStatus();
}
void statusHigh(){
  autonomous_control = HIGH;
  digitalWrite(LED,autonomous_control);
  BroadcastStatus();
}

void statusLow(){
  autonomous_control = LOW;
  digitalWrite(LED,autonomous_control);
  BroadcastStatus();
}
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length){
  Serial.println("SOCKET CONNECTION");
  Serial.println("Type:"+type);
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
        H_bridge_broadcast_value = resposta.toInt();
        Serial.println(H_bridge_broadcast_value);
        H_bridge_handle(H_bridge_broadcast_value);
      }
      break;
  }
}
void BroadcastStatus(){
      String str = "";
      switch(autonomous_control){
        case HIGH:
        str = "Ligado";
        break;
        case LOW:
        str = "Desligado";
        break;
      }
      //Colocar o angulo do servo para o broadcast
      str += ";"+ String(H_bridge_broadcast_value);
      int str_len = str.length()+1;
      char char_arr[str_len];
      str.toCharArray(char_arr,str_len);
      Socket.broadcastTXT(char_arr);
}
void setup() {
  // put your setup code here, to run once:
  //Configurar as saidas
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
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
  ledcAttachPin(ENA, 0);
  ledcAttachPin(ENB, 1);
  ledcSetup(0, PWM_FREQ, 8);
  ledcSetup(1, PWM_FREQ, 8);
  //Configuracao da serial
  Serial.begin(115200);
  Serial.println();
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
  // WiFi.begin(SSID_REDE,SENHA_REDE);  
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
  delay(1000);
  statusHigh();
}

void loop() {
  // put your main code here, to run repeatedly:
  //if ((digitalRead(BURST_BTN) != state)&&(digitalRead(BURST_BTN) == LOW))
  //state = digitalRead(BURST_BTN);
  server.handleClient();
  Socket.loop();
  if(autonomous_control){
    Line_sensor_handle();
  //LED_select();
  }
  else{
    H_bridge_handle(H_bridge_broadcast_value);
  }
  // if (Serial.available() > 0){
  //   String read = Serial.readStringUntil('\n');
  //   Serial.println("ECHO: "+ read);
  //   int HBridgeValue = read.toInt();
  //   H_bridge_handle(HBridgeValue);
  // }
}