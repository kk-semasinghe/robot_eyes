/*
 * Advanced Eyes Animation with Expression Support
 * This version includes expressions: happy, sad, angry, surprised
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define DISPLAY_I2C_ADDRESS 0x3C

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Eye parameters
int eyeCenterX = SCREEN_WIDTH / 2;
int eyeCenterY = SCREEN_HEIGHT / 2;
int pupilRadius = 5;
int eyeRadius = 20;
int eyeOffsetY = 10;

// Animation states
enum AnimationState {
  IDLE,
  HAPPY,
  SAD,
  ANGRY,
  SURPRISED,
  BLINKING,
  LOOK_LEFT,
  LOOK_RIGHT
};

int currentAnimation = IDLE;
unsigned long lastAnimationChange = 0;
unsigned long animationDuration = 4000;
int blinkCounter = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(100);
  
  if (!display1.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS)) {
    Serial.println(F("Display 1 failed"));
    while (1);
  }
  
  if (!display2.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS)) {
    Serial.println(F("Display 2 failed"));
    while (1);
  }
  
  display1.clearDisplay();
  display2.clearDisplay();
  display1.display();
  display2.display();
  
  Serial.println(F("Advanced Eyes initialized!"));
}

void loop() {
  // Change animation every few seconds
  if (millis() - lastAnimationChange > animationDuration) {
    currentAnimation = random(0, 8);
    lastAnimationChange = millis();
    blinkCounter = 0;
  }
  
  switch (currentAnimation) {
    case IDLE:
      drawNormalEyes(0, 0);
      break;
    case HAPPY:
      drawHappyExpression();
      break;
    case SAD:
      drawSadExpression();
      break;
    case ANGRY:
      drawAngryExpression();
      break;
    case SURPRISED:
      drawSurprisedExpression();
      break;
    case BLINKING:
      drawBlinkingAnimation();
      break;
    case LOOK_LEFT:
      drawNormalEyes(-10, 0);
      break;
    case LOOK_RIGHT:
      drawNormalEyes(10, 0);
      break;
  }
  
  delay(40);
}

// Draw normal eyes looking in a direction
void drawNormalEyes(int offsetX, int offsetY) {
  display1.clearDisplay();
  display2.clearDisplay();
  
  drawEye(display1, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, offsetX, offsetY, false, false);
  drawEye(display2, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, offsetX, offsetY, false, false);
  
  display1.display();
  display2.display();
}

// Happy expression (curved eyes, upward pupils)
void drawHappyExpression() {
  display1.clearDisplay();
  display2.clearDisplay();
  
  // Draw happy eyes
  drawEye(display1, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, 0, -3, true, false);
  drawEye(display2, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, 0, -3, true, false);
  
  // Draw smile (arc at bottom)
  display1.drawLine(eyeCenterX - 15, eyeCenterY + eyeOffsetY + 15, eyeCenterX + 15, eyeCenterY + eyeOffsetY + 15, SSD1306_WHITE);
  display2.drawLine(eyeCenterX - 15, eyeCenterY + eyeOffsetY + 15, eyeCenterX + 15, eyeCenterY + eyeOffsetY + 15, SSD1306_WHITE);
  
  display1.display();
  display2.display();
}

// Sad expression (curved eyes downward, downward pupils)
void drawSadExpression() {
  display1.clearDisplay();
  display2.clearDisplay();
  
  // Draw sad eyes
  drawEye(display1, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, 0, 3, false, true);
  drawEye(display2, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, 0, 3, false, true);
  
  // Draw tears (small circles below eyes)
  display1.fillCircle(eyeCenterX - 10, eyeCenterY + eyeOffsetY + 25, 2, SSD1306_WHITE);
  display2.fillCircle(eyeCenterX - 10, eyeCenterY + eyeOffsetY + 25, 2, SSD1306_WHITE);
  
  display1.display();
  display2.display();
}

// Angry expression (downward slant, narrower eyes)
void drawAngryExpression() {
  display1.clearDisplay();
  display2.clearDisplay();
  
  int angryY = eyeCenterY + eyeOffsetY;
  
  // Draw angry eyes (tilted)
  display1.drawLine(eyeCenterX - eyeRadius, angryY - 5, eyeCenterX + eyeRadius, angryY + 2, SSD1306_WHITE);
  display1.drawLine(eyeCenterX - eyeRadius, angryY + 5, eyeCenterX + eyeRadius, angryY - 2, SSD1306_WHITE);
  display1.fillCircle(eyeCenterX - 5, angryY, pupilRadius, SSD1306_BLACK);
  
  display2.drawLine(eyeCenterX - eyeRadius, angryY - 5, eyeCenterX + eyeRadius, angryY + 2, SSD1306_WHITE);
  display2.drawLine(eyeCenterX - eyeRadius, angryY + 5, eyeCenterX + eyeRadius, angryY - 2, SSD1306_WHITE);
  display2.fillCircle(eyeCenterX - 5, angryY, pupilRadius, SSD1306_BLACK);
  
  // Draw angry eyebrows
  display1.drawLine(eyeCenterX - 18, angryY - 10, eyeCenterX - 5, angryY - 15, SSD1306_WHITE);
  display2.drawLine(eyeCenterX - 18, angryY - 10, eyeCenterX - 5, angryY - 15, SSD1306_WHITE);
  
  display1.display();
  display2.display();
}

// Surprised expression (large eyes, raised eyebrows)
void drawSurprisedExpression() {
  display1.clearDisplay();
  display2.clearDisplay();
  
  int largeRadius = eyeRadius + 5;
  int surprisedY = eyeCenterY + eyeOffsetY - 3;
  
  // Draw large surprised eyes
  display1.drawCircle(eyeCenterX, surprisedY, largeRadius, SSD1306_WHITE);
  display1.fillCircle(eyeCenterX, surprisedY, largeRadius - 1, SSD1306_WHITE);
  
  display2.drawCircle(eyeCenterX, surprisedY, largeRadius, SSD1306_WHITE);
  display2.fillCircle(eyeCenterX, surprisedY, largeRadius - 1, SSD1306_WHITE);
  
  // Draw pupils (small and centered)
  display1.fillCircle(eyeCenterX, surprisedY, 3, SSD1306_BLACK);
  display1.fillCircle(eyeCenterX - 1, surprisedY - 1, 1, SSD1306_WHITE);
  
  display2.fillCircle(eyeCenterX, surprisedY, 3, SSD1306_BLACK);
  display2.fillCircle(eyeCenterX - 1, surprisedY - 1, 1, SSD1306_WHITE);
  
  // Draw raised eyebrows
  display1.drawLine(eyeCenterX - 20, surprisedY - 25, eyeCenterX - 5, surprisedY - 28, SSD1306_WHITE);
  display2.drawLine(eyeCenterX - 20, surprisedY - 25, eyeCenterX - 5, surprisedY - 28, SSD1306_WHITE);
  
  // Draw O mouth (surprised)
  display1.drawCircle(eyeCenterX, eyeCenterY + 35, 5, SSD1306_WHITE);
  display2.drawCircle(eyeCenterX, eyeCenterY + 35, 5, SSD1306_WHITE);
  
  display1.display();
  display2.display();
}

// Blinking animation
void drawBlinkingAnimation() {
  static unsigned long lastBlinkTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastBlinkTime > 80) {
    blinkCounter++;
    lastBlinkTime = currentTime;
  }
  
  if (blinkCounter > 8) {
    currentAnimation = IDLE;
    blinkCounter = 0;
    return;
  }
  
  display1.clearDisplay();
  display2.clearDisplay();
  
  int openAmount = (blinkCounter < 3) ? (3 - blinkCounter) : (blinkCounter - 3);
  
  if (openAmount > 0) {
    drawEyePartial(display1, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, openAmount);
    drawEyePartial(display2, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, openAmount);
  }
  
  display1.display();
  display2.display();
}

// Draw a single eye with optional expressions
void drawEye(Adafruit_SSD1306 &display, int centerX, int centerY, int radius,
             int pupilOffsetX, int pupilOffsetY, bool happy, bool sad) {
  
  // Draw eye white
  display.drawCircle(centerX, centerY, radius, SSD1306_WHITE);
  display.fillCircle(centerX, centerY, radius - 1, SSD1306_WHITE);
  
  // Draw iris
  display.fillCircle(centerX, centerY, radius - 4, SSD1306_BLACK);
  display.drawCircle(centerX, centerY, radius - 4, SSD1306_WHITE);
  
  // Draw pupil
  int pupilX = constrain(centerX + pupilOffsetX, centerX - radius + 8, centerX + radius - 8);
  int pupilY = constrain(centerY + pupilOffsetY, centerY - radius + 8, centerY + radius - 8);
  
  display.fillCircle(pupilX, pupilY, pupilRadius, SSD1306_BLACK);
  display.fillCircle(pupilX - 2, pupilY - 2, 2, SSD1306_WHITE);
  
  // Draw expression lines
  if (happy) {
    // Happy eye curve
    display.drawLine(centerX - 15, centerY + 3, centerX + 15, centerY + 3, SSD1306_BLACK);
  }
  
  if (sad) {
    // Sad eye curve
    display.drawLine(centerX - 15, centerY - 3, centerX + 15, centerY - 3, SSD1306_BLACK);
  }
}

// Draw partial eye (for blinking)
void drawEyePartial(Adafruit_SSD1306 &display, int centerX, int centerY, int radius, int openAmount) {
  int height = (openAmount * radius) / 3;
  
  for (int i = -radius; i <= radius; i++) {
    if (abs(i) <= radius) {
      display.drawLine(centerX + i, centerY - height, centerX + i, centerY + height, SSD1306_WHITE);
    }
  }
  
  if (openAmount > 1) {
    display.fillCircle(centerX, centerY, 2, SSD1306_BLACK);
  }
}
