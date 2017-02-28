/**
*/ #define VERSION 0.3 /*
 * 
 * @authors Tad Miller, Danny Nsouli
 * @desc This processor drives the engine of Battleship
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
byte win[8][8] = {{4, 4, 4, 4, 4, 4, 4, 4}, {4, 4, 4, 4, 4, 4, 4, 4}, {4, 4, 4, 4, 4, 4, 4, 4}, {4, 4, 4, 4, 4, 4, 4, 4}, {4, 4, 4, 4, 4, 4, 4, 4}, {4, 4, 4, 4, 4, 4, 4, 4}, {4, 4, 4, 4, 4, 4, 4, 4}, {4, 4, 4, 4, 4, 4, 4, 4}};
byte lose[8][8] = {{64, 64, 64, 64, 64, 64, 64, 64}, {64, 64, 64, 64, 64, 64, 64, 64}, {64, 64, 64, 64, 64, 64, 64, 64}, {64, 64, 64, 64, 64, 64, 64, 64}, {64, 64, 64, 64, 64, 64, 64, 64}, {64, 64, 64, 64, 64, 64, 64, 64}, {64, 64, 64, 64, 64, 64, 64, 64}, {64, 64, 64, 64, 64, 64, 64, 64}};

/************************/
/* RGB LED DISPLAY VARS */
/************************/
#define EMPTY 2
#define HIT 100
#define DESTROY 64
#define SHIP 8
#define CURSOR 110
#define NOHIT 0

/*
 * @class Coords 
 * @desc This stores the information of a coordinate, x and y, or row and col.
 * 
 */

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

/*
 * @class Ship
 * @desc This class stores all of the informatoin about a ship.
 *       It allows us to determine the coordinates from transmitting destroy data
 * 
 */
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
            this -> destroyed = false;  //default ship attributes
            this -> c = c;
            this -> size = size;
            this -> shipNum = shipNum;
        }

        int getSize() //returns size of the ship (how many spaces it takes up on display)
        {
            return size;
        }

        bool isDestroyed()  //checks to see if ship is destroyed to turn it red
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

/*********************/
/*    KEYPAD CODE    */
/*********************/
char keys[4][3] = {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};
byte rowPins[4] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[3] = {4, 3, 2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 3);

/*********************/
/*    RGB DISPLAY    */
/*********************/
int bits[8] = {128, 64, 32, 16, 8, 4, 2, 1};
int clock = 11; // Pin SCK del display
int data = 13;  // Pin DI del display
int cs = 12;    // Pin CS del display
/******/
Ship *ships = new Ship[6];
byte shipsDestroyed = 0;
byte shipNum = 0;
/******************/

/*********************/
/*   TRANSMISSION    */
/*********************/
bool requested = false;
bool recieved = false;

void setup()
{
    init();
    Serial.begin(9600);
    randomSeed(analogRead(0));
    Wire.begin(8);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    // Initialize RGB LED matrix
    initMatrix();

    // Initialize game mechanics
    initGame();

    // Initialize connection between arduinos
    initConnection();

    // Get player 1, start flow of program
    determineFirst();
}

// Unused
void loop()
{
}

// Unused
void receiveEvent(int howMany)
{
    recieved = true;
}

// Unused
void requestEvent(int howMany)
{
    Serial.println("Bytes have been requested!");
    requested = true;
}

// Return a random value generated from time and random()
int t_rand(int x, int y)
{
    return ((((int) millis()) * random(x, y)) % (y + 1));
}

Coords *recieveCoords() //for one arduino to receive the coordinates of certain positions on the display
{
    int row = -1;
    int col = -1;
    byte i = 0;
    Serial.println("RECIEVING COORDS");
    int x = -1;
    int y = -1;

    byte count = 0;

    // Part of TCP/I2C protocol.
    while (x != 88 || row > 7 || row < 0)
    {   // Look for us to recieve the value "X" and then a number between 0 and 7. This would be our X coordinate.
        Wire.flush();
        x = Wire.read();
        delay(7);
        row = Wire.read();

        Serial.print(x);
        Serial.print(":");
        Serial.println(row);

        if (x == 88 && row < 8 && row > 0)
        {
            x = row;
            break;
        }
    }

    // Now that we've received X, transmit back an acknowledgement. This is us writing X = "Hey, I received X. Send Y now"
    while (y != 89 || row > 7 || row < 0)
    {
        Wire.flush();
        Wire.beginTransmission(8);
        Wire.write('X');
        Wire.endTransmission();

        y = Wire.read();
        delay(7);
        count++;
        col = Wire.read();

        Serial.print(y);
        Serial.print(";");
        Serial.println(col);

        if (y == 89 && col < 8 && col > 0 && count > 6)
        {
            y = col;
            break;
        }
    }

    delay(100);

    // OK. We have X and Y. Transmit back we got everything BARE MINIMUM 10 times.
    do
    {
        Wire.flush();
        Wire.beginTransmission(8);
        Wire.write('Y');
        Wire.endTransmission();
        delay(5);
    }
    while (Wire.read() != -1 || ++i < 10);

    Serial.print(x);
    Serial.println(row);

    Serial.print(y);
    Serial.println(col);

    Wire.flush();

    // Done.
    // In the case we receive a glitchy coordunate, we use this modulo to make sure it's between 0 and 7. It has happened a couple of times
    // and is the best solution I can come up with to solve this issue.
    return new Coords(row > 8 ? row % 8 : row, col > 8 ? col % 8 : col);
}

void transmitCoords(int x, int y)
{   
    Serial.print("Transmitting: "); //for one arduino to send the coordinates of certain positions on the display
    Serial.print(x);
    Serial.print(", ");
    Serial.println(y);

    // Transmit 'X' and the coordinate right after on the wire. This way the other end knows we're sending X
    do
    {
        Wire.flush();
        Wire.beginTransmission(8);
        Wire.write('X');
        delay(10);
        Wire.write(x);
        Wire.endTransmission();
    } // ... until we receive 'X' back which means they have that coordinate.
    while (Wire.read() != 'X');

    Serial.println("Sent X coords");

    // OK. They got X. Send Y now.
    do
    {
        Wire.flush();
        Wire.beginTransmission(8);
        Wire.write('Y');
        delay(10);
        Wire.write(y);
        Wire.endTransmission();
    }
    while (Wire.read() != 'Y');

    // Boom done.
    Wire.flush();
}

void myTurn()
{
    Serial.println("My turn");
    updateDisplay(firedPositions);
    Coords coord = placeDot(1);
    char status = 'A';

    Wire.beginTransmission(8);  //allows the player whos turn it is to send the transmission that they are firing on the other player
    Wire.write('F');
    delay(15);
    Wire.write('F');
    Wire.endTransmission();
    
    Serial.print("Transmitting coordinates! (");
    int x = coord.getX();
    int y = coord.getY(); //sending coordinates
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.println(")");
    transmitCoords(x, y);
    
    while (status != 'H' && status != 'N' && status != 'D')
    {
        status = Wire.read();
        delay(10);
    }

    Serial.print("Status: ");
    Serial.println(status);

    if (status == 'H')  //if a boat is hit the coordinates will be sent so that the spot turns orange
    {
        Serial.println("We hit them!");
        firedPositions[coord.getX()][coord.getY()] = HIT;
    }
    else if (status == 'D')
    {
        Serial.println("We've sunk their battleship!"); //if the boat is sunk then the spot will turn red once the coordiantes are transmitted
        shipsDestroyed++;
        firedPositions[coord.getX()][coord.getY()] = DESTROY;

        Serial.println("Lighting up spots destroyed as RED");
        while (Wire.read() != 'L')
        {
            delay(100);

            if (Wire.read() == 'L')
                break;
                
            Coords *c = recieveCoords();
            firedPositions[c -> getX()][c -> getY()] = DESTROY;

        }
    }
    else
    {
        Serial.println("No hit!");  //if no hit then the coordinates sent will turn off the light at that position
        firedPositions[coord.getX()][coord.getY()] = NOHIT;
    }

    updateDisplay(firedPositions);
    delay(2500);

    if (shipsDestroyed == 6)  //counter is kept on how many ships are destroyed, if they are all destoryed (6 of them) then win message is ent
    {
        Serial.println("I win!");

        while (1)
        {
            updateDisplay(win); //makes screen flash green
            delay(500);
            updateDisplay(firedPositions);  
            delay(500);
        }
    }

    if (status == 'H' || status == 'D') //a player is allowed to keep firing if they get a hit or destroy a ship
        myTurn();
    
    waitForTurn();
}

void waitForTurn()
{
    updateDisplay(myShipsDisplay);  //show ships while other person fires

    Serial.println("Waiting for other player to fire...");
    char status = 'A';

    while (Wire.read() != 'F')
        delay(10);

    Serial.println("They are firing!");

    Coords *c = recieveCoords();

    int theirRow = c -> getX(); //getting their column and row values to bolster communcation
    int theirCol = c -> getY();

    Serial.print("Their Row: ");
    Serial.println(theirRow);   //used for testing to see if coordinates were being mapped correctly 
    Serial.print("Their Col: ");
    Serial.println(theirCol);

    // transmit back whether we hit H, destroyed D, or did not hit N
    status = 'N';

    if (myShipsDisplay[theirRow][theirCol] == EMPTY)
        myShipsDisplay[theirRow][theirCol] = NOHIT;
    else if (myShipsDisplay[theirRow][theirCol] == SHIP)
    {
        Serial.print("They hit us!");
        myShipsDisplay[theirRow][theirCol] = HIT;  // ship would turn orange when hit by other player

        if (ships[shipsLoc[theirRow][theirCol]].isDestroyed())
            status = 'D'; //used for testing to see if hit and destory would be differentiated
        else
            status = 'H'; //h is for single hit and d for destoryed
    }

    Serial.print("Status: ");
    Serial.println(status);
    delay(500);

    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(status);
    Wire.endTransmission(); // stop transmitting

    updateDisplay(myShipsDisplay);

    if (status == 'D')
    {
        for (int i = 0; i < ships[shipsLoc[theirRow][theirCol]].getSize(); i++)
        {
            Coords cc = ships[shipsLoc[theirRow][theirCol]].getCoords()[i];

            Serial.print("Transmitting coord ");  //message of destroyed ship would have to be relayed
            int row = cc.getX();
            int col = cc.getY();
            Serial.print(row);
            Serial.print(", ");
            Serial.print(col);
            transmitCoords(row, col);
        }
        delay(100);
        Serial.println("Done transmitting coordinates");
        Wire.beginTransmission(8);
        Wire.write('L');
        delay(15);
        Wire.write('L');
        delay(15);
        Wire.write('L');
        Wire.endTransmission();
    }
    delay(2500);

    if (status == 'H' || status == 'D')
        waitForTurn();
        
    myTurn();
}

// Determines which player goes first, by rolling an 80 sided die.
void determineFirst()
{
    Serial.println("Determining player to go first...");
    Serial.flush();
    int rnd = t_rand(1, 80);
    int myNum = rnd > 0 ? rnd : -rnd;
    int opNum = -1;

    // Transmit data across Wire of my random number.
    do
    {
        opNum = Wire.read();
    
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write(myNum);        // sends five bytes
        Wire.endTransmission();    // stop transmitting
        
        delay(50);
    }
    while (opNum == -1 || opNum == 82);

    // Brute force method. Again.
    delay(25);
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(myNum);        // sends five bytes
    Wire.endTransmission();    // stop transmitting

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
// Maybe if we had more time...
void initGame()
{
    Serial.print("Starting Battleship v");
    Serial.print(VERSION);
    Serial.println("...");
    Serial.flush();

    placeShips();
}

// Establish connection between Arduinos
// Transmit 'R' across wire when ready. Game will begin after this process.
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

    // Brute force technique. Not the best but works for the 99%
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write('R');
    delay(50);
    Wire.write('R');
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

    Serial.println("Returning spot...");
    return isValidSpots(c -> getX(), c -> getY(), true, size) ? c : new Coords(3, 3);
}

// orientation 0 = down/up, 1 = left/right
bool isValidSpots(int row, int col, bool orientation, int size)
{
    int i;

    for (i = 0; i < size; i++)  //checks to see where a spot is that empty to put cursor
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

bool isValidSpot(int row, int col)  //like previous method but is used for ship placement
{
    if (!shipsPlaced)
        return myShipsDisplay[row][col] == EMPTY && 8 > row && row > -1 && 8 > col && col > -1 ? true : false;
    else
        return firedPositions[row][col] == EMPTY && 8 > row && row > -1 && 8 > col && col > -1 ? true : false;
}

void cpyTmpDisplay()  //part of the device driver that we built for the RGB 8X8 matrix
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

void displayDots(int row, int col, bool orientation, int size) //shows individually colored dots on screen
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

void placeDot(int row, int col, bool orientation, int size) //used to construct the colored images on the screen
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
Coords placeDot(int size)
{
    Coords *c = new Coords(3, 3); //findSpot(size);
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
                                            //controls for directional pad functionality with keypad buttons
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

    Coords *ccc = new Coords(row, col);
    return *ccc;
}

void updateDisplay(byte frame[8][8]) //used to change frame, constantly updated when needed
{
    drawFrame(frame);
    delay(20);
    drawFrame(frame);
}

void drawFrame(byte frame[8][8])  //draws frame on 8x8 matrix
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
