# Project-ESP32-Blynk-RGB-LED-strip
Proyecto personal para controlar vía wifi desde cualquier sitio una tira de lueces LED RGB de 4 pines mediante una placa esp32 y Blynk.

## Para el proyecto se requiere
- Tira luces Led RGB 4 pines
- Placa Esp32
- Regulador de voltaje step down DC-DC LM2596
- 3 mosfets IRFZ44N o similar
- Transformador 12V-5A o más (opcional pero recomendable dado el consumo de corriente del regulador y la placa)

## Contexto
Hace tiempo venía dándole vueltas a cómo manejar desde el celular una [tira de luces led RGB 5050 con 4 pines](https://www.amazon.com/-/es/SUPERNIGHT-24-Key-Control-remoto-controlador/dp/B01K7G96E2?th=1) que se manejaba a través de señales IR con un botón de 24 botones.

En lo personal elegí [Blynk](https://blynk.io/) para operar a través de internet dada la familiariedad y simplicidad de su uso, además que ofrece opciones multiplataforma. Sé que existen alternativas mediante ESPHome o páginas web con servidores locales, pero en este caso Blynk cumple perfectamente con el propósito buscado.

El circuito original de las luces LED a partir de ingeniería reversa fue sacado de [acá principalmente](https://www.hackster.io/ashish_8284/making-smart-led-strip-from-boring-ir-controller-led-strip-22b0f9). En la foto se vería así


<img src="img\front_circuit.png" alt="Circuito original front" width="420"/>
<img src="img\back_circuit.png" alt="Circuito original back" width="300"/>

## Control mediante señales IR hacia el chip de la tira de luces (opción descartada)

Hay muchas formas de pelar un conejo, de la misma forma en la que uno puede elegir cómo y dónde conectar el chip para manejar las luces. En un principio quise que la ESP32 emulara el envío de señales infrarrojas del sensor infrarrojo hacia el chip que venía por defecto en la placa de la tira de luces led, pero surgieron un par de problemas debido al diodo Zenner, que, al menos en mi caso, me generaba mediciones negativas de voltaje que la placa y el regulador no podían manejar, así que se descartó esta opción.

De todas formas, si te interesa explorar la alternativa de señales infrarrojas y su codificación/decodificación, el protocolo de comunicación IR de las luces led resultó ser "NEC". Hay varios foros con tutoriales y códigos que te permiten decodificar las señales enviadas desde el control con librerías como **IRreciv**. Mediante ensayo y error, estos fueron los comandos del control decodificados en formato HEX. Recomiendo verificar y probar por tu cuenta si te interesa decodificar e implementar de esta manera las señales.

**Códigos hexadecimales "0x":**

```  
On CE1972FD
Off D4DD0381
Brillo up 8503705D
Brillo down DEB0C861
Flash DCC45BE1
Strobe 374E8B9D
Fade B9C07541
Smooth A7315F7D

Red E85952E1
R2 D3FD9A81
R3 84044BBD
R4 B0F9B3E1
R5 9DE75E1D

Green 78CDA4DD
G2 6471EC7D
G3 14789DB9
G4 416E05DD
G5 F794B621

Blue A2672345
B2 9D52009D
B3 3E121C21
B4 6A844445
B5 57F52E81
```

## Control mediante wifi y Blynk

### Hardware
En cuanto al hardware, se **reemplazó el transformador** de las luces JSY-1230 de 12V 3A por uno de mejor calidad y más potente de 12V y 5A (comprado en un mall chino) ya que el original no era suficientemente estable y generaba caídas de voltaje abruptas de más de 4 voltios que apagarían y reiniciarían la placa constantemente.

Para alimentar y proteger la placa **se alimentó con un regulador de voltaje DC-DC LM2596**, que se alimenta directamente de la fuente del transformador y su salida queda regulada a 5.4V (añadí 0.4V de margen en caso de caídas leves de voltaje, ya que de lo contrario, no me funcionaba correctamente después de cierto tiempo), conectada a los pines de 5V y GND de la ESP32. Existen otros reguladores como el LM7805 pero se calentaban más de la cuenta.

#### Conexión Esp32 y luces led: Driver, BJT o Mosfets?

Para las conexiones de los cables RGB entre la placa ESP32 y la tria de luces LED, es necesario añadir componentes a manera de "válvula" o "compuerta" para su protección. Acá se mencionan tres formas de hacerlo que no afectan al resto de componentes ni al código, donde la más eficiente y que se calienta menos es a través de **Mosfets tipo N**.

Toma la que se ajuste mejor a tus necesidades y elementos a mano.

**1. Driver:**

La primera forma utiliza una configuración tipo Darlington en un único componente, simplificando bastante el circuito con el driver ULN2003, el cual saqué de un motor paso a paso que no estaba utilizando. Su implementación es similar a [este proyecto donde utiliza el sensor IR](https://www.engineersgarage.com/how-to-build-an-ir-remote-operated-rgb-led-strip-using-arduino/). El único problema de este método es que se calienta demasiado, al punto de no poder tocar el driver en pleno funcionamiento, sin embargo, no afecta al rendimiento general, por lo que si no te preocupa la alta temperatura, es una opción bastante cómoda.

El esquema y circuito físico se ven así:

<img src="img\circuit_diagram_darlington.png" alt="Diagrama del circuito" width="600"/>
<img src="img\real_circuit_darlington.jpg" alt="Foto final del hardware" width="316"/>

**2. BJT:**

La segunda forma, que también se calentaba bastante, aunque no pareciera superar los 80°C en máximo funcionamiento, son los BJT [TIP120 conectados con resistencias](https://learn.adafruit.com/rgb-led-strips/usage), los que requieren una resistencia de 10k base desde la placa ESP32 para operar de manera correcta. Obtenemos la ventaja de operar a una temperatura ligeramente menor, pero hace un poco más engorroso el circuito al tener las resistencias más conexiones adicionales.

<img src="img\circuit_diagram_bjt.png" alt="Diagrama del circuito" width="600"/>

**3. Mosfet:**

Finalmente, la tercera forma (y mejor candidata), es utilizar Mosfets tipo N. En mi caso tenía a mano los IRFZ44N que cumplieron perfectamente sin siquiera llegar a calentarse en su punto de máxima operación. Se conecta igual que los TIP120, solo que no requieren resistencias adicionales y se puede conectar directamente a la placa ESP32. 

Por seguridad y eficiencia térmica, esta fue la opción final elegida, además que a la vista, pareciera que al consumir menos corriente, las luces led brillan con más fuerza en comparación a las dos opciones anteriores. El esquema y circuito físico se ven así:

<img src="img\circuit_diagram_mosfet_final.png" alt="Diagrama del circuito" width="600"/>
<img src="img\real_circuit_mosfet.jpg" alt="Foto final del hardware" width="316"/>


### Software: Explicación del código general

El código fue de elaboración propia asistido por Copilot, buscando implementar las mismas funciones del control IR a través de la nube. Lo puedes encontrar completo en archivo del repositorio, así que solo detallaré algunos elementos relevantes de su operación.

En cuanto a las funcionalidades, **descarté los modos de *Fade* y *Strobe*** debido a que se diferencian muy poco de *Smooth* y *Flash* respectivamente, por lo que solo se dejaron estas dos útlimas.


Aprovechando los [dos núcleos de la placa ESP32](https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/), se utilizó el núcleo 0 para las funciones wifi de Blynk, y el núcleo 1 (usualmente utilizado por defecto), para las tareas de envío de señales repetitivas a las luces, como lo son las funciones *Smooth* y *Flash*. A continuación se ven los *TaskHandler* del software **FreeRTOS** que ya viene de manera nativa en las ESP32.

```
TaskHandle_t Core0taskHandle;
TaskHandle_t Core1taskHandle;

void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  xTaskCreatePinnedToCore(task0, "Task0", 4096, NULL, 1, &Core0taskHandle, 0); //Seteamos la tarea al core 0
  xTaskCreatePinnedToCore(task1, "Task1", 4096, NULL, 1, &Core1taskHandle, 1); //Seteamos la tarea al core 1
}
```

La **memoria dediada a cada tarea fue puesta en *4096*** para evitar problemas y gatillar [este tipo de errores](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/fatal-errors.html). Si te surgen, te recomiendo ajustarla según tus necesidades.

Las funciones *smooth* y *flash* son implementadas para estar en un bucle `while(variable)` en el loop de el core 1 *task1*. Su funcionamiento está basado en [el siguiente código](https://learn.adafruit.com/rgb-led-strips/arduino-code).

```
    while (smoothValue){
      Serial.println("Estoy correctamente en loop smooth");
      
      int r, g, b;
      // fade from blue to violet
      for (r = 0; r < 256; r++) {
        if(smoothValue == 0){
          break;}
        analogWrite(REDPIN, r);
        delay(SMOOTHSPEED);
        ...
      }}
```

Acá se transiciona de un color a otro aumentando la **magnitud de un valor a la vez, entre 0 a 255**. Por cada bucle verifica si la variable `smoothValue` cambió de valor entre 0 y 1 debido a alguna señal recibida de Blynk y procesada por el core 0 *task0*. Luego la función *flash* opera de la misma manera pero manteniendo un color fijo durante un tiempo `FLASHSPEED` en vez de `SMOOTHSPEED`.

### Explicación del código de Blynk

Se utilizaron 7 pines digitales (del 0 al 6): los colores RGB por separado, la iluminación, las funciones smooth y flash, además del encendido/apagado. A continuación se ve el código del **Pin RED**.

```
BLYNK_WRITE(V0){ //botón RED
    //apagamos funcion en caso de que este activada  
    if (smoothValue == 1 || flashValue == 1){  
      stopSmoothFlash();}

    redValue = param.asInt(); // Obtén la posición del slider V0
    analogWrite(REDPIN, redValue);
    Serial.print("the value of Red = ");
    Serial.println(redValue);
  }
```

Para evitar conflicto, al apretar un botón se llama a `stopSmoothFlash()`, función que setea los valores de `smoothValue` y `flashValue` en 0 para detener el posible bulce `while` en el core 1 y generar conflicto para finalmente fijar un color.

En el caso de los pines digitales de las funciones **Smooth (V4)** y **Flash (V5)** al detectar una señal de blynk actúan de manera similar que los slider previos, pero dando pie a iniciar el bucle `while` correspondiente en el core 1.

```
BLYNK_WRITE(V4){ //boton SMOOTH
    //apagamos funcion en caso de que este activada
    flashValue = 0;
    Blynk.virtualWrite(V5,flashValue);
    
    smoothValue = param.asInt(); // Obtén el valor del boton V4
    Serial.print("the value of Smooth = ");
    Serial.println(smoothValue);
  }
```

Finalmente, es de notar que el color de encendido se eligió de manera arbitraria.

```
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
  ```

## Configuración de la plataforma Blynk

En la plataforma *Blynk Cloud*, se creó un *template* para el proyecto con 6 datastreams. Para los **colores RGB y brillo** se utilizaron sliders de valores `int` **entre 0 y 255**. El resto son botones de valores `int` entre 0 y 1. El dahsboard queda de la siguiente manera:

<img src="img\web_dahsboard_blynk.png" alt="Wen Dashboard Blynk" width="400"/>


**Recuerda reemplazar las credenciales de Blynk, nombre y contraseña de wifi al código en el idle de Arduino, además de configurar el puerto y el tipo de placa.**

Luego en la app de Blynk, aparece automáticamente el template creado. Allí se añaden los mismos elementos que el dashboard web, sumando además el color picker de Zegbra, con cada datastream asociado a los slider R, G y B en los pines V0, V1 y V2 respectivamente.

<img src="img\color_picker_Zegbra.jpg" alt="Foto color picker zegbra" width="300"/>


Ya con todo configurado, a disfrutar!


## Consideraciones finales

Es posible optimizar y pulir varios aspectos, en general estuve leyendo que el uso de `delay()` no es completamente "ideal", ya que existen otras alterantivas y timer, sobretodo considerando su uso con Blynk.

A futuro quizás evaluar añadir el sensor infrarrojo a la placa ESP32, para que funcione a su vez por wifi y por señales IR.

Este proyecto cumplió su finalidad al realizarse por mero hobby!