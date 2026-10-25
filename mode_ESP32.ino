//PROJETO COM OBJETIVO UNICAMENTE EDUCATIVO!
#include <RF24.h>
#include <esp_wifi.h>

SPIClass *hp = nullptr;
SPIClass *vp = nullptr;

RF24 radioA(26, 5, 10000000);    // CE e CSN — usará VSPI
RF24 radioB(16, 15, 10000000);   // CE e CSN — usará HSPI

byte i = 45;

void setup() {
  //desativa wifi para liberar mais processamento
  esp_wifi_stop();
  esp_wifi_deinit();

  Serial.begin(115200);
  delay(1000);

  vp = new SPIClass(VSPI);
  hp = new SPIClass(HSPI);

  vp->begin();
  hp->begin();

  //os erros só sao exibidos no console serial da IDE arduino
  if (!radioA.begin(vp)) {
    Serial.println("ERRO RADIO - A");          
    while(1); //sistema trava caso haja erro em algum rádio
  }

  if (!radioB.begin(hp)) {
    Serial.println("ERRO RADIO - B");          
    while(1);
  }

  //delay de estabilização dos rádios
  delay(200); 

  //seta os rádios em potência máxima
  radioB.startConstCarrier(RF24_PA_MAX, i);
  radioA.startConstCarrier(RF24_PA_MAX, i);

}

void two() {
  //salta aleatoriamente entre frequências
  i = random(0,80);

  //cada radio opera em uma faixa oposta
  radioA.setChannel(i);
  radioB.setChannel(79 - i);
}

void loop() { 
  two();
}
