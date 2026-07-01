/*
Name: Kadir Alpay     Course: ICS3U      Date: 26/02/25
PROJECT TITLE:                    Battleship
PROJECT OBJECTOVE: I hope to learn more about functions and debugging through this project, and this will help me gain a better understanding of how complicated and large programs function
PROJECT DESCRIPTION:
This program replicates the classic game of "Battleship," with the option to play player vs. player or player vs. bot.

The game is typically played on a grid seperated in half by a divider that blocks the opponents view. 
Each player sets up their side by placing the given battleships of varying sizes on their grid. 
Then the players take turns calling out coordinates on the grid on which they think the opponent has a battleship. 
If a battleship is hit, the opponent responds with "hit," but if the shot lands in the ocean, the opponent responds with "miss."
When all the squares a ship occupies on the grid are hit, the ship is considered sunk, and the player must announce that a ship was sunk.
The first player to sink all of their opponent's ships wins. 
PROGRAM DEVELOPMENT PLAN: The minimum functionality requirement for my game will be the player vs. a basic bot, and optional features will likely include more advanced bots with greater logic

NOTE: THERE IS A BUG WITH THE PLAY AGAIN FEATURE BUT ONLY OCCURS WHEN USER FOR SOME REASON MAKES AN INPUT DURING WIN ANIMATION*************************************
*/

#include <stdio.h>
#include "graphics.h"
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

const char columnLetters[11] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'}; //to help convert between letters and numbers

struct COORDINATES {
    int row; //row
    int col; //column
};

struct SHIPS {
    char name[10]; //name of ship
    COORDINATES coordinates[5]; //store info on coordinates of the ships
    int len; //length of the ship
    int hitCounter; //track how many times the ship has been hit
};

//FUNCTION PROTOTYPES - DESIGN YOUR FUNCTIONS.
void initializeBoard(colour grid[10][10], colour missilesFired[10][10], colour gridBot[10][10]); //clear the board
struct COORDINATES getCoordinate(); //get the coordinate of the missile attack
int validMissileCheck(COORDINATES coords, colour missilesFired[10][10]); //check if missile is valid before confirming
int validPointCheck(int row, int col, int len, colour board[10][10]); //check if a point is already occupied by a ship or empty
void printBoard(colour grid[10][10], colour missilesFired[10][10], SHIPS ship[2][5]); //print the current state of the board
int fireMissile(COORDINATES coords, colour board[10][10], colour missilesFired[10][10]); //return the result of the missile
int checkWin(colour grid[10][10], colour gridBot[10][10]); //check if either side won
void randomizeShips(colour grid[10][10], int playerOrBot, SHIPS ship[2][5]); //randomize the placement of the ships
void manualShips(colour grid[10][10], SHIPS ship[2][5], colour missilesFired[10][10]); //manual placement of ships
void shipCoordinateFiller(int direction, SHIPS ship[2][5], int shipNum, int playerOrBot); //fills in the rest of the coordinates for the ship struct
int validPlacementCheck(int row1, int col1, int row2, int col2, int direction, colour board[10][10], int len); //check if placement is valid before confirming
void placeShip(int row1, int col1, int row2, int col2, int direction, colour board[10][10]); //place the ship
int easyBotTurn(colour grid[10][10], COORDINATES *botCoords); //easy difficulty bot turn (shoots randomly)
int mediumBotTurn(colour grid[10][10], COORDINATES *botCoords, int botCondition, COORDINATES *lastHit); //medium difficulty bot turn (shoots randomly until it gets a hit)
int hardBotTurn(colour grid[10][10], COORDINATES *botCoords, int botCondition, COORDINATES *lastHit); //hard difficulty bot turn (shoots strategically until it gets a hit)
int advancedBotLogic(colour grid[10][10], COORDINATES *botCoords, int botCondition, COORDINATES *lastHit); //for the logic of the medium and hard bot fully sinking a ship after it gets a hit
void printResult(int botTurn, int playerTurn, COORDINATES botCoords, COORDINATES coords, SHIPS ship[2][5], int botShip, int playerShip); //print the result of the turn
int hitCounter(COORDINATES coords, SHIPS ship[2][5], int playerOrBot); //update hitcounter in ship struct by determining which ship was hit
int findDirection(int row1, int col1, int row2, int col2); //find the direction of the ship from first and second coordinates
void winScreen(int winner); //displays the win screen
void menu(); //display the menu screen
void endScreen(); //display the thank you for playing screen


int main()
{
    //LIST OF VARIABLES, AND DESCRIPTIONS.
    srand(time(0)); //new set of random numbers for the entire program
    colour grid[10][10]; //matrix containing info on the players battleship grid, this will help keep track of the colours for the player
    colour missilesFired[10][10]; //matrix containing info on missiles fired, this will help keep track of the colours for the player
    colour gridBot[10][10]; //matrix containing info on the players battleship grid, this will help keep track of the colours for the bot
    char placementChoice; //ask the user if they would like to manually place their ships or have it be done randomly
    int playerOrBot; //0 for player and 1 for bot (for randomize ships function)
    int firstMove; //to identify if we are at the first move
    int botTurn; //track if bot hit or missed in a given turn
    int playerTurn; //track if player hit or missed in a given turn
    char difficulty; //difficulty of the bot
    COORDINATES coords; //coordinates of players missile
    COORDINATES botCoords; //coordinates of bots missile
    COORDINATES lastHit; //to track the coordinates of the first missile that hit a certain ship to determine where to shoot next for medium and hard difficulty bots
    int result; //for result of checkWin function
    SHIPS ship[2][5]; //for information on the individual ships
    int playerHit; //5 if bot ship has not been hit this turn, else it returns the number of the ship hit
    int botHit; //5 if a player ship has not been hit this turn, else it returns the number of the ship hit
    char playAgain = 'y'; //play again 
    int botCondition = 0; //used to track what pattern the medium and hard bots should follow when dealing with a ship
    int targetShip; //records the number of the ship currently being targetted after a hit for medium and hard bot logic
    int specialCase = 0; //records the number of special case scenarios there are to come back to for medium and hard bot logic
    COORDINATES specialCaseCoords[4]; //records the coordinates of the ship that it hit in a special case for medium and hard bot logic
    int specialCaseTargetShip[4]; //records the number of the ship that it will return to in a special case for medium and hard bot logic
    
    menu(); //display the menu screen
    
    while(playAgain == 'y'){ 
        //initializing the variables
        playerOrBot = 1;
        firstMove = 1;
        result = 0;
        playerHit = 5;
        botHit = 5;
        lastHit.row = 0;
        lastHit.col = 0;
        specialCase = 0;
        for(int i = 0; i < 2; i++){
            ship[i][0].len = 5;
            strcpy(ship[i][0].name, "Carrier");
            ship[i][1].len = 4;
            strcpy(ship[i][1].name, "Battleship");
            ship[i][2].len = 3;
            strcpy(ship[i][2].name, "Cruiser");
            ship[i][3].len = 3;
            strcpy(ship[i][3].name, "Submarine");
            ship[i][4].len = 2;
            strcpy(ship[i][4].name, "Destroyer");
            for(int j = 0; j < 5; j++){
                ship[i][j].hitCounter = 0;
            }
        }
        
        initializeBoard(grid, missilesFired, gridBot); //clear the board
        randomizeShips(gridBot, playerOrBot, ship); //randomize the bots ships

        //input difficulty of game
        printf("What difficulty bot would you like to play against ('e' for easy, 'm' for medium, or 'h' for hard)? ");
        scanf("%c", &difficulty);
        while((getchar())!='\n'); //flushed the keyboard buffer
        while(difficulty != 'e' && difficulty != 'm' && difficulty != 'h'){ 
            printf("Please input a valid choice ('e' for easy, 'm' for medium, or 'h' for hard): ");
            scanf("%c", &difficulty);
            while((getchar())!='\n'); //flushed the keyboard buffer
        }

        //input choice between manual and random placement
        printf("Would you like to place your ships manually or have them randomly placed ('m' or 'r')? ");
        scanf("%c", &placementChoice);
        while((getchar())!='\n'); //flushed the keyboard buffer
        placementChoice = tolower(placementChoice); //lowercase to avoid errors due to case
        while(placementChoice != 'r' && placementChoice != 'm'){ //valid input check loop
            printf("Please input a valid choice ('m' for manual placement or 'r' for random placement): ");
            scanf("%c", &placementChoice);
            while((getchar())!='\n'); //flushed the keyboard buffer
            placementChoice = tolower(placementChoice); //lowercase to avoid errors due to case
        }
        
        if(placementChoice == 'r'){ //random placement option
            playerOrBot = 0;
            randomizeShips(grid, playerOrBot, ship);
        }
        else if(placementChoice == 'm'){ //manual placement option
            manualShips(grid, ship, missilesFired);
        }
        
        while(result == 0){ //while nobody has won the game
            printBoard(grid, missilesFired, ship);

            //to skip printing the results of the previous round on the first round
            if(firstMove == 0){
                printResult(botTurn, playerTurn, botCoords, coords, ship, playerHit, botHit);
            }
            firstMove = 0;

            //input valid missile coordinates from user
            printf("What coordinate would you like to fire a missile at next? (number (row) followed by letter (column), i.e. '1A'): ");
            coords = getCoordinate();
            while(validMissileCheck(coords, missilesFired) == 0){ //checking for validity of input and inputting a valid input
                printf("This isn't a valid coordinate or you have already fired a missile there, please try again: ");
                coords = getCoordinate();
            }
            
            playerTurn = fireMissile(coords, gridBot, missilesFired); //fire the missile
            playerHit = hitCounter(coords, ship, 1); //record if it is a hit
            
            if(difficulty == 'e'){ //easy difficulty bot turn
                botTurn = easyBotTurn(grid, &botCoords);
            }
            else if(difficulty == 'm'){ //medium difficulty bot turn
                botTurn = mediumBotTurn(grid, &botCoords, botCondition, &lastHit);
                if(botTurn == 2){ //mediumBotTurn returns 2 under a special case circumstance in which it requires me to make botCondition 3 for the bot logic after it hits a ship
                    botCondition = 3;
                    botTurn = mediumBotTurn(grid, &botCoords, botCondition, &lastHit); //redo the bots turn but now with botCondition set to 3
                }
            }
            else if(difficulty == 'h'){ //hard difficulty bot turn
                botTurn = hardBotTurn(grid, &botCoords, botCondition, &lastHit);
                if(botTurn == 2){ //hardotTurn returns 2 under a special case circumstance in which it requires me to make botCondition 3 for the bot logic after it hits a ship
                    botCondition = 3;
                    botTurn = hardBotTurn(grid, &botCoords, botCondition, &lastHit); //redo the bots turn but now with botCondition set to 3
                }
            }
            botHit = hitCounter(botCoords, ship, 0);
            
            //for medium and hard bot logic to sink a ship after it gets a hit
            if(difficulty != 'e'){ //while difficulty is medium or hard
                if(botHit != 5){ //if bot gets a hit
                    if(botCondition == 0){ //if its the first hit on that ship, botCondition is set to 1
                        botCondition = 1; //begins the process of eliminating the entire ship
                        lastHit.row = botCoords.row; //records the row of the first hit
                        lastHit.col = botCoords.col; //records the column of the first hit
                        targetShip = botHit; //records which ship was hit
                    }
                    else if(botCondition == 1){ //botCondition 1 is to find the direction of the ship
                        if(botHit != targetShip){ //if another ship is found while searching around the target ship, that is recorded into memory
                            specialCase++; //counter for how many special case ships have been found
                            specialCaseCoords[specialCase - 1].row = botCoords.row;
                            specialCaseCoords[specialCase - 1].col = botCoords.col;
                            specialCaseTargetShip[specialCase - 1] = botHit;
                        }
                        else{ //when botCondition is 1 and the bot gets another hit, the condition is set to 2
                            botCondition = 2; //botCondition 2 is to extend further once the direction is found to eliminate the rest of the ship
                        }
                    }
                    else if(botCondition == 2 && botHit != targetShip){ //if another ship is found during botCondition 2, that is taken into memoery and will be returned to
                        specialCase++;
                        specialCaseCoords[specialCase - 1].row = botCoords.row;
                        specialCaseCoords[specialCase - 1].col = botCoords.col;
                        specialCaseTargetShip[specialCase - 1] = botHit;
                    }
                    else if(botCondition == 3){ //botCondition 3 is to change the direction of the search for the rest of that ship
                        botCondition = 4; //botCondition 4 is to sink the rest of the ship that was found after botCondition 3
                    }
                    if(ship[0][targetShip].hitCounter == ship[0][targetShip].len){ //check if ship is sunk
                            botCondition = 0; //send the bot back to random searching (or random but following a pattern in the case of the hard bot)
                            if(targetShip == specialCaseTargetShip[specialCase - 1]){ //if it was one of the special case ships that was found, the counter is reduced by 1
                                specialCase--;
                            }
                    }
                }
                else if(botCondition == 2){ //if the botCondition is 2 and the bot doesnt get a hit, that means it is time to shoot on the opposite side of the first hit on that ship
                    botCondition = 3;
                }
                if(specialCase > 0 && botCondition == 0){ //to attack the ships found during the sinking of the previous ship, the ships found become the new target ships
                    botCondition = 1;
                    lastHit.row = specialCaseCoords[specialCase - 1].row;
                    lastHit.col = specialCaseCoords[specialCase - 1].col;
                    targetShip = specialCaseTargetShip[specialCase - 1];
                }
            } 
            result = checkWin(grid, gridBot); //check whether either the player or bot has won the game
        }
        printBoard(grid, missilesFired, ship); //display final missile hit before win screen
        winScreen(result); //display the win screen
        printf("\n                                         Would you like to play again (y/n)?\n"); //ask the user if they want to play again
        playAgain = getchar();
        while((getchar())!='\n'); //flushed the keyboard buffer
    }
    endScreen(); //display end screen if they dont want to play again
}

void initializeBoard(colour grid[10][10], colour missilesFired[10][10], colour gridBot[10][10]){ //initializing all of the values in both matrices to BLUE
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            grid[i][j] = BLUE;
            missilesFired[i][j] = BLUE;
            gridBot[i][j] = BLUE;
        }
    }
}

void printBoard(colour grid[10][10], colour missilesFired[10][10], SHIPS ship[2][5]){ //print the current state of the board
    clearTerminal();
    printf("\n           Your Battleship Grid                                     Missiles Fired                                        Ships\n");
    printf("                                                                                                           Player:\n");
    if(ship[0][0].hitCounter == ship[0][0].len){
        printf("  A   B   C   D   E   F   G   H   I   J                A   B   C   D   E   F   G   H   I   J                        Carrier: Sunk\n");
    }
    else{
        printf("  A   B   C   D   E   F   G   H   I   J                A   B   C   D   E   F   G   H   I   J                        Carrier: Standing\n");
    }
    for(int i = 0; i < 10; i++){
        clearANSI();
        printf("%d ", i);
        for(int j = 0; j < 10; j++){
            colourANSI(grid[i][j], grid[i][j]);
            printf("  ");
            colourANSI(BLACK, BLACK);
            printf("  ");
        }
        clearANSI();
        printf("           ");
        printf("%d ", i);
        for(int j = 0; j < 10; j++){
            colourANSI(missilesFired[i][j], missilesFired[i][j]);
            printf("  ");
            colourANSI(BLACK, BLACK);
            printf("  ");
        }
        clearANSI();
        printf("\n  ");
        colourANSI(BLACK, BLACK);
        printf("                                        ");
        clearANSI();
        printf("             ");
        colourANSI(BLACK, BLACK);
        printf("                                        ");
        clearANSI();
        if(i < 4){
            if(ship[0][i + 1].hitCounter == ship[0][i + 1].len){
                printf("                     %s: Sunk", ship[0][i + 1].name);
            }
            else{
                printf("                     %s: Standing", ship[0][i + 1].name);
            }
        }
        else if(i == 4){
            printf("               Bot:");
        }
        else if(i > 4){
            if(ship[1][i - 5].hitCounter == ship[1][i - 5].len){
                printf("                     %s: Sunk", ship[1][i - 5].name);
            }
            else{
                printf("                     %s: Standing", ship[1][i - 5].name);
            }
        }
        printf("\n");
        delay(0.01); //delay to avoid bugs in the printing of the board
    }
    printf("\n");
}

struct COORDINATES getCoordinate(){ //inputting coordinates of missile from player
    char coords[100], colCharacter;
    int rowNum, colNum;
    
    scanf("%2s", coords); //inputs only the first two characters
    while((getchar())!='\n'); // flushed the keyboard buffer
    sscanf(coords, "%d%c", &rowNum, &colCharacter); //parses the string for the row number and column character
    colCharacter = toupper(colCharacter); //converting the letter to uppercase in case it might be lowercase
    while(colCharacter > 'J' || colCharacter < 'A'){ //checks for valid input of column character, valid input of row number is already forced due to nature of scanf used previously
        printf("This isn't a valid coordinate or you have already used this coordinate, please try again: ");
        scanf("%s", coords);
        while((getchar())!='\n'); // flushed the keyboard buffer
        sscanf(coords, "%d%c", &rowNum, &colCharacter);
        colCharacter = toupper(colCharacter); //converting the letter to uppercase in case it might be lowercase
    }
    
    //converting the column letter to a number
    for(int i = 0; i < 10; i++){
        if(columnLetters[i] == colCharacter){
            colNum = i;
        }
    }
    //returning a struct containing a value for both the row and the column
    COORDINATES coordinates = {rowNum, colNum};
    return coordinates;
}

int validMissileCheck(COORDINATES coords, colour missilesFired[10][10]){ //checking for the validity of the missile
    if((coords.row < 0 || coords.row > 9) || (coords.col < 0 || coords.col > 9)){ //checking if input is in the range
        return 0;
    }
    else if(missilesFired[coords.row][coords.col] == BLUE){ //checking if a missile has been fired there before
        return 1;
    }
    else{
        return 0;
    }
}

int fireMissile(COORDINATES coords, colour board[10][10], colour missilesFired[10][10]){ //to shoot a missile
    if(board[coords.row][coords.col] == GREEN){
        board[coords.row][coords.col] = RED;
        missilesFired[coords.row][coords.col] = RED;
        return 1;
    }
    else{
        missilesFired[coords.row][coords.col] = WHITE;
        return 0;
    }
}

int checkWin(colour grid[10][10], colour gridBot[10][10]){ //check if either player has won
    int playerWinCondition = 1, botWinCondition = 1;
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            if(grid[i][j] == GREEN){
                botWinCondition = 0;
            }
            if(gridBot[i][j] == GREEN){
                playerWinCondition = 0;
            }
        }
    }
    if(playerWinCondition == 1){
        return 1;
    }
    else if(botWinCondition == 1){
        return 2;
    }
    else{
        return 0;
    }
}

void randomizeShips(colour board[10][10], int playerOrBot, SHIPS ship[2][5]){ //randomly place all the ships
    int direction = 0; //will randomly generate between 0 and 3 to determine which direction the ship extends towards
    for(int i = 0; i < 5; i++){
        //randomly generate the first point of the ship
        ship[playerOrBot][i].coordinates[0].row = rand() % 10;
        ship[playerOrBot][i].coordinates[0].col = rand() % 10;
        while(validPointCheck(ship[playerOrBot][i].coordinates[0].row, ship[playerOrBot][i].coordinates[0].col, ship[playerOrBot][i].len, board) == 0){
            ship[playerOrBot][i].coordinates[0].row = rand() % 10;
            ship[playerOrBot][i].coordinates[0].col = rand() % 10;
        }

        //set row2 and col2 to invalid coordinates so that valid placement check returns 0 and the loop begins, this helps remove some repition in the code
        ship[playerOrBot][i].coordinates[ship[playerOrBot][i].len - 1].row = 100;
        ship[playerOrBot][i].coordinates[ship[playerOrBot][i].len - 1].col = 100;
        
        while(validPlacementCheck(ship[playerOrBot][i].coordinates[0].row, ship[playerOrBot][i].coordinates[0].col, ship[playerOrBot][i].coordinates[ship[playerOrBot][i].len - 1].row, ship[playerOrBot][i].coordinates[ship[playerOrBot][i].len - 1].col, direction, board, ship[playerOrBot][i].len) == 0){
            //randomly choose which direction the ship extends after defining the first point
            direction = rand() % 4;
            shipCoordinateFiller(direction, ship, i, playerOrBot);
        }
        placeShip(ship[playerOrBot][i].coordinates[0].row, ship[playerOrBot][i].coordinates[0].col, ship[playerOrBot][i].coordinates[ship[playerOrBot][i].len - 1].row, ship[playerOrBot][i].coordinates[ship[playerOrBot][i].len - 1].col, direction, board); //finally place the ship
    }
}

void manualShips(colour grid[10][10], SHIPS ship[2][5], colour missilesFired[10][10]){ //for the manual placement of ships
    COORDINATES placementCoordsOne;
    COORDINATES placementCoordsTwo;
    char placementInputOne[5][200] = {"Pick the first coordinate of your Carrier (5-long) ship (number (row) followed by letter (column), i.e. '1A'): ", "Pick the first coordinate of your Battleship (4-long) ship (number (row) followed by letter (column), i.e. '1A'): ", "Pick the first coordinate of your Cruiser (3-long) ship (number (row) followed by letter (column), i.e. '1A'): ", "Pick the first coordinate of your Submarine (3-long) ship (number (row) followed by letter (column), i.e. '1A'): ", "Pick the first coordinate of your Destroyer (2-long) ship (number (row) followed by letter (column), i.e. '1A'): "}; //for the output in manual placement option
    char placementInputTwo[5][200] = {"Pick the second coordinate of your Carrier (5-long) ship (number (row) followed by letter (column), i.e. '1A'): ", "Pick the second coordinate of your Battleship (4-long) ship (number (row) followed by letter (column), i.e. '1A'): ", "Pick the second coordinate of your Cruiser (3-long) ship (number (row) followed by letter (column), i.e. '1A'):", "Pick the second coordinate of your Submarine (3-long) ship (number (row) followed by letter (column), i.e. '1A'): ", "Pick the second coordinate of your Destroyer (2-long) ship (number (row) followed by letter (column), i.e. '1A'): "}; //for the output in manual placement option
    int direction;
    
    printBoard(grid, missilesFired, ship);
   
    for(int i = 0; i < 5; i++){
        printf("%s", placementInputOne[i]);
        placementCoordsOne = getCoordinate();
        while(validPointCheck(placementCoordsOne.row, placementCoordsOne.col, ship[0][i].len, grid) != 1){
            printf("Please input a valid coordinate: ");
            placementCoordsOne = getCoordinate();
        }
        
        printf("%s", placementInputTwo[i]);
        placementCoordsTwo = getCoordinate();
        while(validPointCheck(placementCoordsTwo.row, placementCoordsTwo.col, ship[0][i].len, grid) != 1){
            printf("Please input a valid coordinate: ");
            placementCoordsTwo = getCoordinate();
        } 
        
        direction = findDirection(placementCoordsOne.row, placementCoordsOne.col, placementCoordsTwo.row, placementCoordsTwo.col);
        ship[0][i].coordinates[0].row = placementCoordsOne.row;
        ship[0][i].coordinates[0].col = placementCoordsOne.col;
        
        while(direction == 4 || validPlacementCheck(placementCoordsOne.row, placementCoordsOne.col, placementCoordsTwo.row, placementCoordsTwo.col, direction, grid, ship[0][i].len) != 1){
            printf("Please think your coordinates through (consider ship lengths), and pick valid coordinates to place your ship!\n");
            printf("%s", placementInputOne[i]);
            placementCoordsOne = getCoordinate();
            while(validPointCheck(placementCoordsOne.row, placementCoordsOne.col, ship[0][i].len, grid) != 1){
                printf("Please input a valid coordinate: ");
                placementCoordsOne = getCoordinate();
            }
            
            printf("%s", placementInputTwo[i]);
            placementCoordsTwo = getCoordinate();
            while(validPointCheck(placementCoordsTwo.row, placementCoordsTwo.col, ship[0][i].len, grid) != 1){
                printf("Please input a valid coordinate: ");
                placementCoordsTwo = getCoordinate();
            }
            direction = findDirection(placementCoordsOne.row, placementCoordsOne.col, placementCoordsTwo.row, placementCoordsTwo.col);
            ship[0][i].coordinates[0].row = placementCoordsOne.row;
            ship[0][i].coordinates[0].col = placementCoordsOne.col;
        }
        shipCoordinateFiller(direction, ship, i, 0);
        placeShip(placementCoordsOne.row, placementCoordsOne.col, placementCoordsTwo.row, placementCoordsTwo.col, direction, grid);
        printBoard(grid, missilesFired, ship);
    }
}

void shipCoordinateFiller(int direction, SHIPS ship[2][5], int shipNum, int playerOrBot){ //fills in the rest of the coordinates for the ship struct
    if(direction == 0){ //direction up
        for(int j = 1; j < ship[playerOrBot][shipNum].len; j++){
            ship[playerOrBot][shipNum].coordinates[j].row = ship[playerOrBot][shipNum].coordinates[0].row - j;
            ship[playerOrBot][shipNum].coordinates[j].col = ship[playerOrBot][shipNum].coordinates[0].col;
        }
    }
    else if(direction == 1){ //direction right
        for(int j = 1; j < ship[playerOrBot][shipNum].len; j++){
            ship[playerOrBot][shipNum].coordinates[j].row = ship[playerOrBot][shipNum].coordinates[0].row;
            ship[playerOrBot][shipNum].coordinates[j].col = ship[playerOrBot][shipNum].coordinates[0].col + j;
        }
    }
    else if(direction == 2){ //direction down
        for(int j = 1; j < ship[playerOrBot][shipNum].len; j++){
            ship[playerOrBot][shipNum].coordinates[j].row = ship[playerOrBot][shipNum].coordinates[0].row + j;
            ship[playerOrBot][shipNum].coordinates[j].col = ship[playerOrBot][shipNum].coordinates[0].col;
        }
    }
    else if(direction == 3){ //direction left
        for(int j = 1; j < ship[playerOrBot][shipNum].len; j++){
            ship[playerOrBot][shipNum].coordinates[j].row = ship[playerOrBot][shipNum].coordinates[0].row;
            ship[playerOrBot][shipNum].coordinates[j].col = ship[playerOrBot][shipNum].coordinates[0].col - j;
        }
    }
}

int validPlacementCheck(int row1, int col1, int row2, int col2, int direction, colour board[10][10], int len){ //checking if the placement is valid
    int condition = 1;
    
    if((row2 < 0 || row2 > 9) || (col2 < 0 || col2 > 9)){ //check if the second point is on the grid
        condition = 0;
    }
    else{ //check if a ship passes between the first and second point
        if(direction == 0){ //direction up
            if(row1 - row2 != len - 1){
                condition = 0;
            }
            for(int i = row2; i <= row1; i++){
                if(board[i][col1] == GREEN){
                    condition = 0;
                }
            }
        }
        else if(direction == 1){ //direction right
            if(col2 - col1 != len - 1){
                condition = 0;
            }
            for(int i = col1; i <= col2; i++){
                if(board[row1][i] == GREEN){
                    condition = 0;
                }
            }
        }
        else if(direction == 2){ //direction down
            if(row2 - row1 != len - 1){
                condition = 0;
            }
            for(int i = row1; i <= row2; i++){
                if(board[i][col1] == GREEN){
                    condition = 0;
                }
            }
        }
        else if(direction == 3){ //direction left
            if(col1 - col2 != len - 1){
                condition = 0;
            }
            for(int i = col2; i <= col1; i++){
                if(board[row1][i] == GREEN){
                    condition = 0;
                }
            }
        }
    }
    return condition;
}

int validPointCheck(int row, int col, int len, colour board[10][10]){ //check if a point on the grid is valid to be placed on or already occupied
    int condition = 1; //using a variable to track validity
    
    if((row < 0 || row > 9) || (col < 0 || col > 9)){
        condition = 0;
    }
    else if(board[row][col] == GREEN){
        condition = 0;
    }
    else{ //checks if the point has the capability for the specific size of ship to be place
        int upCondition = 1;
        int rightCondition = 1;
        int downCondition = 1;
        int leftCondition = 1;
        for(int i = 1; i < len; i++){
            if(board[row - i][col] == GREEN || (row - i < 0 || row - i > 9)){
                upCondition = 0;
            }
            if(board[row][col + i] == GREEN || (col + i < 0 || col + i > 9)){
                rightCondition = 0;
            }
            if(board[row + i][col] == GREEN || (row + i < 0 || row + i > 9)){
                downCondition = 0;
            }
            if(board[row][col - i] == GREEN || (col - i < 0 || col - i > 9)){
                leftCondition = 0;
            }
        }
        if(upCondition + rightCondition + downCondition + leftCondition == 0){
            condition = 0;
        }
    }
    return condition;
}

void placeShip(int row1, int col1, int row2, int col2, int direction, colour board[10][10]){ //place the ship by coloring the board
    if(direction == 0){ //direction up
        for(int i = row2; i <= row1; i++){
            board[i][col1] = GREEN;
        }
    }
    else if(direction == 1){ //direction right
        for(int i = col1; i <= col2; i++){
            board[row1][i] = GREEN;
        }
    }
    else if(direction == 2){ //direction down
        for(int i = row1; i <= row2; i++){
            board[i][col1] = GREEN;
        }
    }
    else if(direction == 3){ //direction left
        for(int i = col2; i <= col1; i++){
            board[row1][i] = GREEN;
        }
    }
}

int easyBotTurn(colour grid[10][10], COORDINATES *botCoords){ //easy difficulty bot turn (shoots randomly)
    int row = rand() % 10;
    int col = rand() % 10;
    while(grid[row][col] == RED || grid[row][col] == WHITE){
        row = rand() % 10;
        col = rand() % 10;
    }
    
    botCoords->row = row; //passing struct by reference shenanigans
    botCoords->col = col; //passing struct by reference shenanigans
    
    if(grid[row][col] == GREEN){
        grid[row][col] = RED;
        return 1; //1 meaning hit
    }
    else{
        grid[row][col] = WHITE;
        return 0; //0 meaning miss
    }
}

int mediumBotTurn(colour grid[10][10], COORDINATES *botCoords, int botCondition, COORDINATES *lastHit){ //medium difficulty bot turn (shoots randomly until it gets a hit)
    if(botCondition != 0){
        return advancedBotLogic(grid, botCoords, botCondition, lastHit);
    }
    else{
        int row, col;
        row = rand() % 10;
        col = rand() % 10;
        while(grid[row][col] == RED || grid[row][col] == WHITE){
            row = rand() % 10;
            col = rand() % 10;
        }
        botCoords->row = row; //passing struct by reference shenanigans
        botCoords->col = col; //passing struct by reference shenanigans
        if(grid[row][col] == GREEN){
            grid[row][col] = RED;
            return 1; //1 meaning hit
        }
        else{
            grid[row][col] = WHITE;
            return 0; //0 meaning miss
        }
    } 
}

int hardBotTurn(colour grid[10][10], COORDINATES *botCoords, int botCondition, COORDINATES *lastHit){ //hard difficulty bot turn (shoots strategically until it gets a hit)
    //optimal pattern for shooting missiles, I think, goes in diagonals to avoid redundant missiles
    if(botCondition != 0){
        return advancedBotLogic(grid, botCoords, botCondition, lastHit);
    }
    else{
        int col = rand() % 10;
        int row = rand() % 10;
        if(row % 2 == 0){
            while(col % 2 == 0){
                col = rand() % 10;
            }
        }
        else if(row % 2 == 1){
            while(col % 2 == 1){
                col = rand() % 10;
            }
        }
        while(grid[row][col] == RED || grid[row][col] == WHITE){
            col = rand() % 10;
            row = rand() % 10;
            if(row % 2 == 0){
                while(col % 2 == 0){
                    col = rand() % 10;
                }
            }
            else if(row % 2 == 1){
                while(col % 2 == 1){
                    col = rand() % 10;
                }
            }
        }
        botCoords->row = row; //passing struct by reference shenanigans
        botCoords->col = col; //passing struct by reference shenanigans
        if(grid[row][col] == GREEN){
            grid[row][col] = RED;
            return 1; //1 meaning hit
        }
        else{
            grid[row][col] = WHITE;
            return 0; //0 meaning miss
        }
    }
}

int advancedBotLogic(colour grid[10][10], COORDINATES *botCoords, int botCondition, COORDINATES *lastHit){ //for the logic of the medium and hard bot fully sinking a ship after it gets a hit
    int row, col;
    
    if(botCondition == 1){
        if(lastHit->col + 1 <= 9 && (grid[lastHit->row][lastHit->col + 1] == BLUE || grid[lastHit->row][lastHit->col + 1] == GREEN)){
            row = lastHit->row;
            col = lastHit->col + 1;
        }
        else if(lastHit->col - 1 >= 0 && (grid[lastHit->row][lastHit->col - 1] == BLUE || grid[lastHit->row][lastHit->col - 1] == GREEN)){
            row = lastHit->row;
            col = lastHit->col - 1;
        }
        else if(lastHit->row + 1 <= 9 && (grid[lastHit->row + 1][lastHit->col] == BLUE || grid[lastHit->row + 1][lastHit->col] == GREEN)){
            row = lastHit->row + 1;
            col = lastHit->col;
        }
        else if(lastHit->row - 1 >= 0 && (grid[lastHit->row - 1][lastHit->col] == BLUE || grid[lastHit->row - 1][lastHit->col] == GREEN)){
            row = lastHit->row - 1;
            col = lastHit->col;
        }
        botCoords->row = row; //passing struct by reference shenanigans
        botCoords->col = col; //passing struct by reference shenanigans
        if(grid[row][col] == GREEN){
            grid[row][col] = RED;
            return 1; //1 meaning hit
        }
        else{
            grid[row][col] = WHITE;
            return 0; //0 meaning miss
        }
    }
    else if(botCondition == 2 || botCondition == 4){
        if(lastHit->col - botCoords->col < 0 && (grid[botCoords->row][botCoords->col + 1] == BLUE || grid[botCoords->row][botCoords->col + 1] == GREEN) && botCoords->col + 1 <= 9){
            row = botCoords->row;
            col = botCoords->col + 1;
        }
        else if(lastHit->col - botCoords->col > 0 && (grid[botCoords->row][botCoords->col - 1] == BLUE || grid[botCoords->row][botCoords->col - 1] == GREEN) && botCoords->col - 1 >= 0){
            row = botCoords->row;
            col = botCoords->col - 1;
        }
        else if(lastHit->row - botCoords->row < 0 && (grid[botCoords->row + 1][botCoords->col] == BLUE || grid[botCoords->row + 1][botCoords->col] == GREEN) && botCoords->row + 1 <= 9){
            row = botCoords->row + 1;
            col = botCoords->col;
        }
        else if(lastHit->row - botCoords->row > 0 && (grid[botCoords->row - 1][botCoords->col] == BLUE || grid[botCoords->row - 1][botCoords->col] == GREEN) && botCoords->row - 1 >= 0){
            row = botCoords->row - 1;
            col = botCoords->col;
        }
        else{ //this happens if the place to be shot was already shot at
            return 2; //special case scenario
        }
        botCoords->row = row; //passing struct by reference shenanigans
        botCoords->col = col; //passing struct by reference shenanigans
        if(grid[row][col] == GREEN){
            grid[row][col] = RED;
            return 1; //1 meaning hit
        }
        else{
            grid[row][col] = WHITE;
            return 0; //0 meaning miss
        }
    }
    else{
        if(lastHit->col - botCoords->col < 0){
            row = lastHit->row;
            col = lastHit->col - 1;
        }
        else if(lastHit->col - botCoords->col > 0){
            row = lastHit->row;
            col = lastHit->col + 1;
        }
        else if(lastHit->row - botCoords->row < 0){
            col = lastHit->col;
            row = lastHit->row - 1;
        }
        else if(lastHit->row - botCoords->row > 0){
            col = lastHit->col;
            row = lastHit->row + 1;
        }
        botCoords->row = row; //passing struct by reference shenanigans
        botCoords->col = col; //passing struct by reference shenanigans
        if(grid[row][col] == GREEN){
            grid[row][col] = RED;
            return 1; //1 meaning hit
        }
        else{
            grid[row][col] = WHITE;
            return 0; //0 meaning miss
        }
    }
}

void printResult(int botTurn, int playerTurn, COORDINATES botCoords, COORDINATES coords, SHIPS ship[2][5], int botShip, int playerShip){ //print the result of the turn
    char playerCol; //number to letter of players missile
    char botCol; //number to letter of bots missile
    
    playerCol = columnLetters[coords.col];
    botCol = columnLetters[botCoords.col];
    
    if(botTurn == 0){
        printf("The bot fired a missile at %d%c and missed!", botCoords.row, botCol);
    }
    else if(botTurn == 1){
        printf("The bot fired a missile at %d%c and hit your %s!", botCoords.row, botCol, ship[0][playerShip].name);
    }
    printf("             ");
    if(playerTurn == 0){
        printf("You fired a missile at %d%c and missed!\n\n", coords.row, playerCol);
    }
    else if(playerTurn == 1){
        printf("You fired a missile at %d%c and hit the bot's %s!\n\n", coords.row, playerCol, ship[1][botShip].name);
    }
}

int hitCounter(COORDINATES coords, SHIPS ship[2][5], int playerOrBot){ //update hitcounter in ship struct by determining which ship was hit
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < ship[playerOrBot][i].len; j++){
            if(coords.row == ship[playerOrBot][i].coordinates[j].row && coords.col == ship[playerOrBot][i].coordinates[j].col){
                ship[playerOrBot][i].hitCounter++;
                return i;
            }
        }
    }
    return 5;
}

int findDirection(int row1, int col1, int row2, int col2){ //find the direction of the ship from first and second coordinates
    int direction = 4; //returning 4 would mean the direction cant be found because invalid coordinates
    
    if(row1 - row2 == 0){ //check if direction is horizontal
        if(col1 > col2){ //check if direction is left
            direction = 3;
        }
        else if(col1 < col2){ //check if direction is right
            direction = 1;
        }
    }
    else if(col1 - col2 == 0){ 
        if(row1 > row2){ //check if direction is up
            direction = 0;
        }
        else if(row1 < row2){ //check if direction is down
            direction = 2;
        }
    }
    return direction;
}

void winScreen(int winner){ //displays the win screen
    FILE *infile;
    char art[300];
    char win[30];

    if(winner == 1){
        strcpy(win, "player");
    }
    else if(winner == 2){
        strcpy(win, "bot");
    }

    infile = fopen( "winScreen.txt", "r"); //contains win screen art
    clearTerminal();
    
    for(int i = 0; i < 36; i++){ 
        for(int j = 0; j < 30; j++){
            fgets(art, 200, infile);
            printf("%s", art);
            if(i == 35 && j == 4){
                printf("                                                   The %s has won!", win);
            }
        }
        delay(0.06);
        if(i != 35){
            clearTerminal();
        }
    }
    fclose(infile);
}

void menu(){ //display the menu screen
    FILE *infile;
    char art[300];
    char continueToGame;

    infile = fopen("art.txt", "r"); //continues menu art
    
    clearTerminal();
    for(int i = 0; i < 8; i++){ 
        fgets(art, 300, infile);
        printf("%s", art);
    }
    printf("\n\nGame description and rules: \nEach player sets up their side by placing the given battleships of varying sizes on their grid. \nThen the players take turns calling out coordinates on the grid on which they think the opponent has a battleship. \nIf a battleship is hit, the opponent responds with 'hit,' but if the shot lands in the ocean, the opponent responds with 'miss.' \nWhen all the squares a ship occupies on the grid are hit, the ship is considered sunk, and the player must announce that a ship was sunk. \nThe first player to sink all of their opponent's ships wins.\n");
    printf("\nPLAY IN FULL SCREEN FOR BEST EXPERIENCE AND TO AVOID VISUAL BUGS!\n");
    printf("\nInput anything when you would like to continue to the game: ");
    continueToGame = getchar();
    while((getchar())!='\n'); //flushed the keyboard buffer
    clearTerminal();
    fclose(infile);
}

void endScreen(){ //display thank you for playing screen
    FILE *infile;
    char art[300];

    infile = fopen("art2.txt", "r"); //contains end screen art
    
    clearTerminal();
    for(int i = 0; i < 36; i++){ 
        fgets(art, 200, infile);
        printf("%s", art);
    }
    fclose(infile);
}