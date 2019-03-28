#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
#define TOKEN "pbhg89zrLOXGRqbFsT3S"
static const int RXPin = 15, TXPin = 14;
String hostPort = "*.*.*.*,*";  // 111.215.222.147,18777   udp服务器的ip地址和端口号
static const uint32_t GPSBaud = 9600;
#define ENA 13 //继电器连接在8266的GPIO12上
// The TinyGPS++ object
TinyGPSPlus gps;
LiquidCrystal_I2C lcd(0x27, 16, 2);
// The serial connection to the GPS device

String comdata = "";
String tindex = "";
int resets = 0;
void setup()
{
  Serial.begin(115200);
  Serial2.begin(GPSBaud);
  Serial3.begin(GPSBaud);
  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
  pinMode(ENA, OUTPUT); 
  digitalWrite(ENA, HIGH);
  lcd.init(); //Init with pin default ESP8266 or ARDUINO
  lcd.backlight();
  lcd.setCursor(0, 0);  
  lcd.print("Hello, UNO world!");
}

void loop()
{
    String address = "";
    String len = "";
    while (Serial3.available() > 0){
      if (gps.encode(Serial3.read())){
        address = emptyInfo();
        len = address.length();
        
        sendgps(address);
      }      
    }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }



  // This sketch displays information every time a new sentence is correctly encoded.
}

void sendgps(String str){
  delay(10000); 
  //Serial.println(str+"\n");
  String len = "";
  String addHex = "";

    //delay(2000);
  // put your main code here, to run repeatedly:
  if(comdata == ""){
    Serial2.write("AT+NCONFIG?\r\n");
  
    delay(3000);
    while(Serial2.available() > 0){
      comdata+= char(Serial2.read());
      delay(2);
    }
    Serial.println("111"); 
    Serial.println(comdata);  
    
    if(comdata.length() > 0 && comdata.indexOf("AUTOCONNECT")!=-1){    
      Serial.println("222"); 
      
      String drgam = "AT+NSOCR=DGRAM,17,10000,1\r\n";
      String retd = sendcmd(drgam);
      int index = retd.toInt();
      int flag = 0;
      while(retd.indexOf("ERROR")!=-1){
         retd = sendcmd(drgam);
         flag++;
         if(flag > 5) break;
      }
      
      if(retd.indexOf("ERROR")!=-1){
        String rese = "AT+NRB\r\n";
        String reres = sendcmd(rese);
        Serial.println(reres);  
        comdata = ""; 
      }else{

        while(retd.indexOf("OK")==-1){
         retd = sendcmd(drgam);
      }
          
      Serial.println("333"); 
      Serial.println(index); 
      len = str.length();
      addHex = str2HexStr(str);
      tindex = String(index);
      String cmd = "AT+NSOST="+tindex+","+hostPort+","+len+","+addHex+"\r\n";

      String ret = sendcmd(cmd);

      flag = 0;
      while(ret.indexOf("ERROR")!=-1){
         ret = sendcmd(cmd);
         flag++;
         if(flag > 5) break;
      }

      if(ret.indexOf("ERROR")!=-1){
        String rese = "AT+NRB\r\n";
        String reres = sendcmd(rese);
        Serial.println(reres);  
        comdata = ""; 
      }else{

          Serial.println("444"); 
        
          len = str.length();
          addHex = str2HexStr(str);
          String cmd1 = "AT+NSOST="+tindex+","+hostPort+","+len+","+addHex+"\r\n";
          Serial2.write(cmd1.c_str());
          delay(1000);
          Serial.println("555");
        }
      
        
      }  
      
 
      

    }else{
      if(comdata.indexOf("ERROR")!=-1){
        String rese = "AT+NRB\r\n";
        String reres = sendcmd(rese);
        Serial.println(reres);  
      }  
      comdata = "";      
    }
  }else{
      String controlCom = "";
      while(Serial2.available() > 0){
        controlCom+= char(Serial2.read());
        delay(2);
      }
      Serial.println("12ew232");
      Serial.println(controlCom);
      if(controlCom.indexOf("NSONMI")!=-1){
        Serial.println("NSONMI123456"); 
      }
      Serial2.flush();
      String backcmd = "AT+NSORF="+tindex+",22\r\n";
      String backinfo = "";
      backinfo = sendcmd(backcmd);
      Serial.println("xdew");
      Serial.println(backinfo);
      if(controlCom.indexOf("NSONMI")!=-1 && backinfo.indexOf("7B226D6574686F64223A22736574537461")!=-1){
        Serial.println("ddddddddddddddddddddddddddddddddddd"); 
      
        /*if(methodName == "setStatus"){
            Serial1.write(tempStr1.c_str());
        }else{
            String emptys = emptyInfo();
            Serial1.write(emptys.c_str());
        }*/
        str = displayInfo();
        lcd.clear();
        lcd.print(str);
        len = str.length();
        addHex = str2HexStr(str);
        String cmd = "AT+NSOST="+tindex+","+hostPort+","+len+","+addHex+"\r\n";
        //Serial.println(cmd); 
        Serial2.write(cmd.c_str());         
      }else{
        str = emptyInfo();
        len = str.length();
        addHex = str2HexStr(str);
        String cmd = "AT+NSOST="+tindex+","+hostPort+","+len+","+addHex+"\r\n";
        //Serial.println(cmd); 
        Serial2.write(cmd.c_str());  
      }
     
      
      //delay(1000);
      if(resets>250){
        comdata = "";
        resets = 0;
      } 
      Serial.println(resets); 
      resets++;
  }
  
}

char *strrpc(char *str,char *oldstr,char *newstr){
    char bstr[strlen(str)];//转换缓冲区
    memset(bstr,0,sizeof(bstr));
 
    for(int i = 0;i < strlen(str);i++){
        if(!strncmp(str+i,oldstr,strlen(oldstr))){//查找目标字符串
            strcat(bstr,newstr);
            i += strlen(oldstr) - 1;
        }else{
          strncat(bstr,str + i,1);//保存一字节进缓冲区
      }
    }
 
    strcpy(str,bstr);
    return str;
} 

String sendcmd(String cmd){
  Serial2.flush();
  Serial2.write(cmd.c_str());
  delay(3000);
  comdata = "";
  while(Serial2.available() > 0){
  comdata+= char(Serial2.read());
    delay(2);
  }
  //Serial.println(comdata);
  lcd.clear();
  lcd.print(comdata);
  return comdata;
}

String str2HexStr(String strin)
{
  int lenuse = strin.length();
  char charBuf[lenuse*2-1];
  char strBuf[lenuse*2-1];
  String strout = "";
  strin.toCharArray(charBuf,lenuse*2) ;
  for (int i = 0; i < lenuse; i++)
  {
    sprintf(strBuf, "%02X", charBuf[i]);

    if (String(strBuf) != F("00") )
    {
           strout += strBuf;
    }
  }

  return strout;
}

String emptyInfo(){
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
  data["token"] = TOKEN;
  data["index"] = 0;
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);  
  return strPayload;  
}

String displayInfo()
{
  String x = "";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();

  if (gps.location.isValid())
  {
    data["latitude"] = String(gps.location.lat(), 6);
    data["longitude"] = String(gps.location.lng(), 6);
  }
  else
  {
    data["latitude"] = "0";
    data["longitude"] = "0";    
  }
  
  if (gps.date.isValid())
  {
    x = x+ String(gps.date.month());
    x = x+ String(F("/"));
    x = x+ String(gps.date.day());
    x = x+ String(F("/"));
    x = x+ String(gps.date.year());
  }
  else
  {
    x = x+ String(F("INVALID"));   
  }

  x = x+ String(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) x = x+ (F("0"));
    x = x+ String(gps.time.hour());
    x = x+ String(F(":"));
    if (gps.time.minute() < 10) x = x+ (F("0"));
    x = x+ String(gps.time.minute());
    x = x+ String(F(":"));
    if (gps.time.second() < 10) x = x+ (F("0"));
    x = x+ String(gps.time.second());
    x = x+ String(F("."));
    if (gps.time.centisecond() < 10) x = x+ (F("0"));
    x = x+ String(gps.time.centisecond());

    data["time"] = x;
  }
  else
  {
    x = x+ String(F("INVALID"));
    data["time"] = "0";
  }
  data["token"] = TOKEN;
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);  
  return strPayload;
}
