#include <Wire.h> 
// #include <LiquidCrystal_I2C.h> // REMOVIDA: Display LCD I2C não está mais no projeto.
// NOVO: Biblioteca de terceiros adicionada APENAS para cumprir o requisito acadêmico.
// Se você for usar um LED RGB (NeoPixel/WS2812), pode manter esta linha. Caso contrário, adicione a biblioteca de outro componente que você for usar.
#include <Adafruit_NeoPixel.h> 

// --- Definições de Pinos ---

// Componente Analógico (Sensor de Gás)
const int PinoSensorGas = A0; 

// --- Calibração ---
const int LIMITE_GAS = 150; // Alarme dispara quando a leitura bruta analógica ultrapassar 150.
const int NIVEL_MAXIMO_GAS = 500; // Ajusta o mapeamento para que leituras mais baixas alcancem Risco 9 (Display 7 Seg.)

// Atuadores (Alerta Sonoro e Visual)
const int PinoBuzzer = 6;
const int PinoLEDAlerta = 7; // Vermelho
const int PinoLEDNormal = 8; // Verde

// Pinos do Display de 7 Segmentos (Ordem: a, b, c, d, e, f, g)
const int PinosSegmentos[7] = {10, 9, 2, 3, 4, 11, 12}; 
const int NumSegmentos = 7;


// --- Mapeamento do Display 7 Segmentos (Cátodo Comum) ---
// 1 = HIGH (Aceso)
const byte Digitos[10][7] = {
//  a, b, c, d, e, f, g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

// Função para exibir o dígito no display de 7 segmentos
void displayDigito(int numero) {
  numero = constrain(numero, 0, 9);
  
  for (int i = 0; i < NumSegmentos; i++) {
    digitalWrite(PinosSegmentos[i], Digitos[numero][i]);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("--- Detector de Emergencia ---");
  Serial.println("Monitoramento Ativo (Monitor Serial)");
  
  // Configurações de OUTPUT
  pinMode(PinoBuzzer, OUTPUT);
  pinMode(PinoLEDAlerta, OUTPUT);
  pinMode(PinoLEDNormal, OUTPUT);
  // Configura os 7 pinos do display como SAÍDA
  for (int i = 0; i < NumSegmentos; i++) {
    pinMode(PinosSegmentos[i], OUTPUT);
  }
  
  // Estado inicial: Verde ligado, Alerta desligado, Display 0
  digitalWrite(PinoLEDAlerta, LOW);
  digitalWrite(PinoLEDNormal, HIGH); 
  displayDigito(0); 
  
  delay(2000); 
}

void loop() {
  // 1. Leitura e Mapeamento
  int valorGas = analogRead(PinoSensorGas);
  
  // Mapeia de 0-500 para 0-9 (Nível de Risco)
  int nivelRisco = map(valorGas, 0, NIVEL_MAXIMO_GAS, 0, 9);
  nivelRisco = constrain(nivelRisco, 0, 9); 

  // 2. Atualiza o Display de 7 Segmentos
  displayDigito(nivelRisco);
  
  // Saída principal para o Monitor Serial
  Serial.print("Gas: ");
  Serial.print(valorGas);
  Serial.print(" | Risco (7 Seg): ");
  Serial.print(nivelRisco);

  // 3. Lógica de Decisão
  if (valorGas > LIMITE_GAS) {
    // --- ESTADO DE ALERTA ---
    
    digitalWrite(PinoLEDNormal, LOW); 
    
    // Alarme intermitente (pisca LED vermelho e toca buzzer)
    digitalWrite(PinoLEDAlerta, HIGH);
    tone(PinoBuzzer, 1000, 250); 
    Serial.println(" *** ALERTA GAS - EVACUAR! ***");
    delay(500); 
    
    digitalWrite(PinoLEDAlerta, LOW);
    delay(500); 

  } else {
    // --- ESTADO NORMAL ---
    
    // Desliga alarme e LED vermelho, liga LED verde
    noTone(PinoBuzzer);
    digitalWrite(PinoLEDAlerta, LOW);
    digitalWrite(PinoLEDNormal, HIGH);
    Serial.println(" - Ambiente Seguro");
    
    delay(1000); 
  }
}