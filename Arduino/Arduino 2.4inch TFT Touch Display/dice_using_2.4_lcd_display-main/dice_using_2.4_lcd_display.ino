#if 1

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000
#include <FreeDefaultFonts.h>
#include <Fonts/FreeSans12pt7b.h>
int randNumber;
int activate =0;
// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 6, XM = A2, YP = A1, YM = 7; //ID=0x9341
const int TS_LEFT = 173, TS_RT = 885, TS_TOP = 924, TS_BOT = 175;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button play_btn;

int pixel_x, pixel_y; //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
TSPoint p = ts.getPoint();
pinMode(YP, OUTPUT); //restore shared pins
pinMode(XM, OUTPUT);
digitalWrite(YP, HIGH); //because TFT control pins
digitalWrite(XM, HIGH);
bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
if (pressed) {
pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
}
return pressed;
}

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

void setup(void)
{
Serial.begin(9600);
uint16_t ID = tft.readID();
Serial.print("TFT ID = 0x");
Serial.println(ID, HEX);
Serial.println("Calibrate for your Touch Panel");
if (ID == 0xD3D3) ID = 0x9486; // write-only shield
tft.begin(ID);
tft.setRotation(0); //PORTRAIT
tft.fillScreen(BLACK);
play_btn.initButton(&tft, 120, 290, 240, 50, WHITE, CYAN, BLACK, "PLAY!!!", 3);
play_btn.drawButton(false);

showmsgXY(20, 20, 2, NULL, "RoboticaDIY.com");
showmsgXY(20, 60, 2, NULL, "Game Rule: ");
showmsgXY(20, 90, 2, NULL, "1)Hit the play");
showmsgXY(20, 110, 2, NULL, " button.");
showmsgXY(20, 140, 2, NULL, "2)Wait until it");
showmsgXY(20, 160, 2, NULL, " picks the random");
showmsgXY(20, 180, 2, NULL, " number.");
showmsgXY(20, 210, 2, NULL, "3)Hit Play Again.");

randomSeed(analogRead(5));
}


void loop(void)
{
bool down = Touch_getXY();
play_btn.press(down && play_btn.contains(pixel_x, pixel_y));

if (play_btn.justReleased())
play_btn.drawButton();

if (play_btn.justPressed()) {
//play_btn.drawButton(true);
randNumber = random(1, 7);
activate = 1;
tft.fillScreen(YELLOW);
tft.fillRect(30, 50, 70, 70, RED);
tft.fillRect(140, 50, 70, 70, RED);
tft.fillRect(30, 170, 70, 70, RED);
tft.fillRect(140, 170, 70, 70, RED);
delay(200);
tft.fillScreen(MAGENTA);
tft.fillRect(85, 120, 70, 70, RED);
delay(200);
tft.fillScreen(BLUE);
tft.fillRect(30, 10, 70, 70, RED);
tft.fillRect(130, 10, 70, 70, RED);
tft.fillRect(30, 90, 70, 70, RED);
tft.fillRect(130, 90, 70, 70, RED);
tft.fillRect(30, 170, 70, 70, RED);
tft.fillRect(130, 170, 70, 70, RED);
delay(200);
tft.fillScreen(WHITE);
tft.fillRect(140, 30, 70, 70, RED);
tft.fillRect(30, 140, 70, 70, RED);
delay(200);
tft.fillScreen(CYAN);
tft.fillRect(10, 10, 70, 70, RED);
tft.fillRect(160, 10, 70, 70, RED);
tft.fillRect(85, 85, 70, 70, RED);
tft.fillRect(10, 160, 70, 70, RED);
tft.fillRect(160, 160, 70, 70, RED);
delay(200);
tft.fillScreen(GREEN);
tft.fillRect(10, 170, 70, 70, RED);
tft.fillRect(85, 90, 70, 70, RED);
tft.fillRect(160, 10, 70, 70, RED);
delay(200);

}


Serial.println(randNumber);
if (activate == 1){

switch (randNumber) {
case 1: 
tft.fillScreen(BLACK);
tft.fillRect(85, 120, 70, 70, RED);
activate = 0;
break;
case 2: 
tft.fillScreen(BLACK);
tft.fillRect(140, 30, 70, 70, RED);
tft.fillRect(30, 140, 70, 70, RED);
activate = 0;
break;
case 3: 
tft.fillScreen(BLACK);
tft.fillRect(10, 170, 70, 70, RED);
tft.fillRect(85, 90, 70, 70, RED);
tft.fillRect(160, 10, 70, 70, RED);
activate = 0;
break;
case 4: 
tft.fillScreen(BLACK);
tft.fillRect(30, 50, 70, 70, RED);
tft.fillRect(140, 50, 70, 70, RED);
tft.fillRect(30, 170, 70, 70, RED);
tft.fillRect(140, 170, 70, 70, RED);
activate = 0;
break;
case 5: 
tft.fillScreen(BLACK);
tft.fillRect(10, 10, 70, 70, RED);
tft.fillRect(160, 10, 70, 70, RED);
tft.fillRect(85, 85, 70, 70, RED);
tft.fillRect(10, 160, 70, 70, RED);
tft.fillRect(160, 160, 70, 70, RED);
activate = 0;
break;
case 6: 
tft.fillScreen(BLACK);
tft.fillRect(30, 10, 70, 70, RED);
tft.fillRect(130, 10, 70, 70, RED);
tft.fillRect(30, 90, 70, 70, RED);
tft.fillRect(130, 90, 70, 70, RED);
tft.fillRect(30, 170, 70, 70, RED);
tft.fillRect(130, 170, 70, 70, RED);
activate = 0;
break;

}
}


}
#endif

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg)
{
int16_t x1, y1;
uint16_t wid, ht;
//tft.drawFastHLine(0, y, tft.width(), WHITE);
tft.setFont(f);
tft.setCursor(x, y);
tft.setTextColor(GREEN);
tft.setTextSize(sz);
tft.print(msg);
delay(400);
}
