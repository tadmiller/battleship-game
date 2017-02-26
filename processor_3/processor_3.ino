/**
 * @version 0.1
 * 
 * 
 */

#include <Keypad.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

class Coords
{
    private:
        int x;
        int y;

    public:
        Coords(int x, int y)
        {
            this -> x = x;
            this -> y = y;
        }

        int getX()
        {
            return x;
        }

        int getY()
        {
            return y;
        }
};

//

/*******************/
/* KEYPAD CODE     */
/*******************/
char keys[4][3] = { {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'} };
byte rowPins[4] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[3] = {4, 3, 2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, 4, 3);

// RGB 8x8 PROCESSOR CODE
int bits[8] = {128, 64, 32, 16, 8, 4, 2, 1};

int clock = 11;  // pin SCK del display
int data = 13;   // pin DI del display
int cs = 12;     // pin CS del display

/*******************/
/* RGB LED DISPLAY */
/*******************/
#define EMPTY 2 //2
#define HIT 100
#define DESTROY 44
#define SHIP 8
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
/*******************/


/******************/
/* GAME MECHANICS */
/******************/
bool shipsPlaced = false;
bool waitingOnOpponent = false;
bool isMyTurn = false;

byte myShipsDisplay[8][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
byte tmpShipsDisplay[8][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
/******************/

void setup()
{
    init();
    Serial.begin(9600);
    Wire.begin(8);
    Wire.onReceive(receiveEvent);

    initMatrix();
    initGame();
    initConnection();
    determineFirst();
}

void loop()
{
    
}

void receiveEvent(int howMany)
{

}

void determineFirst()
{
    Serial.println("Determining player to go first...");
    Serial.flush();
    int myNum = random(1, 10);
    int opNum = -1;

    do
    {
        opNum = Wire.read();
    
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write(myNum);        // sends five bytes
        Wire.endTransmission();    // stop transmitting
        
        delay(random(1, 100));
    }
    while (opNum == -1 || opNum == 82);

    Serial.print("My number is: ");
    Serial.println(myNum);

    Serial.print("Op number is: ");
    Serial.println(opNum);
    Serial.flush();
}

// Should add here "WELCOME TO BATTLESHIP" on RGB display
void initGame()
{
    Serial.println("Starting Battleship...");
    Serial.flush();

    placeShips();
}

// R = Ready
void initConnection()
{
    Serial.println("Establishing connection...");

    do
    {
        delay(random(1, 100));
    
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write('R');        // sends five bytes
        Wire.endTransmission();    // stop transmitting
    }
    while (Wire.read() != 'R');

    Serial.println("Connection established");
    Serial.flush();

    delay(500);
}

/**
 * 
 * 
 * 
 * void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  Wire.requestFrom(8, 6);    // request 6 bytes from slave device #8

  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  
void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  delay(100);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write("hello "); // respond with message of 6 bytes
  
 */

// Initialize the matrix. Might need to be in an object eventually
void initMatrix()
{
    pinMode(clock, OUTPUT); // sets the digital pin as output 
    pinMode(data, OUTPUT); 
    pinMode(cs, OUTPUT); 

    updateFrame();
}

// Get an input from the keypad
char findInput()
{
    char action = keypad.getKey();
    Serial.println("Waiting on input");
    Serial.flush();

    // Flash an LED
    while (action == 0)
    {
        delay(50);
        action = keypad.getKey();
    }

    Serial.println("Got valid input");
    Serial.flush();
    return action;
}

// Place ALL ships
void placeShips()
{
    placeShip(2);
    placeShip(5);
    placeShip(3);
    placeShip(4);
    placeShip(3);
    placeShip(2);
}

Coords *findSpot(int size)
{
    Coords *c = new Coords(random(1, 8), random(1, 8));

    while (!isValidSpots(c -> getX(), c -> getY(), true, size))
        c = new Coords(random(1, 8), random(1, 8));

    return c;
}

// orientation 0 = down/up, 1 = left/right
bool isValidSpots(int row, int col, bool orientation, int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (orientation)
        {
            if (!isValidSpot(row, col + i))
                return false;
        }
        else
            if (!isValidSpot(row + i, col))
                return false;
    }

    return true;
}

bool isValidSpot(int row, int col)
{
    return myShipsDisplay[row][col] == EMPTY && 8 > row && row > -1 && 8 > col && col > -1 ? true : false;
}

void cpyTmpShips()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            tmpShipsDisplay[i][j] = myShipsDisplay[i][j];
}

void displayShip(int row, int col, bool orientation, int size)
{
    if (orientation)
        for (int i = 0; i < size; i++)
            tmpShipsDisplay[row][col + i] = SHIP;
    else
        for (int i = 0; i < size; i++)
            tmpShipsDisplay[row + i][col] = SHIP;   
}

void placeShip(int row, int col, bool orientation, int size)
{
    if (orientation)
        for (int i = 0; i < size; i++)
            myShipsDisplay[row][col + i] = SHIP;
    else
        for (int i = 0; i < size; i++)
            myShipsDisplay[row + i][col] = SHIP;       
}

// Place one ship
// KNOWN BUGS: Can't place in spot 0x0
// TODO: Add function that finds valid spot for ship of length int size
// @args: size of ship
void placeShip(int size)
{
    Coords *c = findSpot(size);
    int row = c -> getX();
    int col = c -> getY();
    bool orientation = true;
    char action;

    while (true)
    {
        cpyTmpShips();
        displayShip(row, col, orientation, size);
        updateFrame();

        action = findInput();
        
        if (action == '7' && ((col + size < 9 && !orientation) || (row + size < 9 && orientation)))
            orientation = !orientation;
        else if (action == '8') // up
        {   
            if ((col + size < 8 && orientation) || (col < 7 && !orientation))
                col++;
        }
        else if (action == '2') // down
        {
            if (col > 0)
                col--;
        }
        else if (action == '4') // left
        {
            if (row > 0)
                row--;
        }
        else if (action == '6') // right
        {   
            if ((row < 7 && orientation) || (row + size < 8 && !orientation))
                row++;
        }
        else if (action == '5')
        {
            if (isValidSpots(row, col, orientation, size))
            {
                placeShip(row, col, orientation, size);
                break;
            }
            else
            {
                // display all red
            }
        }
    }
    
    Serial.print("Row: ");
    Serial.println(row);

    Serial.print("Col: ");
    Serial.println(col);
    Serial.flush();
}

void updateFrame()
{
    drawFrame(tmpShipsDisplay);
    delay(10);
    //drawFrame(myShipsDisplay);
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
