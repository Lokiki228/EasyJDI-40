#include <JDI-40.h>


JDI40 jdi(Serial);  //передать Stream объект, например Serial, либо объект софтсериала



void setup() {
  Serial.begin(9600);  //по стандарту у JDI стоит 9600, я не менял и думаю ненужно. Для большей
  //стабильности можно опустить ниже, но перед этим сделать jdi.setBaud(uint8_t baud);
  jdi.configDevice(_DUPLEX, D6, D5);

  if (jdi.begin()) Serial.print("JDI запустилась");

  if (jdi.setID(8899, 8899)) Serial.print("ID ок");  //ИД сети, ИД девайса  !!!ПОКА ЧТО от 1000 до 9999, 4 знака, можно и больше(16 битное). В разработке
  if (jdi.setChannel(1)) Serial.print("Канал ок");
  if (jdi.setPower(9)) Serial.print("Мощность ок");

  jdi.print("Hi!");  //эксперементальная функция, для симплекса
}

void loop() {
}