#include <Keypad.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

// KEYPAD CODE
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = { {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'} };
byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// RGB 8x8 PROCESSOR CODE
int bits[8] = {128, 64, 32, 16, 8, 4, 2, 1};

int clock = 11;  // pin SCK del display
int data = 13;   // pin DI del display
int cs = 12;     // pin CS del display

byte bitmaps[10][8][8];     // Space for 10 frames of 8x8 pixels
byte displayPicture[8][8];  // What is currently ON display.

//Ship myShip(3, 3);

////////////////
int currentBitmap = 0;      // current displayed bitmap, per display
int targetBitmap = 0;       // Desired image, for the animation to strive for, per display
int step;                   // animation step, usually from 0 to 8, per screen
int stepDelay = 19;         // the wait time between each animation frame
//////////////////////////

//////////////////////////////
unsigned int delayCounter;           // holder for the delay, as to not hog to processor, per screen
int animationStyle = 0;     // different types of animation 0 = slide 1 = frame replace
unsigned long lastTime;     // display refresh time
//////////////////////////////

/******************/
/* GAME MECHANICS */
/******************/
bool shipsPlaced = false;
bool waitingOnOpponent = false;
/******************/


/**
void setup()
{
    Serial.begin(9600);  // used for debug

    initMatrix();
    int bitmap = 0;

    // black color for buildings 0
    // sky rotating color 1-7
  
    // bitmap 0
    addLineTobitmap(bitmap, 0, 1, 2, 3, 4, 5, 6, 100, 1);
    addLineTobitmap(bitmap, 1, 1, 1, 1, 1, 0, 0, 1, 1);
    addLineTobitmap(bitmap, 2, 1, 1, 1, 1, 0, 0, 1, 1);
    addLineTobitmap(bitmap, 3, 1, 1, 1, 1, 0, 0, 1, 0);
    addLineTobitmap(bitmap, 4, 1, 0, 1, 0, 0, 0, 1, 0);
    addLineTobitmap(bitmap, 5, 1, 0, 0, 0, 0, 0, 0, 0);
    addLineTobitmap(bitmap, 6, 0, 0, 0, 0, 0, 0, 0, 0);
    addLineTobitmap(bitmap, 7, 0, 0, 0, 0, 0, 0, 0, 0);

    lastTime = millis();
}

void loop()
{
    setBitmap(0);
    drawFrame(displayPicture);

    if (!shipsPlaced)
        placeShip();
}
*/

int main()
{
    init();

    Serial.begin(9600);
    Serial.println("Hello!");
    Serial.flush();
}


char findInput()
{
    char action = keypad.getKey();

    // Flash an LED
    while (action == 0)
    {
        delay(50);
        action = keypad.getKey();
    }

    return action;
}

// Place ALL ships
void placeShips()
{
    
}

void placeShip()
{
    shipsPlaced = true;
    
    char action = findInput();
    char row = 0;
    char col = 0;
    bool settingRows = true;

    while (action != '#')
    {
        
    }
    
}


void drawFrame(byte frame[8][8])
{
    digitalWrite(clock, LOW);  //sets the clock for each display, running through 0 then 1
    digitalWrite(data, LOW);   //ditto for data.
    delayMicroseconds(10);
    digitalWrite(cs, LOW);     //ditto for cs.
    delayMicroseconds(10);
    
    for(int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            // Drawing the grid. x across then down to next y then x across.
            writeByte(frame[x][y]);  
            delayMicroseconds(10);
        }
    }
    
    delayMicroseconds(10);
    digitalWrite(cs, HIGH);
}

// prints out bytes. Each colour is printed out.
void writeByte(byte myByte)
{
    for (int b = 0; b < 8; b++)
    {  // converting it to binary from colour code.
        digitalWrite(clock, LOW);
        
        if ((myByte & bits[b]) > 0)
            digitalWrite(data, HIGH);
        else
            digitalWrite(data, LOW);
            
        digitalWrite(clock, HIGH); 
        delayMicroseconds(10);
        digitalWrite(clock, LOW); 
    }
}

void initMatrix()
{
    pinMode(clock, OUTPUT); // sets the digital pin as output 
    pinMode(data, OUTPUT); 
    pinMode(cs, OUTPUT); 
}

void setBitmap(int bitmap)
{
    for(int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) //copies the bitmap to be displayed ( in memory )
            displayPicture[x][y] = bitmaps[bitmap][x][y];
}

void addLineTobitmap(int bitmap, int line, byte a, byte b, byte c, byte d, byte e, byte f, byte g, byte h)
{
    bitmaps[bitmap][7][line] = a;
    bitmaps[bitmap][6][line] = b;
    bitmaps[bitmap][5][line] = c;
    bitmaps[bitmap][4][line] = d;
    bitmaps[bitmap][3][line] = e;
    bitmaps[bitmap][2][line] = f;
    bitmaps[bitmap][1][line] = g;
    bitmaps[bitmap][0][line] = h;
}


