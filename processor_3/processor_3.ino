/**
*/ #define VERSION 0.18 /*

 * 
 * 
 */

#include <Keypad.h>
#include <Wire.h>

/******************/
/* GAME MECHANICS */
/******************/
bool shipsPlaced = false;
byte firedPositions[8][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
byte myShipsDisplay[8][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
byte tmpDisplay[8][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
byte shipsLoc[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
/************************/
/* RGB LED DISPLAY VARS */
/************************/
#define EMPTY 2 //2
#define HIT 100
#define DESTROY 64
#define SHIP 8
#define CURSOR 110
#define NOHIT 0

class Coords
{
    private:
        int x;
        int y;

    public:
        Coords() {}
    
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

class Ship
{
    private:
        Coords *c;
        int size;
        int shipNum;
        bool destroyed;

    public:
        Ship() {}
        Ship(int x, int y, int size, int shipNum, Coords c[])
        {
            this -> destroyed = false;
            this -> c = c;
            this -> size = size;
            this -> shipNum = shipNum;
        }

        int getSize()
        {
            return size;
        }

        bool isDestroyed()
        {
            if (this -> destroyed)
                return true;
            else
            {
                Coords *c = this -> getCoords();

                for (int i = 0; i < this -> size; i++)
                {
                    Coords cc = c[i];
                    
                    if (myShipsDisplay[cc.getX()][cc.getY()] == SHIP)
                        return false;
                    else if (i + 1 == this -> size)
                    {
                        for (int j = 0; j < this -> size; j++)
                        {
                            Coords ccc = c[j];
                            int row = ccc.getX();
                            int col = ccc.getY();

                            Serial.print("Row: ");
                            Serial.println(row);
                            Serial.print("Col: ");
                            Serial.println(col);
                            
                            myShipsDisplay[row][col] = DESTROY;
                        }
                        
                        this -> destroyed = true;
                        return true;
                    }
                }                
                
                return false;
            }
        }

        Coords *getCoords()
        {
            return c;
        }
};

/*******************/
/* KEYPAD CODE     */
/*******************/
char keys[4][3] = {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};
byte rowPins[4] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[3] = {4, 3, 2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 3);


/***************RGB*/
int bits[8] = {128, 64, 32, 16, 8, 4, 2, 1};
int clock = 11; // Pin SCK del display
int data = 13;  // Pin DI del display
int cs = 12;    // Pin CS del display
/**********************/
Ship *ships = new Ship[6];
byte shipsDestroyed = 0;
byte shipNum = 0;
/******************/

void setup()
{
    init();
    Serial.begin(9600);
    randomSeed(analogRead(0));
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

int t_rand(int x, int y)
{
    return ((((int) millis()) * random(x, y)) % (y + 1));
}

Coords *recieveCoords()
{
    int row = -1;
    int inBetween = -1;
    int col = -1;

    Wire.beginTransmission(8);

    do
    {
        Wire.write('X');
        row = Wire.read();
    }
    while (row < 0 && row > 8);
    Serial.println("Recieved X");

    do
    {
        Wire.write(',');
    }
    while (Wire.read() != ',');
    Serial.println("Recieved ,");

    do
    {
        Wire.write('Y');
        col = Wire.read();
    }
    while (col < 0 && col > 8);
    Serial.println("Recieved Y");

    for (int i = 0; i < 10; i++)
    {
        Wire.write('C');
    }
    Serial.println("Done");

    Wire.endTransmission();

    Serial.print("(");
    Serial.print(row);
    Serial.print(", ");
    Serial.print(col);
    Serial.println(")");

    return new Coords(row, col);
}

void transmitCoords(int x, int y, char status)
{   
    Wire.beginTransmission(8);

    while (Wire.read() != 'X')
    {
        Serial.print("SENT ");
        Serial.println(status);
        Wire.write(status);
        delay(100);
    }
    Serial.println("Sent STATUS");

    while (Wire.read() != ',')
        Wire.write(x);
    Serial.println("Sent X");
    
    while (Wire.read() != 'Y')
        Wire.write(',');
Serial.println("Sent ,");

    while (Wire.read() != 'C')
        Wire.write(y);
    Serial.println("Sent Y");

    delay(100);

    Wire.endTransmission(); // stop transmitting
}

void myTurn()
{
    Serial.println("My turn");
    updateDisplay(firedPositions);
    Coords *coord = placeDot(1);
    char status = 'A';

    Serial.println("Waiting to see if hit or not...");
    transmitCoords(coord -> getX(), coord -> getY(), 'F');
    
    while (status != 'H' && status != 'N' && status != 'D')
    {
        status = Wire.read();
        delay(10);
    }

    Serial.print("Status: ");
    Serial.println(status);

    if (status == 'H')
    {
        Serial.println("We hit them!");
        firedPositions[coord -> getX()][coord -> getY()] = HIT;
    }
    else if (status == 'D')
    {
        Serial.println("We've sunk their battleship!");
        shipsDestroyed++;
        firedPositions[coord -> getX()][coord -> getY()] = DESTROY;

        if (shipsDestroyed == 6)
            Serial.println("I win!");
    }
    else
    {
        Serial.println("No hit!");
        firedPositions[coord -> getX()][coord -> getY()] = NOHIT;
    }

    updateDisplay(firedPositions);
    delay(2500);

    if (status == 'H' || status == 'D')
        myTurn();
    
    waitForTurn();
}

void waitForTurn()
{
    updateDisplay(myShipsDisplay);

    Serial.println("Waiting for other player to fire...");
    char status = 'A';

    while (status != 'F')
    {
        Serial.println(status);
        status = Wire.read();
        delay(10);
    }

    Coords *c = recieveCoords();

    int theirRow = c -> getX();
    int theirCol = c -> getY();

    Serial.print("Their Row: ");
    Serial.println(theirRow);
    Serial.print("Their Col: ");
    Serial.println(theirCol);

    // transmit back whether we hit H, destroyed D, or did not hit N
    char status = 'N';

    if (myShipsDisplay[theirRow][theirCol] == EMPTY)
        myShipsDisplay[theirRow][theirCol] = NOHIT;
    else if (myShipsDisplay[theirRow][theirCol] == SHIP)
    {
        Serial.print("They hit us!");
        myShipsDisplay[theirRow][theirCol] = HIT;

        if (ships[shipsLoc[theirRow][theirCol]].isDestroyed())
            status = 'D';
        else
            status = 'H';
    }

    Serial.print("Status: ");
    Serial.println(status);
    delay(500);

    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(status);
    Wire.endTransmission(); // stop transmitting

    updateDisplay(myShipsDisplay);
    delay(2500);

    if (status == 'H' || status == 'D')
        waitForTurn();
        
    myTurn();
}

void determineFirst()
{
    Serial.println("Determining player to go first...");
    Serial.flush();
    int myNum = t_rand(1, 80);
    int opNum = -1;

    do
    {
        opNum = Wire.read();
    
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write(myNum);        // sends five bytes
        Wire.endTransmission();    // stop transmitting
        
        delay(t_rand(1, 100));
    }
    while (opNum == -1 || opNum == 82);

    Serial.print("My number is: ");
    Serial.println(myNum);

    Serial.print("Op number is: ");
    Serial.println(opNum);
    Serial.flush();

    if (myNum > opNum)
    {
        Serial.println("I go first");
        myTurn();
    }
    else if (myNum == opNum)
        determineFirst();
    else
    {
        Serial.println("I go second");
        waitForTurn();
    }
}

// Should add here "WELCOME TO BATTLESHIP" on RGB display
void initGame()
{
    Serial.print("Starting Battleship v");
    Serial.print(VERSION);
    Serial.println("...");
    Serial.flush();

    placeShips();
}

// R = Ready
void initConnection()
{
    Serial.println("Establishing connection...");

    do
    {
        delay(50);
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write('R');        // sends five bytes
        Wire.endTransmission();    // stop transmitting
    }
    while (Wire.read() != 'R');

    Wire.beginTransmission(8); // transmit to device #8
    Wire.write('R');        // sends five bytes
    Wire.endTransmission();    // stop transmitting

    Serial.println("Connection established");
    Serial.flush();

    delay(500);
}

// Initialize the matrix. Might need to be in an object eventually
void initMatrix()
{
    pinMode(clock, OUTPUT); // sets the digital pin as output 
    pinMode(data, OUTPUT); 
    pinMode(cs, OUTPUT); 

    updateDisplay(tmpDisplay);
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
    placeDot(2);
    placeDot(3);
    placeDot(4);
    placeDot(3);
    placeDot(2);
    placeDot(3);

    shipsPlaced = true;
}

Coords *findSpot(int size)
{
    Coords *c = new Coords(t_rand(1, 8), t_rand(1, 8));

    while (!isValidSpots(c -> getX(), c -> getY(), true, size))
        c = new Coords(t_rand(1, 8), t_rand(1, 8));

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
    if (!shipsPlaced)
        return myShipsDisplay[row][col] == EMPTY && 8 > row && row > -1 && 8 > col && col > -1 ? true : false;
    else
        return firedPositions[row][col] == EMPTY && 8 > row && row > -1 && 8 > col && col > -1 ? true : false;
}

void cpyTmpDisplay()
{
    if (!shipsPlaced)
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                tmpDisplay[i][j] = myShipsDisplay[i][j];
    else
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                tmpDisplay[i][j] = firedPositions[i][j];
}

void displayDots(int row, int col, bool orientation, int size)
{
    if (!shipsPlaced)
        if (orientation)
            for (int i = 0; i < size; i++)
                tmpDisplay[row][col + i] = SHIP;
        else
            for (int i = 0; i < size; i++)
                tmpDisplay[row + i][col] = SHIP;
    else
        tmpDisplay[row][col] = CURSOR;
}

void placeDot(int row, int col, bool orientation, int size)
{
    if (!shipsPlaced)
    {
        Coords *c = new Coords[size];
        
        if (orientation)
            for (int i = 0; i < size; i++)
            {
                myShipsDisplay[row][col + i] = SHIP;
                shipsLoc[row][col + i] = shipNum;
                c[i] = Coords(row, col + i);
            }
        else
            for (int i = 0; i < size; i++)
            {
                myShipsDisplay[row + i][col] = SHIP;
                shipsLoc[row + i][col] = shipNum;
                c[i] = Coords(row + i, col);
            }
            
        ships[shipNum] = Ship(row, col, size, shipNum, c);
        shipNum++;
        Serial.print("Ship num: ");
        Serial.println(shipNum);

        for (int i = 0; i < 2; i++)
        {
            Ship s = ships[shipNum - 1];
            Coords *c = s.getCoords();
            Coords cc = c[i];
            Serial.print("Row: ");
            Serial.println(cc.getX());
            Serial.print("Col: ");
            Serial.println(cc.getY());
        }
    }
    else
        firedPositions[row][col] = CURSOR;
}

// Place one ship
// TODO: Add function that finds valid spot for ship of length int size
// @args: size of ship
Coords *placeDot(int size)
{
    Coords *c = findSpot(size);
    int row = c -> getX();
    int col = c -> getY();
    bool orientation = true;
    char action;

    while (true)
    {
        cpyTmpDisplay();
        displayDots(row, col, orientation, size);
        updateDisplay(tmpDisplay);

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
                placeDot(row, col, orientation, size);
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

    return new Coords(row, col);
}

void updateDisplay(byte frame[8][8])
{
    drawFrame(frame);
    delay(20);
    drawFrame(frame);
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
