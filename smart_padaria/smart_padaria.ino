/*
  PROJETO SMART PADARIA: MONITORAMENTO DE FERMENTAÇÃO COM CONTROLE IR
*/

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h>
#include <IRremote.h> // 2.6.0

// Objeto LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 

// Configurações
#define DHTTYPE DHT11
#define DHTPIN 2
#define RECV_PIN 11
#define BUZER_PIN 10

// Sensor DHT
DHT_Unified dht(DHTPIN, DHTTYPE);

// Configura sensor IR
IRrecv irrecv(RECV_PIN);
decode_results results;

// Variáveis de controle
uint32_t delayExibicaoMS = 2000;
unsigned long tempoInicio;
bool mostrarTempo = false;

// Variáveis para controle do IR
unsigned long ultimoComandoIR = 0;
bool mostrarIR = false;
unsigned long tempoExibicaoIR = 2000;
String ultimoComando = "Nenhum";
unsigned long ultimaLeituraIR = 0;
const unsigned long DEBOUNCE_IR = 300; // Tempo mínimo entre leituras

// COMANDOS DO SEU CONTROLE REMOTO
const unsigned long IR_1           = 0xFFA25D;  // 1
const unsigned long IR_2           = 0xFF629D;  // 2
const unsigned long IR_3           = 0xFFE21D;  // 3
const unsigned long IR_4           = 0xFF22DD;  // 4
const unsigned long IR_5           = 0xFF02FD;  // 5
const unsigned long IR_6           = 0xFFC23D;  // 6
const unsigned long IR_7           = 0xFFE01F;  // 7
const unsigned long IR_8           = 0xFFA857;  // 8
const unsigned long IR_9           = 0xFF906F;  // 9
const unsigned long IR_ASTERISCO   = 0xFF6897;  // *
const unsigned long IR_0           = 0xFF9867;  // 0
const unsigned long IR_HASHTAG     = 0xFFB04F;  // #
const unsigned long IR_SET_UP      = 0xFF18E7;  // up
const unsigned long IR_SET_LEFT    = 0xFF10EF;  // left
const unsigned long IR_OK          = 0xFF38C7;  // ok
const unsigned long IR_SET_RIGHT   = 0xFF5AA5;  // right
const unsigned long IR_SET_DOWN    = 0xFF4AB5;  // down

void setup() {
  Serial.begin(9600);
  pinMode(BUZER_PIN,OUTPUT);
  
  irrecv.enableIRIn(); // Inicia o receptor
  
  // Inicializa sensores
  dht.begin();
  
  // Inicializa LCD
  lcd.begin(16, 2);
  lcd.print("Smart Padaria!");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  
  delay(2000);
  lcd.clear();
  tempoInicio = millis();
}

void loop() {

  verificarIR();
  
  // Controla a exibição
  if (mostrarIR) {
    exibirComandoIR();
  } else {
    exibirDadosNormais();
  }
  
  delay(50); // Reduzido para melhor responsividade
}

void verificarIR() {
  // Verifica debounce - evita leituras muito rápidas
  if (millis() - ultimaLeituraIR < DEBOUNCE_IR) {
    return;
  }
  
  // Detecta comandos IR usando a biblioteca
  if (irrecv.decode(&results)) {
    unsigned long comando = results.value;
    
    // Ignora repetições e valores inválidos
    if (comando != 0xFFFFFFFF && comando != 0 && comando != ultimoComandoIR) {
      String comandoNome = identificarComandoIR(comando);
      
      ultimoComando = comandoNome;
      
      // Ativa a exibição do IR
      mostrarIR = true;
      ultimoComandoIR = comando;
      ultimaLeituraIR = millis();
      
      // Log no serial
      Serial.print("CONTROLE: ");
      Serial.print(comandoNome);
      Serial.print(" (0x");
      Serial.print(comando, HEX);
      Serial.print(") - ");
      // Executa ação baseada no comando
      executarAcaoIR(comando, comandoNome);
    }
    
    // Prepara para próximo código
    irrecv.resume();
  }
  
  // Desativa a exibição do IR após o tempo determinado
  if (mostrarIR && (millis() - ultimaLeituraIR > tempoExibicaoIR)) {
    mostrarIR = false;
    lcd.clear();
  }
}

String identificarComandoIR(unsigned long valor) {
  switch(valor) {
    case IR_1:           return "BOTAO 1";
    case IR_2:           return "BOTAO 2";
    case IR_3:           return "BOTAO 3";
    case IR_4:           return "BOTAO 4";
    case IR_5:           return "BOTAO 5";
    case IR_6:           return "BOTAO 6";
    case IR_7:           return "BOTAO 7";
    case IR_8:           return "BOTAO 8";
    case IR_9:           return "BOTAO 9";
    case IR_0:           return "BOTAO 0";
    case IR_ASTERISCO:   return "REINICIAR TEMPO";
    case IR_HASHTAG:     return "BOTAO #";
    case IR_SET_UP:      return "CIMA";
    case IR_SET_LEFT:    return "ESQUERDA";
    case IR_OK:          return "OK";
    case IR_SET_RIGHT:   return "DIREITA";
    case IR_SET_DOWN:    return "BAIXO";
    case 0xFFFFFFFF:     return "REPETICAO";
    default: 
      return "DESCONHECIDO";
  }
}


void beep(int milisegundos){
    analogWrite(BUZER_PIN, 200); 
    delay(milisegundos);
    analogWrite(BUZER_PIN, 0); 
}

void executarAcaoIR(unsigned long comando, String nome) {
  Serial.print(">>> Acao executada: ");
  beep(50);
  if (comando == IR_ASTERISCO) {
    tempoInicio = millis();
  
    exibirTempoFermentacao();
  }
  else if (comando == IR_OK) {
    Serial.println("OK pressionado");
    // Adicione aqui ações para o botão OK
  }
  else if (comando == IR_SET_UP) {
    Serial.println("Cima pressionado");
    // Adicione aqui ações para o botão CIMA
  }
  else if (comando == IR_SET_DOWN) {
    Serial.println("Baixo pressionado");
    // Adicione aqui ações para o botão BAIXO
  }
  else if (comando >= IR_1 && comando <= IR_9) {
    int numero = comando - IR_1 + 1;
    Serial.print("Numero ");
    Serial.println(numero);
    // Adicione aqui ações para os números
  }
  else {
    Serial.println("Comando reconhecido");
  }
  ultimoComandoIR = 0;
}

void exibirComandoIR() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONTROLE:");
  
  lcd.setCursor(0, 1);  
  lcd.print(ultimoComando);
}

void exibirDadosNormais() {
  static unsigned long ultimaAtualizacao = 0;
  
  // Alterna entre telas a cada 2 segundos
  if (millis() - ultimaAtualizacao > delayExibicaoMS) {
    mostrarTempo = !mostrarTempo;
    ultimaAtualizacao = millis();
    lcd.clear();
  }
  
  if (mostrarTempo) {
    exibirTempoFermentacao();
  } else {
    exibirTemperaturaUmidade();
  }
}

void exibirTemperaturaUmidade() {
  sensors_event_t event;
  
  // Temperatura - Linha 0
  lcd.setCursor(0, 0);
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    lcd.print("Temp: Erro    ");
  } else {
    lcd.print("Temp: ");
    lcd.print(event.temperature, 1);
    lcd.print(" C   ");
  }
  
  // Umidade - Linha 1
  lcd.setCursor(0, 1);
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    lcd.print("Umid Erro    ");
  } else {
    lcd.print("Umid: ");
    lcd.print(event.relative_humidity, 1);
    lcd.print("%   ");
  }
}

void exibirTempoFermentacao() {
  unsigned long tempoDecorrido = millis() - tempoInicio;
  int segundosTotais = tempoDecorrido / 1000;
  int minutos = segundosTotais / 60;
  int segundos = segundosTotais % 60;
  int horas = minutos / 60;
  minutos = minutos % 60;
  
  // Linha 0
  lcd.setCursor(0, 0);
  lcd.print("Tempo Ferm:    ");
  
  // Linha 1 - Formato HH:MM:SS
  lcd.setCursor(0, 1);
  if (horas < 10) lcd.print("0");
  lcd.print(horas);
  lcd.print(":");
  if (minutos < 10) lcd.print("0");
  lcd.print(minutos);
  lcd.print(":");
  if (segundos < 10) lcd.print("0");
  lcd.print(segundos);
}