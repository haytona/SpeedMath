/* Speed Game made by José Luis Bejarano Vásquez
   31/08/2016 */
#define DEBUG true
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
byte sad[8] = {
  B00000,
  B01010,
  B01010,
  B00000,
  B00000,
  B01110,
  B10001,
};

unsigned long time = 0;
unsigned long inicio = 0;
int m, mu = 0, md = 0; //inicializace minut
int s, su = 0, sd = 0; //inicializace sekund
int c, cu, cd = 0; //inicializace milisekund
byte intento = 0;

char level;
boolean modePlay = false;
int cifra_azar;
int numero1 = 0;
int numero2 = 0;
int numero3 = 0;
int temp = 0;
int resultado;
String operando = "";
String sResultado;
String sLevel;
int largo = 0;
boolean activar = false;
boolean temporizar = false;


char cifra_jugador[4]; //Stores the number of the player

String numero_jugador = String();
String sNumero_jugador;

int cuenta = 0;
int i, j, x;
int puntos, famas = 0;
int intentos = 0;
int maximo_intentos = 10;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = { 11, 10, 9, 8 }; // Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte colPins[COLS] = {  7, 6, 5, 4 }; // Connect keypad COL0, COL1 and COL2 to these Arduino pins.
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void game_over()
{
  temporizar = false;
  modePlay = false;
  intento = 0;        //reset attempts
  activar = false;    // stop timer
  lcd.clear();
  //if (sd >= 3 || mu >= 1 || md >= 1) // 30s limit
  if ( md >=1 ) // 10 minute timeout
  {
    //longer than 30s
    lcd.setCursor(0, 0);
    lcd.print("Too slow!       ");
    lcd.setCursor(2, 1);
    lcd.print("To try again");
  }
  else
  {
    lcd.setCursor(4, 0);
    lcd.print("Victory!");
    lcd.setCursor(0, 1);
    lcd.print(" Time: ");
    lcd.print(md);
    lcd.print(mu);
    lcd.print(":");
    lcd.print(sd);
    lcd.print(su);
    lcd.print(":");
    lcd.print(cd);
    lcd.print(cu);
    delay(1000);
  }

}

// generate the question/problem
void generate_random()
{
  lcd.clear();
  cuenta = 0;
  sResultado = "";
  sNumero_jugador = "";

  randomSeed(analogRead(analogRead(0)));

  switch (level)
  {
    case '1':
      numero1 = random(1, 11); //Generates a number between un número aleatorio entre 1 and 10
      numero2 = random(1, 11); //Generates a number between un número aleatorio entre 1 and 10
      break;

    case '2':
      numero1 = random(50, 100); //Generates a number between un número aleatorio entre 50 and 99
      numero2 = random(1, 11); //Generates a number between un número aleatorio entre 1 and 10
      break;

    case '3':
      numero1 = random(50, 100); //Generates a number between un número aleatorio entre 1 y 99
      numero2 = random(50, 100); //Generates a number between un número aleatorio entre 1 y 10
      break;
  }

  numero3 = random(1, 5); //Generates a number between 1 and 4

  switch (numero3)
  {
    case 1:
      operando = "+";
      resultado = numero1 + numero2;
      break;
    case 2:
      operando = "-";
      if (numero1 < numero2)
      {
        temp = numero1;
        numero1 = numero2;
        numero2 = temp;
      }
      resultado = numero1 - numero2;
      break;
    case 3:
      operando = "*";
      resultado = numero1 * numero2;
      break;
    case 4:
      operando = "/";
      resultado = numero1 % numero2;
      if (resultado != 0) //If not is zero the mod
      {
        operando = "*";
        resultado = numero1 * numero2;
      }
      else  //The mod is zero
      {
        resultado = numero1 / numero2;
      }
  }

  sResultado =  String(resultado);

  lcd.setCursor(0, 0);
  lcd.print(numero1);
  lcd.setCursor(2, 0);
  lcd.print(operando);
  lcd.setCursor(3, 0);
  lcd.print(numero2);
  lcd.setCursor(12, 1);
  lcd.print(cuenta);

  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.setCursor(0, 1);

}

void timer()
{

  if (modePlay == true)
  {
    time = millis() - inicio;

    m = (time / 1000) / 60;           //Minutes
    mu = m % 10;
    md = (m - mu) / 10;

    s = (time / 1000) % 60;           //Seconds
    su = s % 10;
    sd = (s - su) / 10;

    c = (time / 100) % 60;
    cu = c % 10;
    cd = (c - cu) / 10;

    lcd.setCursor(8, 0);
    lcd.print(md);
    lcd.print(mu);
    lcd.print(":");
    lcd.print(sd);
    lcd.print(su);
    lcd.print(":");
    lcd.print(cd);
    lcd.print(cu);
  }

}

void setup()
{
  //Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, sad);
  choose();    //Displays the select level mode
}

//********************************************************
void verificar()
{
  if (sNumero_jugador == sResultado)
  {
    lcd.setCursor(6, 0);
    lcd.print("G");
    intento = intento + 1;
    generate_random();
  }
  else
  {
    lcd.setCursor(0, 1);
    cuenta = 0;
    sNumero_jugador = "";
    lcd.setCursor(6, 1);
    lcd.write(byte(1));//sad face
  }

  lcd.setCursor(8, 1);
  lcd.print("Try:");
  lcd.print(intento);
  lcd.print("/10");

  if (intento == maximo_intentos)
  {
    game_over();  // Ends the game
  }

}

void choose()
{
  modePlay = false;
  intento = 0;
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Select level");
  lcd.setCursor(0, 1);
  lcd.print("1-E   2-M    3-H");
}

void conteo()
{
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print(sLevel);
  delay(2000);

  lcd.clear();

  for (int x = 3; x >= 1; x--)
  {
    lcd.setCursor(8, 0);
    lcd.print(x);
    delay(300);
  }

  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("Go");

  delay(600);

  generate_random();

  modePlay = true;

  if (activar == false)
  {
    inicio = millis();
    activar = true;
  }

}


void loop()
{
  timer();

  char key = keypad.getKey();

  if (key)
  {
    //If is the select level display
    if (modePlay == false)
    {
      if (key == '1' || key == '2' || key == '3')
      {
        level = key;
        lcd.clear();
        lcd.setCursor(5, 1);

        switch (level)
        {
          case '1':
            sLevel = "Easy Level";
            break;
          case '2':
            sLevel = "Medium Level";
            break;
          case '3':
            sLevel = "Hard Level";
            break;
        } //end switch

        conteo();

      } // end IF key select level

    } //end IF mode play off
    else
    {
      //Mode player

      temporizar = true;

      if (activar == false)
      {
        inicio = millis();
        activar = true;
        lcd.clear();
      }

      if (key != 'A' && key != 'B' && key != 'C' && key != 'D')
      {

        cifra_jugador[cuenta] = key;

        numero_jugador = String(key);

        sNumero_jugador = sNumero_jugador + numero_jugador;

        lcd.setCursor(0 + cuenta, 1);

        lcd.print(cifra_jugador[cuenta]);

        cuenta++;

        largo = sResultado.length();

        if (cuenta == largo)
        {
          //lcd.setCursor(10,1);
          verificar();
        }

      } //End if key!=

    } // End else mode player

    if (key == 'A')
    {
      modePlay = false;
      inicio = millis();
      activar = false;
      choose();
    }

  }    //End if Key main

} //End loop
