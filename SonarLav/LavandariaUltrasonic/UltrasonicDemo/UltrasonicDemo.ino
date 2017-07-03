#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <Ultrasonic.h>
//#include "DHT.h"

#define TRIGGER_PIN  12
#define ECHO_PIN     13

Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);

int bomba1= 7; //Bomba nos lavados
int bomba2= 6; //Bomba do poço
int led_alarme = 9;
int led_bomba1= 10;
int led_bomba2= 11;
int interruptor_bomba2=3;
char str1[4]=""; 
char str2[4]="";

//DHT dht;

void setup()
{
  Serial.begin(9600);
  lcd.begin ();
  lcd.backlight();
  pinMode (7, OUTPUT);
  pinMode (6, OUTPUT);
  pinMode (9, OUTPUT);
  pinMode (10, OUTPUT);
  pinMode (11, OUTPUT);
  pinMode (3, OUTPUT),
  digitalWrite(bomba1, LOW);
  digitalWrite (led_bomba1, LOW);
  digitalWrite(bomba2, LOW);
  digitalWrite (led_bomba2, LOW);
  digitalWrite (led_alarme, LOW);
  //dht.setup(2); // Ligaçao ao pino 2 digital

}

void loop()
{
  int TanquePequenoVazio=123;//altura em cm do tanque dado pelo o sensor ultra som
  float altura_actual=0.0; //medicao actual dada pelo sonar
  long microsec = ultrasonic.timing();
  int percentagem=0;//percentagem de agua no tanque
  
  //float humidity = dht.getHumidity(); //Humidade dada pela funcao da biblioteca  
  //float temperature = dht.getTemperature();//Temperatura em C dada pela funcao da biblioteca
  
  altura_actual = ultrasonic.convert(microsec, Ultrasonic::CM); 
  altura_actual=altura_actual-12;//ajustes da mediçao devido a alguma perda de sinal ao londo do cabo
  
  percentagem=medicao_volume_tanque_pequeno(altura_actual, TanquePequenoVazio);
  
  LCD(percentagem,str1,str2);
  
  Bombas_ON_OFF (percentagem,bomba1,bomba2,TanquePequenoVazio,str1,str2);
  
  delay(250);
}


/** Calcular volume do tanque pequeno, retornando a percentagem actual do Volume */

int medicao_volume_tanque_pequeno(float altura_actual, int TanqueVazio) {

  int percentagem_volume= 100-((altura_actual*100)/TanqueVazio);
  if (percentagem_volume < 0) {
    percentagem_volume= 0;
  }
  if (percentagem_volume > 100) {
    percentagem_volume= 100;
  }

  return percentagem_volume;
}


/** De acordo com os valores da percentagem do volume, realizar a activaçao ou desactivaçao das bombas */

void Bombas_ON_OFF (int percentagem, int bomba1, int bomba2, int TanquePequenoVazio,char str1[], char str2[]) {

int percent=0;//variavel utilizada para calcular percentagem de volume quando este se encontra inferior a 10%
float humidade = 0.0;
float temperatura=0.0;
long microsec=0;
float altura = 0.0;

digitalWrite(led_alarme, LOW);

  /** Depois de o volume ser inferior a 10% a bomba1 encontra se desligada e so volta a ligar quando a % for igual ou seperior a 50%*/
 
  if (percentagem < 10) {
    do{
      
      //float humidade = dht.getHumidity();   
      //float temperatura = dht.getTemperature();
      long microsec=ultrasonic.timing();
      float altura = ultrasonic.convert(microsec, Ultrasonic::CM); 

      altura=altura-12;//ajustes da mediçao devido a alguma perda de sinal ao londo do cabo
      percent=medicao_volume_tanque_pequeno(altura, TanquePequenoVazio);
    
      
      digitalWrite(bomba1,LOW);
      digitalWrite(led_bomba1,LOW);
      strcpy(str1,"OFF");
      
      if (digitalRead(3)==1){
        digitalWrite(bomba2,HIGH);
        digitalWrite(led_bomba2,HIGH);
        strcpy(str2,"ON");
      }
      
      else {
        digitalWrite(bomba2,LOW);
        digitalWrite(led_bomba2,LOW);
        strcpy(str2,"OFF");
      }
      LCD(percent,str1,str2);
      Serial.println(str1);
      Serial.println(str2);
    }
    while(percent<50);
  }
  
  digitalWrite (bomba1, HIGH);
  digitalWrite(led_bomba1,HIGH);
  strcpy(str1,"ON");
  
  if (digitalRead(3)==1 && percentagem <= 80) {
    
      digitalWrite(bomba2, HIGH);
      digitalWrite(led_bomba2,HIGH);
      strcpy(str2,"ON");
    }
    
    else { 
    digitalWrite(bomba2, LOW);
    digitalWrite(led_bomba2,LOW);
    strcpy(str2,"OFF");
   }
 
  
  if (percentagem==100) {
    digitalWrite(led_alarme,HIGH);
  }

}

/** Inserir os valores de percentagem no LCD*/

void LCD (int percentagem,char str1[], char str2[]){

  lcd.setCursor(0,0);
  lcd.print("Tanque:");
  lcd.print(percentagem);
  lcd.print("%");
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("Bomba1: ");
  lcd.print(str1);
  lcd.print("                ");
  delay(2000);
  lcd.setCursor(0,1);
  lcd.print("Bomba2: ");
  lcd.print(str2);
  delay(1500);
}






