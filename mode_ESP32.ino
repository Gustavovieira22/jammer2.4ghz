#include <RF24.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BOTAO 32
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool botaoAnterior = HIGH;

SPIClass *hp = nullptr;
SPIClass *vp = nullptr;

RF24 radioA(26, 5, 10000000);    // CE e CSN — usará VSPI
RF24 radioB(16, 15, 10000000);   // CE e CSN — usará HSPI

byte i = 45;
int level = 1;
static const uint8_t palvls[] = {RF24_PA_HIGH, RF24_PA_MAX};

void setup() {
  esp_wifi_stop();
  esp_wifi_deinit();
  
  pinMode(BOTAO, INPUT_PULLUP); // Ativa pull-up interno

  Wire.begin(21, 22); // SDA, SCL - Display

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

  vp = new SPIClass(VSPI);
  hp = new SPIClass(HSPI);

  vp->begin();
  hp->begin();

  if (!radioA.begin(vp)) {
    display.setRotation(2); 
    display.clearDisplay();
    display.setTextSize(1);       
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);    
    display.println("ERRO RADIO - A");
    display.display();            
    
    while(1);
  }
  if (!radioB.begin(hp)) {
    display.setRotation(2); 
    display.clearDisplay();
    display.setTextSize(1);        
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);     
    display.println("ERRO RADIO - B");
    display.display();             
    
    while(1);
  }

  delay(200); //delay de estabilização dos rádios

  radioB.startConstCarrier(RF24_PA_MAX, i);
  radioA.startConstCarrier(RF24_PA_MAX, i);
  
  display.setRotation(2); 
  display.clearDisplay();
  display.setTextSize(2);  
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("ONLINE");
  display.display();        
}

void two() {

  level = random(0,2);
  i = random(0,80);

  radioA.setPALevel(palvls[level]);
  radioB.setPALevel(palvls[1 - level]);

  radioA.setChannel(i);
  radioB.setChannel(79 - i);
}

void loop() { 
  bool estadoAtual = digitalRead(BOTAO);

  // Detecta transição de HIGH para LOW (apertou o botão)
  if (botaoAnterior == HIGH && estadoAtual == LOW) {

    // --- Exibe informações no display ---
    display.setRotation(2); 
    display.clearDisplay();
    display.setTextSize(1);  // usamos texto menor para caber tudo
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);

    // Informação para radioA
    display.print("radio A: ");
    display.print(radioA.getChannel());
    display.print(" - PWR - ");
    display.println(radioA.getPALevel());

    // Informação para radioB
    display.print("radio B: ");
    display.print(radioB.getChannel());
    display.print(" - PWR - ");
    display.println(radioB.getPALevel());

    display.display();
    // ------------------------------------
  }

  botaoAnterior = estadoAtual;

  two();
}