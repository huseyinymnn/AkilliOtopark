/**********************************************************
 *      Nesnelerin İnterneti ve Uygulamaları Dersi
 *         Proje Uygulaması (Akıllı Otopark)
 *              Yazar : Hüseyin Yaman
 *********************************************************/

/* Kütüphane Dosyaları */
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

// ---------- Otopark Değişkenleri ve Pin Tanımları ------------//
#define trig1 D7
#define echo1 D8
#define trig2 D1
#define echo2 D2
#define led D3

/* Kablosuz Bağlantı Bilgileri */
const char* WLAN_SSID = "Asgarov";
const char* WLAN_PASSWORD = "nihat12345";
WiFiClient client;

/* ThingSpeak Kurulumu */
unsigned long channelID =2388816;
unsigned int field_no=1; 
const char* writeAPIKey = "BNMEZEICXC3D8EOH"; 
const char* thingSpeakHost = "api.thingspeak.com";

// ---------- Global Değişkenler ------------//
int maxMesafe= 5; 
int ArabaSayisi=0;
int maxArabaSayisi=10;

/*** ESP8266 WiFi Kurulum Fonksiyonu ***/
void wifiSetup()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Kablosuz Agina Baglaniyor");
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("Kablosuz Agina baglandi");
  }
}

/* ThingSpeak Field Yazma İşlemi **/
void upload()
{
  int x;
  wifiSetup();
  x=ThingSpeak.writeField(channelID,field_no,ArabaSayisi,writeAPIKey); 
  if(x==200){
    Serial.println("Veri güncellendi...");
  }
}

int mesafe(int maxMesafe,int trig,int echo)
{
  int sure,uzaklik;
  
  digitalWrite(trig,LOW);
  delayMicroseconds(5);
  digitalWrite(trig,HIGH);
  delayMicroseconds(15);
  digitalWrite(trig,LOW);

  sure=pulseIn(echo,HIGH);
  uzaklik= (sure/2)/29.1;
  delay(20);
  
  if(uzaklik>=maxMesafe)
  {
    return 0;
  }
  return uzaklik;
}

void setup() 
{
  Serial.begin(9600);
  wifiSetup();
  ThingSpeak.begin(client);
  pinMode(trig1,OUTPUT);
  pinMode(echo1,INPUT);
  pinMode(trig2,OUTPUT);
  pinMode(echo2,INPUT);
  pinMode(led,OUTPUT);
}

void loop() 
{
 int uzaklikGiris=mesafe(maxMesafe,trig1,echo1);
 int uzaklikCikis=mesafe(maxMesafe,trig2,echo2);
 
 if(uzaklikGiris > 0)
 {
     ArabaSayisi++;
     Serial.print("Uzaklık(cm): ");
     Serial.print(uzaklikGiris);
     Serial.print("  Yeni bir araba giriş yaptı. Toplam araba sayısı: ");
     Serial.println(ArabaSayisi);
     digitalWrite(led,HIGH);
     delay(10);
     digitalWrite(led,LOW);
  }

  if(uzaklikCikis > 0)
  {
     ArabaSayisi--;
     Serial.print("Uzaklık(cm): ");
     Serial.print(uzaklikCikis);
     Serial.print("  Bir araba çıkış yaptı. Toplam araba sayısı: ");
     Serial.println(ArabaSayisi);
     digitalWrite(led,HIGH);
     delay(10);
     digitalWrite(led,LOW);
  }
    
  if(ArabaSayisi>= maxArabaSayisi)
  {
     Serial.println("Otopark dolu! Lütfen başka bir yer deneyin.");
     digitalWrite(led,HIGH);
  }
  else
  {
     digitalWrite(led,LOW);
  }
  
  upload();
  delay(500);
}