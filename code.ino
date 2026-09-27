// C++ code
//
#include <Adafruit_LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>

#define PIN 5
#define NUMPIXELS 36

void resetGame();

class ButtonController{
  public:
  ButtonController(){}
  
  void begin(){
    pinMode(13, INPUT);
    pinMode(12, INPUT);
    pinMode(11, INPUT);
    pinMode(10, INPUT);
    pinMode(9, INPUT);
  }
  
  char getKey(){
    if(digitalRead(13) == HIGH){
    	return 'U';
    }
    else if(digitalRead(12) == HIGH){
    	return 'D';
    }
    else if(digitalRead(11) == HIGH){
    	return 'L';
    }
    else if(digitalRead(10) == HIGH){
    	return 'R';
    }
    else if(digitalRead(9) == HIGH){
    	return 'H';
    }
    return ' ';
  }
};

class SoundController{
  public:
  SoundController(){}
  
  void begin(){
    pinMode(3, OUTPUT);
  }
  
  void play(int frequency, int time){
    tone(3, frequency, time);
  }
  
  void playMelody(int *frequencies, int length, int time){
    for(int i = 0; i < length; ++i){
      tone(3, frequencies[i], time);
      delay(time);
    }
  }
};

class StripController{
  private:
  Adafruit_NeoPixel strip;
  byte pixelMatrix[6][6] = {
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
  }
  
  void show(){
    strip.show();
  }
  
  void rainbow(int cycles){
    byte colors[18][3] = {
      {255,  85,   0},
      {255, 170,   0},
      {255, 255,   0},

      {170, 255,   0},
      { 85, 255,   0},
      {  0, 255,   0},
        
      {  0, 255,  85},
      {  0, 255, 170},
      {  0, 255, 255},
        
      {  0, 170, 255},
      {  0,  85, 255},
      {  0,   0, 255},
        
      { 85,   0, 255},
      {170,   0, 255},
      {255,   0, 255},
        
      {255,   0, 170},
      {255,   0,  85},
      {255,   0,   0}
    };
    for (int shift = 0; shift < cycles * 18; shift++) {
      for (int col = 0; col < 6; ++col) {
        int color = (col + shift) % 18;
        for (int row = 0; row < 6; ++row) {
          strip.setPixelColor(
                      pixelMatrix[row][col],
                      strip.Color(
                        colors[color][0],
                        colors[color][1],
                        colors[color][2]));
        }
      }
      strip.show();
      delay(50);
    } 
  }
  
  void setAll(int r, int g, int b){
    uint32_t color = strip.Color(r, g, b);
    for (int i = 0; i < NUMPIXELS; i++){
      strip.setPixelColor(i, color);
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
  char line1[17] = "";
  char line2[17] = "";
  Adafruit_LiquidCrystal lcd;
  
  public:
  LcdController() : lcd(0){}
  
  void begin(){
    lcd.begin(16, 2);
  }
  
  void print(char* line1, char* line2){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    strcpy(this->line1, line1);
    strcpy(this->line2, line2);
  }
  
  void briefPrint(char* line1, char* line2, int time){
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
  byte collisionMatrix[6][6] = {{0}};
  StripController &sc;
  
  public:
  CollisionMap(StripController &sc, byte collisionMatrix[6][6]) : sc(sc){
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
    sc.show();
  }
  
  void hide(){
    for (int row = 0; row < 6; ++row){
      for (int col = 0; col < 6; ++col){
        if(collisionMatrix[row][col] == 1){
          sc.setPixel(row + 1, col + 1, 0, 0, 0);
        }
      }
    }
    sc.show();
  }
  
  bool hasCollision(int x, int y){
    if(collisionMatrix[x - 1][y - 1] == 1){
      return true;
    }
    return false;
  }
};

class EnemyMap{
  private:
  byte enemyMatrix[6][6] = {{0}};
  StripController &sc;
  
  public:
  EnemyMap(StripController &sc, byte enemyMatrix[6][6]) : sc(sc){
    for (int row = 0; row < 6; ++row){
      for (int col = 0; col < 6; ++col){
        this->enemyMatrix[row][col] = enemyMatrix[row][col];
      }
    }
  }
  
  void draw(){
    for (int row = 0; row < 6; ++row){
      for (int col = 0; col < 6; ++col){
        if(enemyMatrix[row][col] == 1){
          sc.setPixel(row + 1, col + 1, 255, 170, 0);
        }
      }
    }
    sc.show();
  }
  
  void hide(){
    for (int row = 0; row < 6; ++row){
      for (int col = 0; col < 6; ++col){
        if(enemyMatrix[row][col] == 1){
          sc.setPixel(row + 1, col + 1, 0, 0, 0);
        }
      }
    }
    sc.show();
  }
  
  bool hasEnemy(int x, int y){
    if(enemyMatrix[x - 1][y - 1] == 1){
      return true;
    }
    return false;
  }
  
  void removeEnemy(int x, int y){
    enemyMatrix[x - 1][y - 1] = 0;
  }
  
  void reset(byte enemyMatrix[6][6]){
    for (int row = 0; row < 6; ++row){
      for (int col = 0; col < 6; ++col){
        this->enemyMatrix[row][col] = enemyMatrix[row][col];
      }
    }
  }
};

class Player{
  private:
  int x = 1;
  int y = 1;
  int health = 100;
  StripController &sc;
  LcdController &lc;
  SoundController &sdc;
  CollisionMap *cm;
  EnemyMap *em;
  int winX;
  int winY;
  
  int r, g, b;
  
  public:
  Player(StripController &sc, LcdController &lc, 
         SoundController &sdc ,CollisionMap &cm, EnemyMap &em, 
         int winX, int winY, int r, int g, int b) : sc(sc), lc(lc), sdc(sdc), 
  		 cm(&cm), em(&em), winX(winX), winY(winY), r(r), g(g), b(b){}
  
  void reset(){
    x = 1;
    y = 1;
    health = 100;
  }
  
  void draw(){
    sc.setPixel(x, y, r, g, b);
    sc.show();
  }
  
  void hide(){
    sc.setPixel(x, y, 0, 0, 0);
    sc.show();
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
        lc.print("YOU WON", "THE GAME");
        int melody[3] = {330, 349, 392};
      	sdc.playMelody(melody, 3, 200);
        sc.rainbow(1);
        resetGame();
      }
      if(em->hasEnemy(x, y)){
        sdc.play(500, 100);
        lc.briefPrint("ENEMY DID", "20 DAMAGE", 500);
        health -= 20;
        em->removeEnemy(x, y);
        dieIfCan();
      }
    }
    else{
      sdc.play(300, 50);
    }
  }
  
  void moveY(int amount){
    if(y + amount <= 6 && y + amount >= 1 && !cm->hasCollision(x, y + amount)){
      y += amount;
      if(x == winX && y == winY){
        lc.print("YOU WON", "THE GAME");
        int melody[3] = {330, 349, 392};
      	sdc.playMelody(melody, 3, 300);
        sc.rainbow(1);
        resetGame();
      }
      if(em->hasEnemy(x, y)){
        sdc.play(500, 100);
        lc.briefPrint("ENEMY DID", "20 DAMAGE", 500);
        health -= 20;
        em->removeEnemy(x, y);
        dieIfCan();
      }
    }
    else{
      sdc.play(300, 50);
    }
  }
  
  void changeCollisionMap(void *newMap){
    cm = static_cast<CollisionMap*>(newMap);
    if(cm->hasCollision(x, y) == true){
      health = 0;
      dieIfCan();
    }
  }
  
  void dieIfCan(){
    if(health == 0){
      lc.print("YOU LOST", "THE GAME");
      int melody[3] = {330, 294, 262};
      sdc.playMelody(melody, 3, 300);
      resetGame();
    }
  }
  
  void printHealth(int time){
    char healthString[16];
    lc.briefPrint("Player health:", itoa(health, healthString, 10), time);
  }
};

ButtonController bc;
SoundController sdc;
StripController sc;
LcdController lc;
byte lightMapMatrix[6][6] = {
  {0, 1, 0, 0, 1, 0},
  {0, 1, 0, 1, 0, 0},
  {0, 1, 0, 1, 0, 0},
  {1, 0, 0, 1, 0, 1},
  {0, 0, 0, 1, 0, 0},
  {1, 1, 0, 1, 0, 0},
};
byte darkMapMatrix[6][6] = {
  {0, 1, 0, 1, 0, 0},
  {1, 1, 0, 0, 0, 1},
  {0, 1, 0, 1, 0, 0},
  {0, 0, 0, 0, 1, 0},
  {0, 1, 1, 1, 1, 0},
  {0, 0, 0, 1, 0, 0},
};
byte enemyMapMatrix[6][6] = {
  {0, 0, 1, 0, 0, 1},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 1, 1},
  {0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1},
  {0, 0, 1, 0, 1, 0},
};
bool isLight = false;
CollisionMap cmLight(sc, lightMapMatrix);
CollisionMap cmDark(sc, darkMapMatrix);
EnemyMap em(sc, enemyMapMatrix);
int winNode[2] = {6, 6};
Player player(sc, lc, sdc, cmDark, em, winNode[0], winNode[1], 0, 0, 255);

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
  em.reset(enemyMapMatrix);
  em.draw();
  sc.setPixel(winNode[0], winNode[1], 0, 255, 0);
  sc.show();
  lc.print("BUTTONS TO MOVE", "LIGHT CHANGE MAP");
}

void setup() {
  pinMode(A0, INPUT);
  lc.begin();
  sc.begin();
  bc.begin();
  sdc.begin();
  
  resetGame();
}

void loop(){
  char key = bc.getKey();
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
    if(key == 'U'){
      player.hide();
      player.moveX(-1);
      player.draw();
    }
    else if(key == 'D'){
      player.hide();
      player.moveX(1);
      player.draw();
    }
    else if(key == 'L'){
      player.hide();
      player.moveY(-1);
      player.draw();
    }
    else if(key == 'R'){
      player.hide();
      player.moveY(1);
      player.draw();
    }
    else if(key == 'H'){
      player.printHealth(500);
    }
  }
}
