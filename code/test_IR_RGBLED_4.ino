//Blynk
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_TEMPLATE_ID "" //reemplazar credenciales de blynk
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

char ssid[] = ""; //Reemplazar credenciales de wifi
char pass[] = "";

BlynkTimer timer; // Crea un objeto timer

#define REDPIN 2    //pines de la esp32 utilizados
#define GREENPIN 15
#define BLUEPIN 0

TaskHandle_t Core0taskHandle;
TaskHandle_t Core1taskHandle;

int redValue = 0;
int greenValue = 0;
int blueValue = 0;
int smoothValue = 0;
int flashValue = 0;

#define SMOOTHSPEED 20     // make this higher to slow down
#define FLASHSPEED 800     // make this higher to slow down

void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  xTaskCreatePinnedToCore(task0, "Task0", 4096, NULL, 1, &Core0taskHandle, 0);
  xTaskCreatePinnedToCore(task1, "Task1", 4096, NULL, 1, &Core1taskHandle, 1);
}

void loop() {
  Blynk.run();
}

void task0(void * parameter) {
  for (;;) {
    Blynk.run(); // Esto permite que Blynk procese las tareas entrantes
    // Aquí va el código para recibir señales de internet y de un sensor
    delay(500);
  }
}

void task1(void * parameter) {
  for (;;) {
    // Aquí va el código para enviar señales a una tira de luces LED
    while (smoothValue){
      Serial.println("Estoy correctamente en loop smooth");
      
      int r, g, b;
      // fade from blue to violet
      for (r = 0; r < 256; r++) {
        if(smoothValue == 0){
          break;}
        analogWrite(REDPIN, r);
        delay(SMOOTHSPEED);
        } 
      // fade from violet to red
      for (b = 255; b > 0; b--) {
        if(smoothValue == 0){
          break;}
        analogWrite(BLUEPIN, b);
        delay(SMOOTHSPEED);
        } 
      // fade from red to yellow
      for (g = 0; g < 256; g++) {
        if(smoothValue == 0){
          break;}
        analogWrite(GREENPIN, g);
        delay(SMOOTHSPEED);
        } 
      // fade from yellow to green
      for (r = 255; r > 0; r--) {
        if(smoothValue == 0){
          break;}
        analogWrite(REDPIN, r);
        delay(SMOOTHSPEED);
        } 
      // fade from green to teal
      for (b = 0; b < 256; b++) {
        if(smoothValue == 0){
          break;}
        analogWrite(BLUEPIN, b);
        delay(SMOOTHSPEED);
        } 
      // fade from teal to blue
      for (g = 255; g > 0; g--) {
        if(smoothValue == 0){
          break;}
        analogWrite(GREENPIN, g);
        delay(SMOOTHSPEED);
        }
      }
      
    while (flashValue){
      Serial.println("Estoy correctamente en loop flash");

      analogWrite(REDPIN, 255); //rojo
      analogWrite(GREENPIN, 0);
      analogWrite(BLUEPIN, 0);
      delay(FLASHSPEED);
      if(flashValue == 0){
        break;}
      analogWrite(REDPIN, 255); //magenta
      analogWrite(GREENPIN, 0);
      analogWrite(BLUEPIN, 255);
      delay(FLASHSPEED);
      if(flashValue == 0){
        break;}
      analogWrite(REDPIN, 255); //amarillo
      analogWrite(GREENPIN, 255);
      analogWrite(BLUEPIN, 0);
      delay(FLASHSPEED);
      if(flashValue == 0){
        break;}
      analogWrite(REDPIN, 0); //cian
      analogWrite(GREENPIN, 255);
      analogWrite(BLUEPIN, 255);
      delay(FLASHSPEED);
      if(flashValue == 0){
        break;}      
      analogWrite(REDPIN, 0); //verde
      analogWrite(GREENPIN, 255);
      analogWrite(BLUEPIN, 0);
      delay(FLASHSPEED);
      if(flashValue == 0){
        break;}      
      analogWrite(REDPIN, 0); //azul
      analogWrite(GREENPIN, 0);
      analogWrite(BLUEPIN, 255);
      delay(FLASHSPEED);
      if(flashValue == 0){
        break;}      
      analogWrite(REDPIN, 0); //cian
      analogWrite(GREENPIN, 255);
      analogWrite(BLUEPIN, 255);
      delay(FLASHSPEED);
      if(flashValue == 0){
        break;}
    }

    delay(600);
  }
}


BLYNK_WRITE(V0){ //botón RED
    //apagamos funcion en caso de que este activada  
    if (smoothValue == 1 || flashValue == 1){  
      stopSmoothFlash();}

    redValue = param.asInt(); // Obtén la posición del slider V0
    analogWrite(REDPIN, redValue);
    Serial.print("the value of Red = ");
    Serial.println(redValue);
  }
BLYNK_WRITE(V1){ //botón GREEN
    //apagamos funcion en caso de que este activada
    if (smoothValue == 1 || flashValue == 1){  
      stopSmoothFlash();}
    
    greenValue = param.asInt(); // Obtén la posición del slider V1
    analogWrite(GREENPIN, greenValue);
    Serial.print("the value of Green = ");
    Serial.println(greenValue);
  }
BLYNK_WRITE(V2){ //botón BLUE
    //apagamos funcion en caso de que este activada
    if (smoothValue == 1 || flashValue == 1){  
      stopSmoothFlash();}
    
    blueValue = param.asInt(); // Obtén la posición del slider V2
    analogWrite(BLUEPIN, blueValue);
    Serial.print("the value of Blue = ");
    Serial.println(blueValue);
  }
BLYNK_WRITE(V3){ //slide BRIGHT
    //apagamos funcion en caso de que este activada
    stopSmoothFlash();

    int brightness = param.asInt(); // Obtén la posición del slider V3
    Serial.print("the value of Bright = ");
    Serial.println(brightness);
    //escribimos los valores de los pines
    analogWrite(REDPIN, redValue * brightness / 255);
    analogWrite(GREENPIN, greenValue * brightness / 255);
    analogWrite(BLUEPIN, blueValue * brightness / 255);
    // Actualiza los sliders de colores en la aplicación Blynk
    Blynk.virtualWrite(V0, redValue * brightness / 255);
    Blynk.virtualWrite(V1, greenValue * brightness / 255);
    Blynk.virtualWrite(V2, blueValue * brightness / 255);
  }
BLYNK_WRITE(V4){ //boton SMOOTH
    //apagamos funcion en caso de que este activada
    flashValue = 0;
    Blynk.virtualWrite(V5,flashValue);
    
    smoothValue = param.asInt(); // Obtén el valor del boton V4
    Serial.print("the value of Smooth = ");
    Serial.println(smoothValue);
  }
BLYNK_WRITE(V5){ //boton Flash
    //apagamos funcion en caso de que este activada
    smoothValue = 0;
    Blynk.virtualWrite(V4,smoothValue);

    flashValue = param.asInt(); // Obtén el valor del boton V5
    Serial.print("the value of Flash = ");
    Serial.println(flashValue);
  }
BLYNK_WRITE(V6){ //ON OFF
    stopSmoothFlash();
    
    int value = param.asInt(); // Obtén el valor del boton V6
    Serial.print("the value of On/Off = ");
    Serial.println(value);
    if (value == 1){
      //Actualizamos las variables
      redValue = 60;
      greenValue = 20;
      blueValue = 0;
    }else{
      //Actualizamos las variables
      redValue = 0;
      greenValue = 0;
      blueValue = 0;
      }
    //escribimos los valores de los pines al encender en amarillo suave
    analogWrite(REDPIN, redValue);
    analogWrite(GREENPIN, greenValue);
    analogWrite(BLUEPIN, blueValue);
    // Actualiza los sliders de colores en la aplicación Blynk
    Blynk.virtualWrite(V0, redValue);
    Blynk.virtualWrite(V1, greenValue);
    Blynk.virtualWrite(V2, blueValue);
  }

void stopSmoothFlash(){
    //apagamos funciones smooth/flash en caso de que este activa
    smoothValue = 0;
    flashValue = 0;
    Blynk.virtualWrite(V4, smoothValue);
    Blynk.virtualWrite(V5, flashValue);
    delay(400);
}
