/*------------------------------- LIBRAIRIES UTILISEES -------------------------------*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SoftwareSerial.h>

/*------------------------------- DECLARATIONS ET DEFINITIONS DIVERSES -------------------------------*/


// Définition des pins pour les capteurs à étudier

#define GRAPH_SENSOR A0 //Capteur graphite sur l'entrée analogique A0
#define FLEX_SENSOR A1 //Capteur commercial sur l'entrée analogique A1


// Définition des pins de l'encodeur rotatoire

#define encoderPinA 2   //CLK
#define encoderPinB 4  
#define Switch 3



// Définition des broches RXD et TXD du module BT HC-05
#define RX_PIN 8 // Broche TXD du module BT connecté à la broche 7 de l'arduino (IL SONT INVERSEE!!!!) pas de communication sinon
#define TX_PIN 7 // Broche RXD du module BT connecté à la broche 8 de l'arduino


// Déclaration d'un objet de type SoftwareSerial pour poourvoir communiquer avec le BT
SoftwareSerial Bluetooth(RX_PIN, TX_PIN); //Définition du software serial


// Déclaration de l'écran OLED avec la bibliothèque AdaFruit

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



// Menu de l'écran OLED

const char* menuItems[] = {"Res: 200 Ohms", "Res: 1 kOhms", "Res: 5 kOhms", "Res: 7.5 kOhms", "Res: 10 kOhms"};
const int totalItems = 5;
int currentSelection = 0;



// Déclaration pour le potentiomètre digital

const byte csPin           = 10;      //Active le potentiomètre digitale, si LOW le MCP écoute le SPI
const int  maxPositions    = 256;    
const long rAB             = 10000.0;  //Résistance totale du potentiomètre, MCP41010 --> 10kOhm
const byte rWiper          = 52;    
const byte pot0Write       = 0x11;    // pot0 addr, commande SPI pour écrire dans le potentiomètre
const byte pot0Shutdown    = 0x21;    // pot0 shutdown, commande SPI pour éteindre le potentiomètre
int pos_res_variable = 5; // Fixe la résistance initiale du potentiomètre digital



// Déclaration pour le flex sensor et le capteur graphite

const float VCC = 5.0;                      // Tension alimentation
const float flexR_DIV = 27000.0;            // Résistance fixe du pont diviseur de tension qui accompagne le FlexSensor
const float flexflatResistance = 25000.0;   // Résistance FlexSensor à plat
const float flexbendResistance = 100000.0;  // Résistance FlexSensor à 90 dégrés
float tension_graphite = 0;        // Tension mesuré par le capteur graphite
float tension_flex = 0;            // Tension mesuré par le FlexSensor
long resistance_variable = 0;      // Résistance du MCP
float resistance_graphite = 0;     // Résistance du capteur graphene
long resistance_flex = 0;          // Résistance FlexSensor



 

// Varisables du movement de l'éncodeur

volatile long encoderValue = 0;
long lastEncoderValue = -1;
unsigned long lastButtonTime = 0;
const int debounceDelay = 200;

volatile unsigned long lastInterruptTime = 0;



void setup() {

  Serial.begin(9600);
  Bluetooth.begin(9600);

  // Initialisation des pins

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(Switch, INPUT);

  pinMode(GRAPH_SENSOR, INPUT);
  pinMode(FLEX_SENSOR, INPUT);

  
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, FALLING);

  Serial.println("Hola que tal");

  // Initialisation de l'écran OLED

  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {    //Si no reconoce OLED
  Serial.println("No reconozco LED");
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  drawMenu(); // Dibujo inicial
  Serial.println("OLED dibujao");
  
  // Initialisation de la communication avec le potentiomètre digital

  digitalWrite(csPin, HIGH);  // Le dispositif SPI n'est pas sélectioné pour éviter des activations accidentales --> pas de communication  
  pinMode(csPin, OUTPUT);          
  SPI.begin();

  Serial.println("--- SETUP COMPLETADO, ENTRANDO AL LOOP ---");
}



void loop() {

  // Movement de l'éncodeur
  if (encoderValue != lastEncoderValue) {

    // Circulaire!!!
    if (encoderValue >= totalItems) encoderValue = 0;
    if (encoderValue < 0) encoderValue = totalItems - 1;

    currentSelection = encoderValue;
    lastEncoderValue = encoderValue;
    drawMenu();

  }

  // Pulsar boton
  if (digitalRead(Switch) == LOW) {

    if (millis() - lastButtonTime > debounceDelay) {
      actionbouton(currentSelection);
      lastButtonTime = millis();
      drawMenu();

    }

  }

  // Mesures des capteurs et de la résistance variable

  int valor_graph = analogRead(GRAPH_SENSOR);
  tension_graphite =  valor_graph * VCC / 1023.0;
  resistance_variable = ((rAB * pos_res_variable) / maxPositions) + rWiper;

if (tension_graphite > 0.01) {
    //resistance_graphite = ((VCC/tension_graphite)*100000*(1+(1000/resistance_variable))-100000-10000)/1000000; //Résultat en MOHMS
    resistance_graphite = ((VCC/tension_graphite)*100000*(1+(1000/resistance_variable))-100000-10000); //Résultat en OHMS
  }

  int valor_flex = analogRead(FLEX_SENSOR); 
  tension_flex = valor_flex * VCC / 1023.0;

  //resistance_flex = 0.0; 
  if (tension_flex > 0.01) {
    resistance_flex = flexR_DIV * (VCC / tension_flex - 1);
  }

  //Serial.print("Resistencia_Flex:"); Verification des donnees bt et du delay
  //Serial.println(resistance_flex);
  Bluetooth.print("F");
  Bluetooth.println(resistance_flex); //"F512"
  delay(50);  //Delay aussi du timer de lapp

  Serial.print("Resistencia_Graph:"); 
  Serial.println(resistance_graphite);

  Bluetooth.print("G");
  Bluetooth.println(resistance_graphite); //"G1023"
  delay(50);



  //if (Bluetooth.available()) {

    // Lit les données reçues depuis le module Bluetooth
   // char receivedChar = Bluetooth.read();
    // Affiche les données reçues sur le moniteur série
    //Serial.print(receivedChar);
 // }
  display.fillRect(10, 55, 100, 8, SSD1306_BLACK);
  display.setCursor(10, 55);
  display.print("Val : ");
  display.print(resistance_variable);
  display.display();

}


  
/*------------------------------- FONCTIONS UTILIES ------------------------------- */

// Fonction pour régler la valeur du potentiomètre

void setPotWiper(byte address, int value) {

  digitalWrite(csPin, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(csPin, HIGH);

}



void actionbouton(int seleccion) {

  display.clearDisplay();
  display.setCursor(20, 20);
  display.setTextSize(1);

  // Aquí defines qué hace cada opción del menú

  switch (seleccion) {

    case 0: // 500
      display.println(F("Résistance de: "));
      display.println(F("200 Ohms"));
      pos_res_variable = 1;
      setPotWiper(pot0Write, pos_res_variable);
      break;

    case 1: // 1k

      display.println(F("Résistance de: "));
      display.println(F("1000 Ohms"));
      pos_res_variable = 25;
      setPotWiper(pot0Write, pos_res_variable);
      break;

    case 2: // 5k

      display.println(F("Résistance de: "));
      display.println(F("5 kOhms"));
      pos_res_variable = 128;           //7500 x 255/10000 = 128 (pas de 10k pour le 41010)
      setPotWiper(pot0Write, pos_res_variable);
      break;

    case 3: // 10k

      display.println(F("Résistance de: "));
      display.println(F("7,5 kOhms"));
      pos_res_variable = 191;
      setPotWiper(pot0Write, pos_res_variable);
      break;

    case 4: // 100k

      display.println(F("Résistance de: "));
      display.println(F("10 kOhms"));
      pos_res_variable = 255;
      setPotWiper(pot0Write, pos_res_variable);
      break;


  }

  display.display();
  delay(2000); // El mensaje dura 2 segundos

}



void updateEncoder() {
  // Leemos el tiempo actual
  unsigned long interruptTime = millis();

  // Si han pasado más de 5 milisegundos desde el último giro, es un movimiento real
  if (interruptTime - lastInterruptTime > 5) {

    if (digitalRead(encoderPinB) != digitalRead(encoderPinA)) {
      encoderValue++;
    } else {
      encoderValue--;
    }

  }
  // Guardamos el tiempo de este movimiento
  lastInterruptTime = interruptTime;
}



void drawMenu() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  //display.println(F("Selectionner Val"));
  //display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

  for (int i = 0; i < totalItems; i++) {

    display.setCursor(0, 2 + (i * 9));
    if (i == currentSelection) {
      display.print(F("> "));
      display.print(menuItems[i]);
    } else {
      display.print(F("  "));
      display.print(menuItems[i]);
    }

  }

  display.display();
}