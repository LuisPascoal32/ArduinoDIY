#include <LiquidCrystal_I2C.h>
#include <Wire.h>



LiquidCrystal_I2C lcd(0x27, 20, 4);

int mDireita = 4;
int mEsquerda = 3;
int agua = 2;
int esgoto = 9;
int mCentrifugar = 6;
int lavagemButao = 12;
int pausa = 11;
int centrifugacaoButao = 7;
int potenciometro = 0;
int sensorAgua = 8;
int buzzer = 5;
int rst = 10;

long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second


void setup() {

  lcd.init();
  lcd.backlight();
  Serial.begin(57600);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(12, INPUT);
  pinMode(7, INPUT);
  pinMode(11, INPUT);
  pinMode(0, INPUT);
  pinMode(10, OUTPUT);
  pinMode(8, OUTPUT);

  for (int i = 2; i <= 6; i++) { //Desligar inicialmente todos os relays
    digitalWrite(i, 1);
  }
}



void loop() {

  long cornometroInicio = millis();

  int tempo = map(analogRead(0), 1023, 0, 1, 60);
  lcd.setCursor(0, 0);
  lcd.print("Minutos Lavagem");
  lcd.setCursor(8, 1);
  lcd.print(tempo);
  Serial.println(tempo);
  if (tempo < 10) {
    lcd.setCursor(9, 1);
    lcd.print(" ");
  }

  if (digitalRead(12) == 1) {

    long tempoDecorrido = cornometroInicio;
    Serial.println(tempoDecorrido);
    int diferencaMinutos = ((tempoDecorrido % day) % hour) / minute;
    int diferencaSegundos = (((tempoDecorrido% day) % hour) % minute) / second;
    Serial.print(diferencaMinutos);
    lcd.setCursor(0, 1);
    lcd.print("             ");
    lavagem(tempo, diferencaMinutos, diferencaSegundos);
    centrifugacao();
    digitalWrite(10, 1);
  }

  else if (digitalRead(7) == 1) {

    centrifugacao();
    digitalWrite(10, 1);
  }

  else {
    for (int i = 2; i <= 9; i++) { //Desligar inicialmente todos os relays
      digitalWrite(i, 1);
    }
  }


}

void lavagem(int tempo, int minDiferenca, int secDiferenca) {

  lcd.setCursor(0, 0);
  lcd.print("Iniciar Lavagem");
  Serial.println("Iniciando lavagem...");

  digitalWrite(esgoto, 0); //Fecha o esgoto e espera 2 segundos para abrir a agua
  delay(2000);
  digitalWrite(agua, 0);

  int pausaMinutos = 0, diferencaMinutos = 0,diferencaSegundos = 0,pausaSegundos = 0;
  long cornometro = 0.0, pausaMFinal = 0.0, pausaSFinal = 0.0;
  int minutes = 0;
  int seconds = 0, tempoTotal = 0;
  minutes = (((cornometro % day) % hour) / minute) - minDiferenca;
  int tempoTotalEstatico = tempo - minutes - minDiferenca;
  Serial.print(tempoTotalEstatico);
  delay(1000);
  
  do {

    cornometro = millis();
    seconds = ((((cornometro % day) % hour) % minute) / second) - secDiferenca - diferencaSegundos;
    minutes = (((cornometro % day) % hour) / minute) - minDiferenca - diferencaMinutos;
    tempoTotal = tempo - minutes;

    Serial.print(minDiferenca);
    Serial.print(" ");
    Serial.print(minutes);
    lcd.setCursor(0, 0);
    lcd.print("Lavagem...     ");
    lcd.setCursor(0, 1);
    lcd.print("TempoRestante:");
    lcd.print(tempoTotal );
    Serial.print("Lavagem: Tempo restante ");
    Serial.print(tempoTotal);
    Serial.print(" ");
    Serial.println(seconds);
    

    if (minutes != 0) {
      secDiferenca = 0;
    }

    if ((tempoTotal) < 10) {
      lcd.setCursor(15, 1);
      lcd.print(" ");
    }

    if (digitalRead(8) == 1) {
      digitalWrite(agua, 1);
    }
    else digitalWrite(agua, 0);


    if (minutes % 2 == 0 && seconds < 55) {
      digitalWrite(3, 0);
      digitalWrite(4, 1);
    }
    else digitalWrite(3, 1);

    if (minutes % 2 != 0 && seconds < 55) {
      digitalWrite(4, 0);
      digitalWrite(3, 1);
    }
    else digitalWrite(4, 1);

    
    if (digitalRead(11) == 1) {

      for (int i = 2; i <= 6; i++) { //Desligar todos os relays
        digitalWrite(i, 1);
        lcd.setCursor(0, 1);
        lcd.print("                ");
      }
          
      delay(200);
      
      diferencaMinutos = 0, diferencaSegundos= 0, pausaMinutos=0, pausaSegundos=0;
      pausaMinutos = minutes;
      pausaSegundos= seconds;
      
      while (digitalRead(11)!=1) {
         
        lcd.setCursor(0, 1);
        lcd.print("Pausa...");
        Serial.println("PAUSA...");
        pausaMFinal = minutes, pausaSFinal = seconds;
        
      }   delay(200);
          
          digitalWrite(agua, 0);
          
          diferencaMinutos = pausaMFinal - pausaMinutos;
          diferencaSegundos = pausaSFinal - pausaSegundos;
           
           Serial.print (" Dif:");
           Serial.print(pausaMFinal);
           Serial.print("-");
           Serial.print (pausaMinutos);
           Serial.println("=" + diferencaMinutos);
           Serial.print(pausaSFinal);
           Serial.print("-");
           Serial.print(pausaSegundos); 
           Serial.println("=" + diferencaSegundos);
      }
      
    

  }
  while (minutes < tempoTotalEstatico);
  lcd.setCursor(0, 0);
  lcd.print("                ");

}

void centrifugacao () {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  Serial.println("");

  lcd.setCursor(0, 0);
  lcd.print("Centrifugacao...");
  lcd.setCursor(0, 1);
  lcd.print("Passo 1/5");
  Serial.println("Centrifugacao: Passo 1 / 5 ");
  digitalWrite(agua, 1);
  digitalWrite(esgoto, 1);
  delay(5000);

  lcd.setCursor(0, 1);
  lcd.print("Passo 2/5");
  Serial.println("Centrifugacao: Passo 2 / 5 ");
  digitalWrite(mDireita, 0);
  delay(90000);
  digitalWrite(mDireita, 1);
  delay(5000);

  lcd.setCursor(0, 1);
  lcd.print("Passo 3/5");
  Serial.println("Centrifugacao: Passo 3 / 5 ");
  digitalWrite(mEsquerda, 0);
  delay(90000);
  digitalWrite(mEsquerda, 1);
  delay(5000);

  lcd.setCursor(0, 1);
  lcd.print("Passo 4/5");
  Serial.println("Centrifugacao: Passo 4 / 5 ");
  digitalWrite(mDireita, 0);
  delay(60000);
  digitalWrite(mDireita, 1);

  lcd.setCursor(0, 1);
  lcd.print("Passo 5/5");
  Serial.println("Centrifugacao: Passo 5 / 5 ");
  digitalWrite(mCentrifugar, 0);
  delay(2 * 60000);
  digitalWrite(mCentrifugar, 1);
  delay(5000);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  aviso();

}


/** Liga o buzzer indicar o fim da centrifugaçao*/

void aviso() {
  lcd.setCursor(0, 0);
  lcd.print("Fim Centrifugacao");
  for (int i = 0; i < 5; i++) {
    lcd.noBacklight();
    tone(5, 900);
    delay(200);
    lcd.backlight();
    noTone(5);
    delay(500);
    lcd.noBacklight();
    tone(5, 900);
    delay(500);
    lcd.backlight();
  }
  noTone(5);
}

