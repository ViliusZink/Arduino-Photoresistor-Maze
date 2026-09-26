// C++ code
//
#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>

#define PIN 5
#define NUMPIXELS 36

Adafruit_LiquidCrystal lcd(0);

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
    strip.clear();
    
    for (int shift = 0; shift < cycles * 65; ++shift){
      for (int row = 0; row < 6; ++row){
        for (int col = 0; col < 6; ++col){
          int hue = (shift * 1000L + col * 10000L) % 65536;
          strip.setPixelColor(pixelMatrix[row][col], strip.ColorHSV(hue));
        }
      }
      strip.show();
      delay(5);
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
  StripController &sc;
  CollisionMap *cm;
  int r, g, b;
  
  public:
  Entity(StripController &sc, CollisionMap &cm, int r, int g, int b) : sc(sc), 
  												cm(&cm), r(r), g(g), b(b){}
  
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
    if(this->x + amount <= 6 && this->x + amount >= 1 && !cm->hasCollision(x + amount, y)){
      this->x += amount;
    }
  }
  
  void moveY(int amount){
    if(this->y + amount <= 6 && this->y + amount >= 1 && !cm->hasCollision(x, y + amount)){
      this->y += amount;
    }
  }
  
  void changeCollisionMap(void *newMap){
    this->cm = static_cast<CollisionMap*>(newMap);
  }
};

StripController sc;
int lightMapMatrix[6][6] = {
  {0, 1, 0, 0, 1, 1},
  {0, 1, 0, 0, 0, 0},
  {0, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0},
  {1, 1, 0, 1, 0, 0},
};
CollisionMap cmLight(sc, lightMapMatrix);
Entity player(sc, cmLight, 0, 0, 255);

void setup() {
  pinMode(A0, INPUT);
  
  lcd.begin(16, 2);
  lcd.print("WELCOME");
  
  sc.begin();
  player.draw();
  cmLight.draw();
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
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
  }
}
