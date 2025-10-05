#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5); //Rx-D1,Tx-D2

const char* ssid = "prateeksingh";
const char* password = "kumar@12345";

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;


WiFiServer server(80);
void setup()
{
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println();
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
}

void loop()
{
  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 6);
      }

      if (gps.date.isValid())
      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
        DateString = '0';
        DateString += String(date);

        DateString += " / ";

        if (month < 10)
        DateString += '0';
        DateString += String(month);
        DateString += " / ";

        if (year < 10)
        DateString += '0';
        DateString += String(year);
      }

      if (gps.time.isValid())
      {
        TimeString = "";
        hour = gps.time.hour()+ 5; //adjust UTC
        minute = gps.time.minute();
        second = gps.time.second();
    
        if (hour < 10)
        TimeString = '0';
        TimeString += String(hour);
        TimeString += " : ";

        if (minute < 10)
        TimeString += '0';
        TimeString += String(minute);
        TimeString += " : ";

        if (second < 10)
        TimeString += '0';
        TimeString += String(second);
      }

    }
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  //Response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>NEO-6M GPS Readings</title> <style>";
  s += "table, th, td {border: 1px solid blue;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER>NEO-6M GPS Readings</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += LatitudeString;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += LongitudeString;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += DateString;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += TimeString;
  s += "</td>  </tr> </table> ";
 
  
  if (gps.location.isValid())
  {
    s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += LatitudeString;
    s += "+";
    s += LongitudeString;
    s += """ target=""_top"">Click here</a> to open the location in Google Maps.</p>";
  }

  s += "</body> </html> \n";

  client.print(s);
  delay(100);

}
