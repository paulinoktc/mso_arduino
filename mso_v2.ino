#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Sensor de temperatura
#include <OneWire.h>
#include <DallasTemperature.h>

// Display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//acelerometro
#include <math.h>

// Define los pines de los botones y el bus I2C según el tipo de placa
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
  #define WIRE Wire
#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32C6)
  #define BUTTON_A  7
  #define BUTTON_B  6
  #define BUTTON_C  5
  #define WIRE Wire
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
  #define WIRE Wire
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
  #define WIRE Wire
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
  #define WIRE Wire
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
  #define WIRE Wire
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
  #define BUTTON_A  9
  #define BUTTON_B  8
  #define BUTTON_C  7
  #define WIRE Wire1
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
  #define WIRE Wire
#endif

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);

const int oneWireBus = 4; // Pin temperatura
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

int arrayVibrate[20];
int hzs = 0;
int suma = 0;
int Pin_Sensor = 5;
int Pin_led = 2;

String serverUrl = "http://192.168.8.70/serverx.php";
float temperatureC=35;
float tmax=35;
float hzmax=50;

double roll, pitch, yaw;
const int x_out = A0; /* connect x_out of module to A1 of UNO board */
const int y_out = A3; /* connect y_out of module to A2 of UNO board */
const int z_out = A4; /* connect z_out of module to A3 of UNO board */

WebServer server(80);
String server_l="";
String valor2="";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  //const char* ssid = "MERCUSYS_5EBE";         /*Enter Your SSID*/
  //const char* password = "57931289"; /*Enter Your Password*/

  String ssid = "RAZOR";         /*Enter Your SSID*/
  String password = "12345678z"; /*Enter Your Password*/
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.display();

  dysplayInit();
  Serial.println("Desconectamos antes de conectar el WiFi");
  WiFi.disconnect();
/*

  // Leer el nombre del WiFi
  Serial.println("Escribe Nombre WiFi:");
  while (Serial.available() == 0) {}  // Espera hasta que haya datos disponibles
  String tx_ssid = Serial.readString();  // Lee el texto ingresado
  tx_ssid.trim();  // Elimina espacios y saltos de línea
  ssid = tx_ssid;  // Asigna el texto leído a la variable 'ssid'
  
  // Leer la contraseña del WiFi
  Serial.println("Escribe la contraseña del WiFi:");
  while (Serial.available() == 0) {}  // Espera hasta que haya datos disponibles
  String tx_password = Serial.readString();  // Lee el texto ingresado
  tx_password.trim();  // Elimina espacios y saltos de línea
  password = tx_password;  // Asigna el texto leído a la variable 'password'
  
  // Imprimir los valores ingresados para confirmar
  Serial.println("Nombre WiFi ingresado: " + ssid);
  Serial.println("Contraseña WiFi ingresada: " + password);
  
*/


  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    String content = "";
    content += "<html>";
    content += "<body>";
    content += "<h1>BIENVENIDO A MEMOSC</h1>";
    content += "<form action=\"/submit\" method=\"POST\">";
    
    content += "IP Servidor : <input type=\"text\" name=\"valor1\"><br>";
    content += "Temperatura maxima : <input type=\"number\" name=\"valor2\" value=\""+String(tmax)+"\"><br>";
    content += "Hz maxima : <input type=\"number\" name=\"valor3\" value=\""+String(hzmax)+"\"><br>";

    content += "<input type=\"submit\" value=\"Enviar\">";
    content += "</form>";
    content += "</body>";
    content += "</html>";
    server.send(200, "text/html", content);
  });

    // Ruta para procesar los datos del formulario
  server.on("/submit", HTTP_POST, []() {
    if (server.hasArg("valor1") && server.hasArg("valor2")) {
      // Almacenar los valores en las variables globales
      server_l = server.arg("valor1");
      server_l="http://"+server_l+"/serverx.php";
      tmax = server.arg("valor2").toFloat();
      hzmax=server.arg("valor3").toFloat();

      // Imprimir los valores recibidos
      Serial.println("Valor 1: " + server_l);
      //Serial.println("Valor 2: " + valor2);

      // Respuesta al cliente
      server.send(200, "text/html", "Datos recibidos: Valor 1 = " + server_l + " Valor 2 = "+tmax+" Valor3="+hzmax);// + valor2);
    } else {
      server.send(400, "text/html", "Faltan parámetros");
    }
  });

  // Iniciar el servidor
  server.begin();


  pinMode(Pin_Sensor, INPUT);
  pinMode(Pin_led, OUTPUT);
  
}


void displayInfo(float temperatureC, int suma) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("..M E M O S C..");
  display.setCursor(0, 8);
  display.print(suma);
  display.print(" Hz");
  display.setCursor(0, 16);
  display.print(temperatureC);
  display.print(" C");
  display.setCursor(0, 24);
  display.print(WiFi.localIP());

  display.display();
}


void dysplayInit() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("..M E M O S C..");
  display.setCursor(0, 8);
  display.print("Conectando....");
  display.display();
}



void loop() {
  // put your main code here, to run repeatedly:
  int x_adc_value, y_adc_value, z_adc_value; 
  double x_g_value, y_g_value, z_g_value;

  x_adc_value = analogRead(x_out); /* Digital value of voltage on x_out pin */ 
  y_adc_value = analogRead(y_out); /* Digital value of voltage on y_out pin */ 
  z_adc_value = analogRead(z_out); /* Digital value of voltage on z_out pin */ 
  Serial.print("x = ");
  Serial.print(x_adc_value);
  Serial.print("\t\t");
  Serial.print("y = ");
  Serial.print(y_adc_value);
  Serial.print("\t\t");
  Serial.print("z = ");
  Serial.print(z_adc_value);
  Serial.println("");
 
  x_g_value = ( ( ( (double)(x_adc_value * 3.3)/4096) - 1.65 ) / 0.330 ); /* Acceleration in x-direction in g units */ 
  y_g_value = ( ( ( (double)(y_adc_value * 3.3)/4096) - 1.65 ) / 0.330 ); /* Acceleration in y-direction in g units */ 
  z_g_value = ( ( ( (double)(z_adc_value * 3.3)/4096) - 1.80 ) / 0.330 ); /* Acceleration in z-direction in g units */ 

  roll = ( ( (atan2(y_g_value,z_g_value) * 180) / 3.14 ) + 180 ); /* Formula for roll */
  pitch = ( ( (atan2(z_g_value,x_g_value) * 180) / 3.14 ) + 180 ); /* Formula for pitch */
  yaw = ( ( (atan2(x_g_value,y_g_value) * 180) / 3.14 ) + 180 ); /* Formula for yaw */
  /* Not possible to measure yaw using accelerometer. Gyroscope must be used if yaw is also required */

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("Roll = ");
  Serial.print(roll);
  Serial.print("\t");
  Serial.print("Pitch = ");
  Serial.print(pitch);
  Serial.print("\t");
  Serial.print("Yaw = ");
  Serial.print(yaw);
  Serial.print("\n\n");
  
  //sensor de temperatura
  sensors.begin();
  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("C");


    if(yaw>=hzmax || temperatureC>=tmax){
      digitalWrite(Pin_led, HIGH);
    }else{
      digitalWrite(Pin_led, LOW);
    }
  displayInfo(temperatureC, yaw);
  
  //coneectando con servidor
  String data = "sensor=1&valor=" + String(temperatureC) + "&sensor2=2&valor2=" + String(yaw);
  HTTPClient http;
    http.begin(server_l);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(data);
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error en la solicitud HTTP: ");
      Serial.println(httpResponseCode);
    }

    http.end();

    server.handleClient();

    delay(100);
    yield();


}
