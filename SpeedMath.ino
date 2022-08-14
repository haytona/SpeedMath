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
unsigned long inicio = 0; // beginning = time each quiz starts
int m, mu = 0, md = 0; //inicializace minut
int s, su = 0, sd = 0; //inicializace sekund
int c, cu, cd = 0; //inicializace milisekund


boolean modePlay = false;
boolean sadFace = false;
int numero1 = 0;
int numero2 = 0;
int numero3 = 0;
int resultado;
String operando = "";
String sResultado;
char level; //used in level select and generate question
String sLevel;

char cifra_jugador[4]; //Stores input from player

String sNumero_jugador;

int cuenta = 0;           // cursor (used to track input/guess)
byte intento = 1;         //question counter
int maximo_intentos = 10; //questions in game

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
  modePlay = false;
  intento = 1;        //reset attempts
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
  }

  // wait for any keypress
  // CAUTION: blocking method. any lights/timers will pause
  keypad.waitForKey();
  choose();

}

// generate the question/problem and display on screen
void generate_random()
{
  sResultado = "";
  sNumero_jugador = "";

  // use selected level to control range of random number
  switch (level)
  {
    case '1':
      numero1 = random(1, 11); //Generates a random number between 1 and 10
      numero2 = random(1, 11);
      break;
    case '2':
      numero1 = random(50, 100);
      numero2 = random(1, 11);
      break;
    case '3':
      numero1 = random(50, 100);
      numero2 = random(50, 100);
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
        int temp = numero1;
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

  lcd.clear();

  // print the equation top left
  lcd.setCursor(0, 0);
  lcd.print(numero1);
  lcd.setCursor(2, 0);
  lcd.print(operando);
  lcd.setCursor(3, 0);
  lcd.print(numero2);

  // print the question and total-questions botto right
  lcd.setCursor(intento>9? 11:12, 1);
  lcd.print(intento);
  lcd.print("/");
  lcd.print(maximo_intentos);

  // clearn input
  lcd.setCursor(0, 1);
  lcd.print("    ");
  //lcd.setCursor(0, 1);

}

void timer()
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
} // end timer()

void setup()
{
  randomSeed(analogRead(analogRead(0)));

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
    // correct answer
    intento = intento + 1; // increase question number

    if (intento <= maximo_intentos)
    {
      //generate next question
      generate_random();
    }
    else
    {
      game_over();  // Ends the game
    }
  }
  else
  {
    // incorrect answer :(
    sNumero_jugador = "";
    lcd.setCursor(6, 1);
    lcd.write(byte(1));//sad face
    sadFace = true;
  }

} //end verificar() // check guess

// display level select "menu"
void choose()
{
  //modePlay = false; // shouldn't be needed as set to false by default and in game_over()
  //intento = 1;      // shouldn't be needed as REset to 1 in game_over()
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Select level");
  lcd.setCursor(0, 1);
  lcd.print("1-E   2-M    3-H");
}

// start the game with countdown
void conteo()
{
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print(sLevel);
  delay(1000);

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
  inicio = millis();

} // end conteo() (start game)


// main loop
void loop()
{
  if (modePlay == true)
  {
    timer();
  }

  char key = keypad.getKey();

  // if a key has been pressed
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

        conteo(); // start the game

      } // end IF key select level

    } //end IF mode play off
    else
    {
      //Mode player

      if (key == '*')
      {
        // clear input
        cuenta = 0;           // reset cursor
        sNumero_jugador = ""; //reset answer as string
        lcd.setCursor(0, 1);  //clear input
        lcd.print("    ");
      }
      else if (key != 'A' && key != 'B' && key != 'C' && key != 'D')
      {

        cifra_jugador[cuenta] = key;

        sNumero_jugador = sNumero_jugador + String(key);
        //sNumero_jugador.concat(key);

        lcd.setCursor(0 + cuenta, 1);

        lcd.print(cifra_jugador[cuenta]);

        cuenta++; // increment cursor eg ones then tens then hundreds

        // if input = length of answer then check it
        if (cuenta == sResultado.length())
        {
          cuenta = 0; // reset cursor
          verificar();
        } 
        else if ( cuenta == 1 && sadFace )
        {
          //answer is longer than 1 number and
          //we got the previous attempt wrong so
          // clear rest of previous answer
          lcd.print("   ");
        }

      } //End if key!=ABCD

    } // End else mode player

  }    //End if Key pressed

} //End main loop
