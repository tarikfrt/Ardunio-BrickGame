#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <SevSeg.h>

//SevSeg sevseg; // SevSeg nesnesi oluştur
 // SevSeg nesnesi oluştur
#define SEG_A 2
#define SEG_B 3
#define SEG_C 4
#define SEG_D 5
#define SEG_E 6
#define SEG_F 7
#define SEG_G 8

const uint8_t digits[]={

0b01000000,
0b11111001,
0b10100100,
0b00110000,
0b00011001,
0b00010010,
0b00000010,
0b01111000,
0b00000000,
0b00010000,
};
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin bağlantıları
//byte numDigits = 1;
//byte digitPins[] = {29}; // Dig pinleri
//byte segmentPins[] = {37, 39, 41, 43, 45, 47, 49}; // Seg pinleri

unsigned long prevMillis = 0; // Önceki zamanı saklamak için
unsigned int numberToShow = 1; // Gösterilecek sayı

// Topun konumu ve hızı
float ballX = (SCREEN_WIDTH / 2);
float ballY = 35;
float ballSpeedX = 1.6;
float ballSpeedY = 1.6;

// Başlangıç noktası
float initialBallX = ballX;
float initialBallY = ballY;

// Çubuğun konumu ve genişliği
float paddleX = SCREEN_WIDTH / 2;
const float paddleWidth = 30;
const float paddleHeight = 4;

// Tuğla bilgileri
const int numBricksX = 10;
const int numBricksY = 3;
const int brickWidth = SCREEN_WIDTH / numBricksX;
const int brickHeight = 8;
bool bricks[numBricksX][numBricksY];

const int slidePotPin = A2; // Slayt potansiyometrenin sinyal pini A0'a bağlı
const int potPin = A1;
int healty = 3;
int ledPinA = 31;
int ledPinB = 33;
int ledPinC = 35;

const int buttonPin = 11; // Örnek olarak 2. dijital pini kullanıyoruz
int buttonState = 0;


// Seviye sayacı
int level = 1;
int score = 0; // Score değişkeni tanımlama
int skorLedicin=0;

void setup() {
  // initialize with the number of digits, and the pins for the digits and segments
  //sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  //sevseg.setBrightness(90); // Parlaklığı ayarla (0 ile 100 arasında)

  

  
  Serial.begin(9600);
  
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
  displaySegment(digits[0]);

  pinMode(ledPinA, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  pinMode(ledPinC, OUTPUT);

  pinMode(buttonPin, INPUT);

  digitalWrite(ledPinA, HIGH);
  digitalWrite(ledPinB, HIGH);
  digitalWrite(ledPinC, HIGH);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 initialization failed. Please check your connections."));
    for (;;);
  }

  display.display(); // Ekranı temizle
  delay(2000);  // 2 saniye bekle
  display.clearDisplay(); // Ekranı temizle

  // Tuğla dizisini başlat
  for (int i = 0; i < numBricksX; i++) {
    for (int j = 0; j < numBricksY; j++) {
      bricks[i][j] = true;
    }
  }
}


void loop() {


  if(buttonState == 1){
   display.invertDisplay(true);
  }
  if(buttonState == 0)
  {
    display.invertDisplay(false);
  }
  

  buttonState = digitalRead(buttonPin);

  // Durum seri monitöre yazdırılır
  Serial.println(buttonState);
  Serial.println(score);
  
  int slidePotValue = analogRead(slidePotPin);
  // Dairesel potansiyometrenin değerini oku
  int potValue = analogRead(potPin);
  // Ekranı temizle
  display.clearDisplay();

  // Topu hareket ettir
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Duvar çarpışmalarını kontrol et
  if (ballX <= 0 || ballX >= SCREEN_WIDTH - 1) {
    ballSpeedX *= -1;
  }
  if (ballY <= 0) {
    ballSpeedY *= -1;
  }

  // Çubuk çarpışmasını kontrol et
  if (ballY >= SCREEN_HEIGHT - paddleHeight && ballX >= paddleX && ballX <= paddleX + paddleWidth) {
    ballSpeedY *= -1; // Topun yönünü değiştir (aşağı doğru gitmesi için)
  }

  // Top çubuğun hizasından aşağı düştü mü kontrol et
  if (ballY >= SCREEN_HEIGHT) 
  {
    if(healty == 3)
    {
      healty --;
      digitalWrite(ledPinA, LOW); // LED'i söndür
    }
    else if(healty == 2)
    {
      healty --;
      digitalWrite(ledPinB, LOW); // LED'i söndür
    }
    else
    {
      digitalWrite(ledPinC, LOW); // LED'i söndür
      // Oyun bitti
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10, 20);
      display.println("Oyun Bitti!");
      display.display();
      delay(30000); // 3 saniye bekle
      // Oyunu başlat
      startGame();
    }
    
    // Topu başlangıç noktasına geri döndür
    ballX = initialBallX;
    ballY = initialBallY;
  }

  // Tuğlalarla çarpışmayı kontrol et
  int ballXIndex = int(ballX / brickWidth);
  int ballYIndex = int(ballY / brickHeight);
  if (ballYIndex < numBricksY && ballXIndex < numBricksX && bricks[ballXIndex][ballYIndex]) {
    bricks[ballXIndex][ballYIndex] = false;
    score++;

    skorLedicin++;
    displaySegment(digits[skorLedicin]);
    if(skorLedicin==9){
      skorLedicin=-1;
    }

    //sevseg.setNumber(score); // 42 sayısını göster
    // sevseg.refreshDisplay(); // Gösterimi güncelle
    ballSpeedY *= -1;
  }

  // Tuğlaları çiz
  bool allBricksDestroyed = true; // Tüm tuğlaların yok edilip edilmediğini kontrol etmek için bir bayrak
  for (int i = 0; i < numBricksX; i++) {
    for (int j = 0; j < numBricksY; j++) {
      if (bricks[i][j]) {
          display.fillRect(i * brickWidth, j * brickHeight, brickWidth, brickHeight, WHITE);
        
        allBricksDestroyed = false; // En az bir tuğla varsa bayrağı false yap
      }
    }
  }

  // Tüm tuğlalar yoksa topun hızını %20 artır ve tuğlaları yeniden başlat
  if (allBricksDestroyed) {
    ballSpeedX *= 1.2; // Topun x hızını artır (%20)
    ballSpeedY *= 1.2; // Topun y hızını artır (%20)
    ballX = SCREEN_WIDTH / 2; // Topun başlangıç konumunu ayarla
  ballY = 35;
    // Tuğlaları yeniden başlat
    for (int i = 0; i < numBricksX; i++) {
      for (int j = 0; j < numBricksY; j++) {
        bricks[i][j] = true;
      }
    }
    // Yeni seviye mesajı göster
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print("Seviye ");
    display.println(level+1);
    display.display();
    delay(3000); // 3 saniye bekle
    level++; // Seviye artır
  }

  // Çubuğu çiz
  paddleX = map(slidePotValue, 0, 1023, 0, SCREEN_WIDTH - paddleWidth); // Çubuğun x konumunu slayt potansiyometresinin değerine göre belirle
      display.fillRect(paddleX, SCREEN_HEIGHT - paddleHeight, paddleWidth, paddleHeight, WHITE);

  // Topu çiz
      display.fillCircle(ballX, ballY, 3, WHITE);

  // Ekranı güncelle
  display.display(); // Ekran güncelleme
  delay(10); // Gecikme ekle
}

void startGame() {
  // Yeni oyun başlatma işlemleri
  healty = 3; // Canları yeniden ayarla
  digitalWrite(ledPinA, HIGH); // Ledleri resetle
  digitalWrite(ledPinB, HIGH);
  digitalWrite(ledPinC, HIGH);
  
  ballSpeedX = ballSpeedX*1.2; // Topun hızını başlangıç hızına ayarla
  ballSpeedY = ballSpeedY*1.2;
  level = 1; // Seviyeyi sıfırla
  displaySegment(digits[0]);
}

void displaySegment(uint8_t digit){
for(int i =0;i<7;i++){
  if(digit & (1<<i)){
    digitalWrite(SEG_A +i,HIGH);


  }else{
    digitalWrite(SEG_A+i,LOW);
  }
}
}
