// C++ code
//
#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>

#define PIN 5
#define NUMPIXELS 36

void resetGame();

char keys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[4] = {13, 12, 11, 10}; 
byte colPins[4] = {9, 8, 7, 6}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

class StripController{
  private:
  Adafruit_NeoPixel strip;
  int pixelMatrix[6][6] = {
  {30, 31, 32, 33, 34, 35},
  {24, 25, 26, 27, 28, 29},
  {18, 19, 20, 21, 22, 23},
  {12, 13, 14, 15, 16, 17},
  { 6,  7,  8,  9, 10, 11},
  { 0,  1,  2,  3,  4,  5},
  };
  
  public:
  StripController(): strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800){}
  
  void begin(){
    strip.begin();
    strip.clear();
    strip.show();
  }
  
  void setPixel(int row, int col, int r, int g, int b){
    strip.setPixelColor(pixelMatrix[row - 1][col - 1], strip.Color(r, g, b));
    strip.show();
  }
  
  void rainbow(int cycles){
    for(int shift = 0; shift < cycles * 36; shift++){
      for(int col = 0; col < 6; col++){
        int color = (col + shift) % 36;
        int r, g, b;
        if(color < 6){
          r = 255;
          g = color * 51;
          b = 0;
        }
        else if(color < 12){
          r = (11 - color) * 51;
          g = 255;
          b = 0;
        }
        else if(color < 18){
          r = 0;
          g = 255;
          b = (color - 12) * 51;
        }
        else if(color < 24){
          r = 0;
          g = (23 - color) * 51;
          b = 255;
        }
        else if(color < 30){
          r = (color - 24) * 51;
          g = 0;
          b = 255;
        }
        else{
          r = 255;
          g = 0;
          b = (35 - color) * 51;
        }
        for(int row = 0; row < 6; row++){
          strip.setPixelColor(
            pixelMatrix[row][col],
            strip.Color(r, g, b)
          );
        }
      }
      strip.show();
      delay(50);
  	}
  }
  
  void setAll(int r, int g, int b){
    strip.clear();
    for (int row = 0; row < 6; ++row){
      for (int col = 0; col < 6; ++col){
        strip.setPixelColor(pixelMatrix[row][col], strip.Color(r, g, b));
      }
    }
    strip.show();
  }
  
  void clear(){
    strip.clear();
    strip.show();
  }
};

class LcdController{
  private:
  String line1 = "";
  String line2 = "";
  Adafruit_LiquidCrystal lcd;
  
  public:
  LcdController() : lcd(0){}
  
  void begin(){
    lcd.begin(16, 2);
  }
  
  void print(String line1, String line2){
    this->line1 = line1;
    this->line2 = line2;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
  
  void briefPrint(String line1, String line2, int time){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    delay(time);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(this->line1);
    lcd.setCursor(0, 1);
    lcd.print(this->line2);
  }
};

class CollisionMap{
  private:
  int collisionMatrix[6][6] = {{0}};
  StripController &sc;
  
  public:
  CollisionMap(StripController &sc, int collisionMatrix[6][6]) : sc(sc){
    for (int row = 0; row < 6; ++row){
      for (int col = 0; col < 6; ++col){
        this->collisionMatrix[row][col] = collisionMatrix[row][col];
      }
    }
  }
  
  void draw(){
    for (int row = 0; row < 6; ++row){
      for (int col = 0; col < 6; ++col){
        if(collisionMatrix[row][col] == 1){
          sc.setPixel(row + 1, col + 1, 255, 0, 0);
        }
      }
    }
  }
  
  void hide(){
    for (int row = 0; row < 6; ++row){
      for (int col = 0; col < 6; ++col){
        if(collisionMatrix[row][col] == 1){
          sc.setPixel(row + 1, col + 1, 0, 0, 0);
        }
      }
    }
  }
  
  bool hasCollision(int x, int y){
    if(collisionMatrix[x - 1][y - 1] == 1){
      return true;
    }
    return false;
  }
};

class Entity{
  private:
  int x = 1;
  int y = 1;
  int health = 100;
  StripController &sc;
  LcdController &lc;
  CollisionMap *cm;
  int winX;
  int winY;
  
  int r, g, b;
  
  public:
  Entity(StripController &sc, LcdController &lc, CollisionMap &cm, 
         int winX, int winY, int r, int g, int b) : sc(sc), lc(lc), 
  		 cm(&cm), winX(winX), winY(winY), r(r), g(g), b(b){}
  
  void reset(){
    x = 1;
    y = 1;
    health = 100;
  }
  
  void draw(){
    sc.setPixel(x, y, r, g, b);
  }
  
  void hide(){
    sc.setPixel(x, y, 0, 0, 0);
  }
  
  void setLocation(int x, int y){
    if(x <= 6 && x >= 1){
      this->x = x;
    }
    if(y <= 6 && y >= 1){
      this->y = y;
    }
  }
  
  void moveX(int amount){
    if(x + amount <= 6 && x + amount >= 1 && !cm->hasCollision(x + amount, y)){
      x += amount;
      if(x == winX && y == winY){
        sc.rainbow(1);
        resetGame();
      }
    }
  }
  
  void moveY(int amount){
    if(y + amount <= 6 && y + amount >= 1 && !cm->hasCollision(x, y + amount)){
      y += amount;
      if(x == winX && y == winY){
        sc.rainbow(1);
        resetGame();
      }
    }
  }
  
  void changeCollisionMap(void *newMap){
    cm = static_cast<CollisionMap*>(newMap);
    if(cm->hasCollision(x, y) == true){
      resetGame();
    }
  }
  
  void printHealth(int time){
    lc.briefPrint("Player health:", String(health), time);
  }
};

StripController sc;
LcdController lc;
int lightMapMatrix[6][6] = {
  {0, 1, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0},
  {0, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0},
  {1, 1, 0, 1, 0, 0},
};
int darkMapMatrix[6][6] = {
  {0, 1, 0, 0, 1, 1},
  {0, 1, 0, 0, 0, 0},
  {0, 1, 1, 1, 0, 0},
  {1, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0},
  {1, 1, 0, 1, 0, 0},
};
bool isLight = false;
CollisionMap cmLight(sc, lightMapMatrix);
CollisionMap cmDark(sc, darkMapMatrix);
int winNode[2] = {6, 6};
Entity player(sc, lc, cmDark, winNode[0], winNode[1], 0, 0, 255);

void resetGame(){
  sc.setAll(255, 0, 0);
  lc.print("WELCOME TO", "THE GAME");
  delay(100);
  sc.clear();
  player.reset();
  player.draw();
  if(isLight){
  	cmLight.draw();
  }
  else{
    cmDark.draw();
  }
  sc.setPixel(winNode[0], winNode[1], 0, 255, 0);
}

void setup() {
  pinMode(A0, INPUT);
  lc.begin();
  sc.begin();
  
  resetGame();
}

void loop(){
  char key = keypad.getKey();
  int lightValue = analogRead(A0);
  if(lightValue >= 512 && !isLight){
    cmDark.hide();
    cmLight.draw();
    sc.setPixel(winNode[0], winNode[1], 0, 255, 0);
    player.changeCollisionMap(&cmLight);
    isLight = true;
  }
  else if(lightValue < 512 && isLight){
    cmLight.hide();
    cmDark.draw();
    sc.setPixel(winNode[0], winNode[1], 0, 255, 0);
    player.changeCollisionMap(&cmDark);
    isLight = false;
  }
    
  if (key){
    if(key == '2'){
      player.hide();
      player.moveX(-1);
      player.draw();
    }
    else if(key == '8'){
      player.hide();
      player.moveX(1);
      player.draw();
    }
    else if(key == '4'){
      player.hide();
      player.moveY(-1);
      player.draw();
    }
    else if(key == '6'){
      player.hide();
      player.moveY(1);
      player.draw();
    }
    else if(key == '*'){
      player.printHealth(500);
    }
  }
}
