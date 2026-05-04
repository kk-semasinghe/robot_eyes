#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// I2C addresses for the displays
#define DISPLAY_I2C_ADDRESS 0x3C  // Both displays on same I2C bus (check with 'I2C_Scanner' if unsure)

// Create display objects
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Eye parameters
int eyeCenterX = SCREEN_WIDTH / 2;
int eyeCenterY = SCREEN_HEIGHT / 2;
int pupilRadius = 5;
int eyeRadius = 20;
int eyeOffsetY = 10; // Eyes position higher on screen

// Animation states
enum AnimationState {
  IDLE,
  BLINK,
  LOOK_LEFT,
  LOOK_RIGHT,
  LOOK_UP,
  LOOK_DOWN,
  LOOK_DIAGONAL
};

int currentAnimation = IDLE;
unsigned long lastAnimationChange = 0;
unsigned long animationDuration = 3000; // 3 seconds per animation

// Blink variables
int blinkState = 0;
unsigned long lastBlinkTime = 0;
unsigned long blinkInterval = 100;

void setup() {
  Serial.begin(9600);
  
  // Initialize I2C
  Wire.begin();
  delay(100);
  
  // Initialize both displays
  if (!display1.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed for display1"));
    while (1);
  }
  
  if (!display2.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed for display2"));
    while (1);
  }
  
  display1.clearDisplay();
  display2.clearDisplay();
  display1.display();
  display2.display();
  
  Serial.println(F("Eyes animation initialized!"));
}

void loop() {
  // Change animation every few seconds
  if (millis() - lastAnimationChange > animationDuration) {
    currentAnimation = random(0, 6);
    lastAnimationChange = millis();
    blinkState = 0;
  }
  
  // Draw the appropriate animation
  switch (currentAnimation) {
    case IDLE:
      drawIdleEyes();
      break;
    case BLINK:
      drawBlinkingEyes();
      break;
    case LOOK_LEFT:
      drawLookingEyes(-8, 0);
      break;
    case LOOK_RIGHT:
      drawLookingEyes(8, 0);
      break;
    case LOOK_UP:
      drawLookingEyes(0, -8);
      break;
    case LOOK_DOWN:
      drawLookingEyes(0, 8);
      break;
    case LOOK_DIAGONAL:
      drawLookingEyes(6, -6);
      break;
  }
  
  delay(30); // ~33 FPS refresh rate
}

// Draw both eyes idle - looking straight ahead
void drawIdleEyes() {
  drawEyePair(0, 0);
}

// Draw eyes with pupils looking in a specific direction
void drawLookingEyes(int offsetX, int offsetY) {
  drawEyePair(offsetX, offsetY);
}

// Draw blinking animation
void drawBlinkingEyes() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastBlinkTime > blinkInterval) {
    blinkState++;
    lastBlinkTime = currentTime;
    
    if (blinkState > 6) {
      blinkState = 0;
      currentAnimation = random(0, 6); // Switch to different animation
      animationDuration = random(2000, 5000);
    }
  }
  
  if (blinkState < 3) {
    // Opening/closing blink
    int openingAmount = (3 - blinkState) * 2; // 6, 4, 2, then close
    drawEyeBlinking(openingAmount);
  } else {
    // Closed
    drawEyeBlinking(0);
  }
}

// Draw eye with blink effect (vertical scaling)
void drawEyeBlinking(int openingAmount) {
  display1.clearDisplay();
  display2.clearDisplay();
  
  // Draw left eye (on display 1)
  drawEyeOpen(display1, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, 
              openingAmount == 0 ? 1 : openingAmount);
  
  // Draw right eye (on display 2)
  drawEyeOpen(display2, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, 
              openingAmount == 0 ? 1 : openingAmount);
  
  display1.display();
  display2.display();
}

// Draw a pair of full eyes (both displays)
void drawEyePair(int pupilOffsetX, int pupilOffsetY) {
  display1.clearDisplay();
  display2.clearDisplay();
  
  // Draw left eye on display 1
  drawSingleEye(display1, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, 
                pupilOffsetX, pupilOffsetY);
  
  // Draw right eye on display 2
  drawSingleEye(display2, eyeCenterX, eyeCenterY + eyeOffsetY, eyeRadius, 
                pupilOffsetX, pupilOffsetY);
  
  display1.display();
  display2.display();
}

// Draw a single eye on a specific display
void drawSingleEye(Adafruit_SSD1306 &display, int centerX, int centerY, int radius,
                   int pupilOffsetX, int pupilOffsetY) {
  // Draw eye white (sclera) - circle
  display.drawCircle(centerX, centerY, radius, SSD1306_WHITE);
  display.fillCircle(centerX, centerY, radius - 1, SSD1306_WHITE);
  
  // Draw iris (slightly darker - using a slightly smaller filled circle for iris area)
  display.fillCircle(centerX, centerY, radius - 4, SSD1306_BLACK);
  display.drawCircle(centerX, centerY, radius - 4, SSD1306_WHITE);
  
  // Draw pupil (black circle that moves)
  int pupilX = constrain(centerX + pupilOffsetX, centerX - radius + 5, centerX + radius - 5);
  int pupilY = constrain(centerY + pupilOffsetY, centerY - radius + 5, centerY + radius - 5);
  
  display.fillCircle(pupilX, pupilY, pupilRadius, SSD1306_BLACK);
  
  // Draw pupil highlight (shine)
  display.fillCircle(pupilX - 2, pupilY - 2, 2, SSD1306_WHITE);
}

// Draw eye with vertical opening effect
void drawEyeOpen(Adafruit_SSD1306 &display, int centerX, int centerY, int radius, int openingAmount) {
  if (openingAmount <= 0) {
    // Fully closed eye - draw simple line
    display.drawLine(centerX - radius, centerY, centerX + radius, centerY, SSD1306_WHITE);
    return;
  }
  
  // Draw eye white with vertical scaling
  int scaledHeight = (openingAmount * radius) / 20;
  
  // Draw ellipse-like eye (simplified with multiple circles/lines)
  for (int i = -radius; i <= radius; i++) {
    int yOffset = (scaledHeight * sqrt(1.0 - (i * i) / (radius * radius))) / radius;
    display.drawLine(centerX + i, centerY - yOffset, centerX + i, centerY + yOffset, SSD1306_WHITE);
  }
  
  // Draw pupil at center if eye is open enough
  if (openingAmount > 5) {
    display.fillCircle(centerX, centerY, pupilRadius - 1, SSD1306_BLACK);
    display.fillCircle(centerX - 1, centerY - 1, 1, SSD1306_WHITE); // Shine
  }
}
