/*
  PROJETO SMART PADARIA: MONITORAMENTO DE FERMENTAÇÃO

  DESCRIÇÃO: Este código lê a temperatura e umidade do ar através do sensor DHT11,
  controla o tempo decorrido e exibe os dados em um Display LCD 16x2 e no Serial Monitor do Arduino.
  
  HARDWARE NECESSÁRIO:
  - Arduino Uno (ATmega328P)
  - Sensor DHT11 (ou DHT22)
  - Display LCD 16x2 (via interface paralela)
  - Protoboard
  - Resistor de 10K
  - Jumpers para ligação no protoboard
 */

#include<Adafruit_Sensor.h>       //Biblioteca base para sensores Adfruit (DHT)
#include<DHT.h>                   // Biblioteca DHT para leitura de dados do sensor
#include<DHT_U.h>                 // Biblioteca Unified Sensor para acesso a eventos e metadados
#include<LiquidCrystal.h>         // Biblioteca para controle do Display LCD padrão


// Objeto LCD: Inicializa a biblioteca com os pinos de interface
// Pinos: (RS, E, D4, D5, D6, D7) conectados ao Arduino
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 

// --- Configurações do Sensor DHT ---
#define DHTTYPE DHT11              // Define o tipo de sensor que está sendo utilizado (DHT11)
#define DHTPIN 2                   // Define o pino digital do Arduino ao qual o pino de dados do sensor está conectado  

// Cria uma instância unificada do sensor DHT, usando o pino e o tipo definidos
DHT_Unified dht(DHTPIN, DHTTYPE);  

// Variáveis de controle de tempo
uint32_t delayExibicaoMS = 2000;   // Tempo de exibição de cada tela (2 segundos)
unsigned long tempoInicio;         // Armazena o valor de millis() (tempo total em ms desde o boot) no setup()

// Variável de controle para alternar as telas
bool mostrarTempo = false;


void setup() {
  // Inicializa a comunicação serial a 9600 bps para depuração e monitoramento no PC
  Serial.begin(9600);

  // Inicializa o sensor DHT
  dht.begin();

   // Variável local que armazena informações sobre o sensor
  sensor_t sensor;

  // Inicializa o Display LCD
  lcd.begin(16, 2);


  // Mensagem de boas-vindas na Linha 0
  lcd.print("Smart Padaria!");
  // Move o cursor para a primeira coluna da segunda linha (0, 1)
  lcd.setCursor(0, 1);
  // Mensagem de inicialização na Linha 1
  lcd.print("Iniciando...");
  
  // Pausa de 3 segundos para que o usuário veja a mensagem inicial antes de começar a leitura
  delay(3000);

  // Limpa o display para a exibição dos dados de leitura
  lcd.clear();

   //Inicializa o contador de tempo.
  tempoInicio = millis(); 
}

void loop() {
  // Lógica de alternância de tela: O display muda a cada 'delayExibicaoMS'
  // O valor de 'mostrarTempo' inverte a cada ciclo de exibição
  mostrarTempo = !mostrarTempo; 

  if(!mostrarTempo){
      // Objeto para armazenar o "evento" (leitura) dos sensores.
      sensors_event_t event;

      // --- Leitura da Temperatura ---
      dht.temperature().getEvent(&event);
      
      //Exibição da Temperatura e umidade no Serial Monitor
      Serial.println("--------------------");

      if(isnan(event.temperature)){
        Serial.println("Erro na leitura da Temperatura!");
      } else{
        Serial.print("Temperatura: ");
        Serial.print(event.temperature, 1);
        Serial.println(" ºC");

        // 1. Exibição no LCD (Linha 0)
        lcd.setCursor(0, 0);
        // Limpar apenas o necessário para não piscar
        lcd.print("Temp: ");
        // Imprime o valor com uma casa decimal (1)
        lcd.print(event.temperature, 1); 
        lcd.print(" C ");
      }
  

      // --- Leitura da Umidade ---
      // Obtém o evento de umidade e armazena os dados no mesmo objeto 'event'
      dht.humidity().getEvent(&event); 

      if(isnan(event.relative_humidity)){
        Serial.println("Erro na leitura da Umidade!");
      }else{
        Serial.println("Umidade: ");
        Serial.print(event.relative_humidity);
        Serial.println("%");

        // 2. Exibição no LCD (Linha 1)
        lcd.setCursor(0, 1);
        lcd.print("Umidade: ");
        // Imprime o valor com uma casa decimal (1)
        lcd.print(event.relative_humidity, 1);
        lcd.print("%  "); 
      }
  }else {

    // Calcula o tempo decorrido total em milissegundos
    unsigned long tempoDecorrido = millis() - tempoInicio; 
    
    // Converte milissegundos em minutos e segundos
    int segundosTotais = tempoDecorrido / 1000;
    int minutos = segundosTotais / 60;
    int segundos = segundosTotais % 60; // Resto da divisão é o segundo
    
    // Exibe no Serial
    Serial.print("Tempo Decorrido: ");
    Serial.print(minutos);
    Serial.print("m ");
    Serial.print(segundos);
    Serial.println("s");

    // Exibe no LCD
    lcd.clear(); // Limpa a tela para a nova mensagem
    lcd.setCursor(0, 0);
    lcd.print("Fermentacao:");
    
    lcd.setCursor(0, 1);
    // Adiciona um '0' à esquerda se o minuto/segundo for menor que 10 (formato MM:SS)
    if (minutos < 10) lcd.print("0");
    lcd.print(minutos);
    lcd.print("m ");
    if (segundos < 10) lcd.print("0");
    lcd.print(segundos);
    lcd.print("s");
  }

  // Pausa antes da próxima alternância de tela/leitura.
  // A frequência de atualização de TELA 1 e TELA 2 é controlada por esta linha.
  delay(delayExibicaoMS);
}
