//  Summary of formatting commands in this library.
//  Format Options:  RESET, BOLD, DIM, ITALIC, UNDERLINE, STRIKETHROUGH
//  Colour Options:  BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, DEFAULT
//  Funtions:
//  colourANSI(colour C);     Sets colour.  Choose from colour option above
//  colourANSI(colour C, colour BG);  Sets text colour, and background colour.
//  formatANSI(format F);     Sets format.  Choose from format option above
//  clearANSI();              Clears all formatting commands.
//  setCursor(int x, int y);  Sets cursor position.  Position 1,1 is top left.
//  clearTerminal();          Clears the screen     
//  delay(double sec);        Pauses the program for a number of seconds.

#include <iostream>
#include <unistd.h> // Required for usleep un the sleep function.
using namespace std;

enum format{
  RESET = 0,
  BOLD,
  DIM,
  ITALIC,
  UNDERLINE,
  BLINKING,//incompatible with current juicemind terminal
  REVERSE, //incompatible with current juicemind terminal
  HIDDEN,  //works weirdly with the juicemind terminal
  STRIKETHROUGH
};

enum colour{
  BLACK = 0,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE,
  DEFAULT = 39
};

//all functions return a null charachter so they can be inserted in cout statments without errors
//E.G. cout << "123? " << colourANSI(GREEN) << formatANSI(UNDERLINE) << "456" << endl;
//printANSI definitions
char colourANSI(colour C){
  printf("\x1B[%dm", (C + 30));
  return '\0';
}
char colourANSI(colour C, colour BG){
  printf("\x1B[%d;%dm", (BG + 40), (C + 30));
  return '\0';
}

// Sets format style.  See 'format' enum type above for options.
char formatANSI(format F){
  printf("\x1B[%dm", F);
  return '\0';
}

// Clears all formatting commands.
char clearANSI(){
  printf("\x1B[0m");
  return '\0';
}

//setCursor puts the terminate cursor to a specific set of coordinates, starting at 1, 1
void setCursor(int x, int y){
  printf("\033[%d;%dH", x, y);
}

// Clears the screen. 
void clearTerminal(){
  printf("\033[2J\033[1;1H");
}


void delay(double sec){
    usleep(sec * 1000000); // Pause for 'sec' seconds
}