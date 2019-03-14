#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8); // RX, TX

String data = "";
double Lat,Long;

void setup() {
  Serial.begin(4800);
  while (!Serial) {
    ; 
  }
  mySerial.begin(4800);
  delay(1000);
  
  mySerial.println("ATE0");  //Desactivamos el eco de comandos
  limpiarSerial();
  mySerial.println("AT+CGPS=1,1");  //Activamos el GPS
  limpiarSerial();
  mySerial.println("AT+CGPSHOR=50");  //Fijamos precision de 50 metros
  limpiarSerial();
  delay(30000); //Esperamos 30 segundos para esperar que el GPS tenga una localizacion precisa
}

void loop() {

  leerGPS(&Lat, &Long);
  Serial.println(Lat,4);
  Serial.println(Long,4);

}
//****Envia  coamando y retorna la respuesta***//
String sendRetornoResp(String ATcommand, byte intentos)
{
  byte contadorDeIntentos = 0;
  data = "";
  limpiarSerial();
  mySerial.println(ATcommand);
  mySerial.flush();
  data = mySerial.readStringUntil('\n');
  while (data.length() < 3 && contadorDeIntentos < intentos)
  {
    data = mySerial.readStringUntil('\n');
    contadorDeIntentos++;
  }
  limpiarSerial();
  return data;
}

//****Realiza procesamiento de datos del GPS****//
void leerGPS(double *latitud, double *longitud)
{
  *latitud = 0.0;
  *longitud = 0.0;
  //limpiarSerial();
  String campo[13];
  String dataString = sendRetornoResp(F("AT+CGPSINFO"), 5); 
  Serial.println(dataString);
  dataString.replace("+CGPSINFO: ", "");
  dataString.replace("\r", "");
  if (!dataString.equals("+CGPSINFO: ,,,,,,,,"))
  {
    int ant = 0;
    int pos = 0;
    for (int i = 0; i < 8; i++)
    {
      pos = dataString.indexOf(",", ant + 1);
      campo[i] = dataString.substring(ant, pos);
      ant = pos + 1;
    }
    *latitud = decNMEA(campo[0], campo[1]);
    *longitud = decNMEA(campo[2], campo[3]);
  }
}

//****Realiza correccion de latitud y longitud de coordenadas GPS****//
double decNMEA(String l, String h)
{
  int a = int(l.toFloat()) / 100;
  double b = (l.toFloat() - (a * 100)) / 60;

  if (h.equals("W") || h.equals("S"))
  {
    return (a + b) * -1;
  }
  else
  {
    return a + b;
  }
}
//****Vacia el buffer de entrada de SoftwareSerial****//
void limpiarSerial(void)
{
  uint16_t timeoutloop = 0;
  while (timeoutloop++ < 120)
  {
    while (mySerial.available())
    {
      mySerial.read();
      timeoutloop = 0; // If char was received reset the timer
    }
    delay(1);
  }
}
