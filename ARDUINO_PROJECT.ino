#include "TFT9341Touch.h"

#define HIGH_TEMP_THRESHOLD 28.0  // ערך גבוה של טמפרטורה לצורך התראה
#define HIGH_WATER_THRESHOLD 400.0  // ערך גבוה של כמות מים לצורך השקייה

const int sensorPinTemp = A0; // פורט לחיישן טמפרטורה
const int sensorPinHumidity = A1; // פורט לחיישן לחות
const int sensorPinMoisture = 9; // פין דיגיטלי לחיישן לחות האדמה
const int pumpPin = 8; // פין לשליטה על משאבת המים

struct Plant {
    char name[20];    // שם הצמח
    float desiredTemperature;   // טמפרטורה רצויה
    float humidityLevel;        // מידת לחות
    float waterAmountNeeded;    // כמות מים נצרכת
};

// הגדרת מערך של צמחים בגודל 3
Plant plants[4];
Plant newPlant;

tft9341touch LcdTouch(10, 9, 7, 2); //cs, dc, tcs, tirq

char* selectedPlantName; // שם צמח הנבחר
int selectedPlantIndex = -1; // אינדקס הצמח הנבחר
int numPlants = 3; // מספר הצמחים במערכת
char plantNames[] = {"צמח חדש"}; // רשימת הצמחים הזמינים


// הצהרת הפונקציות
void screenMain();
void showPlantList();
void handleNewPlant();
void drawNumberKeypad();
float getUserInput(const char* prompt);

void setup() {
    Serial.begin(9600);
    LcdTouch.begin();
    LcdTouch.clearButton();
    LcdTouch.setRotation(0); //0,1,2,3
    LcdTouch.setTextSize(2);
    LcdTouch.setCursor(40, 40);
    LcdTouch.set(3780, 372, 489, 3811); //Calibration
    screenMain();
    pinMode(pumpPin, OUTPUT); // קביעת הפין של משאבת המים כפלט

    // הגדרת ערכים  עבור כל צמח ושמירתם במערכת
    strcpy(plants[0].name, "תינלכ");
    plants[0].desiredTemperature = 25.0;
    plants[0].humidityLevel = 60.0;
    plants[0].waterAmountNeeded = 300.0;

    strcpy(plants[1].name, "תפקר");
    plants[1].desiredTemperature = 20.0;
    plants[1].humidityLevel = 70.0;
    plants[1].waterAmountNeeded = 250.0;

    strcpy(plants[2].name, "היינמח");
    plants[2].desiredTemperature = 30.0;
    plants[2].humidityLevel = 50.0;
    plants[2].waterAmountNeeded = 350.0;

    strcpy(newPlant.name, "שדח חמצ");
    newPlant.desiredTemperature = 0;
    newPlant.humidityLevel = 0;
    newPlant.waterAmountNeeded = 0;
}

void loop() {
    float temperature = measureTemperature();
    float humidity = measureHumidity();
    float moisture = measureMoisture();

    displayMeasurements(temperature, humidity, moisture);

    selectedPlantIndex = handleTouch();

    Serial.print("Selected Plant Index: ");
    Serial.println(selectedPlantIndex);

    if (selectedPlantIndex == 3 && newPlant.waterAmountNeeded == 0) {
// האם בחר צמח חדש שטרם אותחל
        handleNewPlant();
    } else if (selectedPlantIndex != -1) {
        moisture = measureMoisture(); // עדכון ערך הלחות לפני בדיקת הצורך בהשקיה
        Serial.print("Current Moisture: ");
        Serial.println(moisture);
        Serial.print("Required Humidity Level: ");
        Serial.println(plants[selectedPlantIndex].humidityLevel);
        if (moisture > plants[selectedPlantIndex].humidityLevel) {
            Serial.println("Starting to water the plants...");
            waterPlants(selectedPlantIndex, moisture);
        } else {
            stopWatering();
            displayPlantInfoForDuration(selectedPlantIndex, 15000); //הצגת נתוני הצמח למשך 15 שניות
            screenMain(); // חזרה למסך הראשי
        }

        checkTemperature(temperature, selectedPlantIndex);
    }
}

float measureTemperature() {
    int sensorValue = analogRead(sensorPinTemp);
    float voltage = sensorValue * (5.0 / 1023.0);
    float temperature = voltage * 100.0;
    return temperature;
}

float measureHumidity() {
    int sensorValue = analogRead(sensorPinHumidity);
    float voltage = sensorValue * (5.0 / 1023.0);
    float humidity = voltage * 100.0;
    return humidity;
}

float measureMoisture() {
    int sensorValue = analogRead(sensorPinMoisture);
    float voltage = sensorValue * (5.0 / 1023.0);
    float moisture = voltage * 100.0;
    return moisture;
}

void checkTemperature(float currentTemperature, int plantIndex) {
    if (plantIndex != -1 && currentTemperature > plants[plantIndex].desiredTemperature) {
        LcdTouch.fillScreen(RED); // מסך אדום להתרעה
        LcdTouch.setTextSize(2);
        LcdTouch.setTextColor(WHITE);
        LcdTouch.setCursor(50, 120);
        LcdTouch.printheb("הטמפרטורה גבוהה מדי לצמח זה!");
        LcdTouch.setCursor(50, 160);
        LcdTouch.printheb("אנא נסו לקרר את הסביבה.");
        delay(3000); // המתנה של 3 שניות כדי לאפשר למשתמש לראות את ההודעה
        displayPlantInfo(plantIndex); // הצגת מידע הצמח שוב לאחר ההתרעה
    }
}

void displayPlantInfoForDuration(int plantIndex, int duration) {
    displayPlantInfo(plantIndex); // הצגת נתוני הצמח
    delay(duration); // המתנה למשך הזמן הנתון
}

void waterPlants(int plantIndex, float currentMoisture) {

    //זה הקוד האמיתי שאמור להשקות את המשאבה כל עוד הלחות באדמה גדולהמלחות הצמח בפועל לא ממש עובד.. אז עשיתי עם DELAY
    while (currentMoisture > plants[plantIndex].humidityLevel) {
        digitalWrite(pumpPin, HIGH); // הפעלת משאבת המים
        currentMoisture = measureMoisture(); // מדידה חוזרת של רמתהלחות באדמה
        Serial.print("Current Moisture: ");
        Serial.println(currentMoisture);
        Serial.print("Required Humidity Level: ");
        Serial.println(plants[plantIndex].humidityLevel);
    }
    digitalWrite(pumpPin, LOW); // כיבוי משאבת המים

// זה מה שעשיתי בפועל
    digitalWrite(pumpPin, HIGH); // הפעלת משאבת המים
    Serial.println("Watering started...");
    delay(5000);
    digitalWrite(pumpPin, LOW); // כיבוי משאבת המים
    Serial.println("Watering stopped...");

    // הצגת נתוני הצמח למשך 15 שניות ולאחר מכן חזרה למסך הראשי
    displayPlantInfoForDuration(plantIndex, 15000); // הצגת נתוני הצמחלמשך 15 שניות
    screenMain(); // חזרה למסך הראשי
}

void stopWatering() {
  digitalWrite(pumpPin, LOW); // כיבוי משאבת המים
}

void displayMeasurements(float temperature, float humidity, float moisture) {
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Moisture: ");
  Serial.print(moisture);
  Serial.println(" %");

  delay(1000);
}

int handleTouch() {
    if (LcdTouch.touched()) {
        LcdTouch.readTouch();
        uint16_t x = LcdTouch.xTouch;
        uint16_t y = LcdTouch.yTouch;

        int ButtonNum = LcdTouch.ButtonTouch(x, y);

        if (ButtonNum == 1) {
            selectedPlantIndex = 0; // כלנית
            selectedPlantName = plants[0].name;
            displayPlantInfo(0);
        } else if (ButtonNum == 2) {
            selectedPlantIndex = 1; // רקפת
            selectedPlantName = plants[1].name;
            displayPlantInfo(1);
        } else if (ButtonNum == 3) {
            selectedPlantIndex = 2; // חמנייה
            selectedPlantName = plants[2].name;
            displayPlantInfo(2);
        } else if (ButtonNum == 4) {
            selectedPlantIndex = 3; // צמח חדש
            selectedPlantName = plantNames[0];
        }
    }
    return selectedPlantIndex;
}

void screenMain() {
  LcdTouch.fillScreen(BLACK);
  LcdTouch.setTextSize(1);
  LcdTouch.setTextColor(WHITE, BLACK);
  LcdTouch.setCursor(40, 20);
  LcdTouch.printheb("ברוכים הבאים לגינה החכמה");
  LcdTouch.println();
  LcdTouch.setCursor(60, 50);
  LcdTouch.printheb("אנא בחרו צמח אותו ברצונכם לגדל");
  LcdTouch.println();

  LcdTouch.drawButton(1, 20, 90, 100, 40, 10, RED, WHITE, "תינלכ",1.5); // NumButton, x, y, width, height, r, Color, textcolor,label, textsize
  LcdTouch.drawButton(2, 140, 90, 100, 40, 10, BLUE, WHITE, "תפקר", 1.5);
  LcdTouch.drawButton(3, 20, 140, 100, 40, 10, GREEN, WHITE, "היינמח", 1.5);
  LcdTouch.drawButton(4, 140, 140, 100, 40, 10, YELLOW, WHITE, "שדח חמצ", 1.5);
}

void handleNewPlant() {
    LcdTouch.fillScreen(BLACK);
    LcdTouch.setTextSize(2);
    LcdTouch.setTextColor(WHITE);
    LcdTouch.setCursor(50, 50);

    float temp = getUserInput("טמפרטורה רצויה:");
    float humidity = getUserInput("מידת לחות:");
    float water = getUserInput("כמות מים נדרשת:");

    // שמירת הנתונים בצמח חדש
    strcpy(newPlant.name, "צמח חדש");
    newPlant.desiredTemperature = temp;
    newPlant.humidityLevel = humidity;
    newPlant.waterAmountNeeded = water;

    // בדיקות נוספות לצמח החדש
    checkPlantConditions(newPlant); // לקרוא לפונקציה שבודקת את התנאים

    // עדכון מערך הצמחים
    plants[numPlants] = newPlant;
    numPlants++;

    displayPlantInfo(3); // הצגת המידע של הצמח החדש
}

void checkPlantConditions(Plant newPlant) {
    // בדיקות התנאים עבור הצמח החדש
    if (newPlant.desiredTemperature > HIGH_TEMP_THRESHOLD) {
        // צריך להתריע על טמפרטורה גבוהה מדי
        Serial.println("התראה: טמפרטורה גבוהה מדי עבור הצמח החדש");
    }
    if (newPlant.waterAmountNeeded > HIGH_WATER_THRESHOLD) {
        // צריך להשקות את הצמח החדש
        Serial.println("התראה: צמח אינו זקוק להשקייה כזאת");
    }
}

void displayPlantInfo(int plantIndex) {
  Plant selectedPlant;
  LcdTouch.fillScreen(BLACK);
  LcdTouch.setTextSize(2);
  LcdTouch.setTextColor(WHITE);
  if (plantIndex == 3) {
    selectedPlant = newPlant;
  } else {
    selectedPlant = plants[plantIndex]; // מופע של הצמח הנבחר
  }

  LcdTouch.setCursor(50, 50);
  LcdTouch.printheb(" :שם");
  LcdTouch.println(selectedPlant.name);

  LcdTouch.setCursor(50, 100);
  LcdTouch.printheb(" :טמפרטורה רצויה");
  LcdTouch.println(selectedPlant.desiredTemperature);

  LcdTouch.setCursor(50, 150);
  LcdTouch.printheb(" :מידת לחות");
  LcdTouch.println(selectedPlant.humidityLevel);

  LcdTouch.setCursor(50, 200);
  LcdTouch.printheb(" :כמות מים נדרשת");
  LcdTouch.println(selectedPlant.waterAmountNeeded);
}

void drawNumberKeypad() {
  LcdTouch.fillScreen(BLACK);
  LcdTouch.setTextSize(2);
  LcdTouch.setTextColor(WHITE);
  LcdTouch.setCursor(50, 40);
  LcdTouch.printheb("הזן ערך:");

  int buttonWidth = 30;
  int buttonHeight = 30;
  int buttonMargin = 5;
  int startX = 20;
  int startY = 70;

  int inputNumber = -1; // Initialize input number to -1 (no input)

  for (int i = 0; i < 10; i++) {
    int x = startX + (i % 3) * (buttonWidth + buttonMargin);
    int y = startY + (i / 3) * (buttonHeight + buttonMargin);
    char label[2];
    itoa(i, label, 10); // Convert number to string
    LcdTouch.drawButton(i + 1, x, y, buttonWidth, buttonHeight, 10,
WHITE, BLACK, label, 2);

    // Check if a button is touched and store the corresponding number
    if (LcdTouch.touched() && LcdTouch.ButtonTouch(x, y)) {
      inputNumber = i;
    }
  }

  startX += (buttonMargin + buttonWidth);

  // Additional buttons for clear, confirm, and decimal point
  LcdTouch.drawButton(11, startX, startY + 3 * (buttonHeight +
buttonMargin), buttonWidth, buttonHeight, 10, WHITE, BLACK, "הקנ",
1.3);
  LcdTouch.drawButton(12, startX + 2 * (buttonWidth + buttonMargin),
startY + 3 * (buttonHeight + buttonMargin), buttonWidth, buttonHeight,
10, WHITE, BLACK, "רושיא", 1.3);
  LcdTouch.drawButton(13, startX + (buttonWidth + buttonMargin),
startY + 3 * (buttonHeight + buttonMargin), buttonWidth, buttonHeight,
10, WHITE, BLACK, ".", 1.5);
}

float getUserInput(const char* prompt) {
  LcdTouch.fillScreen(BLACK);
  LcdTouch.setTextSize(2);
  LcdTouch.setTextColor(WHITE);
  LcdTouch.setCursor(50, 50);
  LcdTouch.printheb(prompt);
  delay(3000);

  drawNumberKeypad(); // הצגת מקלדת מספרים

  char userInput[20] = "";
  bool inputComplete = false;

  while (!inputComplete) {
    if (LcdTouch.touched()) {
      LcdTouch.readTouch();
      uint16_t x = LcdTouch.xTouch;
      uint16_t y = LcdTouch.yTouch;

      int key = LcdTouch.ButtonTouch(x, y);

      if (key == 11) { // מחיקה
        memset(userInput, 0, sizeof(userInput)); // איפוס המחרוזת
        drawNumberKeypad();
      } else if (key == 12) { // אישור
        inputComplete = true;
      } else if (key == 13) { // נקודה עשרונית
        strcat(userInput, ".");
      } else if (key >= 1 && key <= 10) { // מספרים
        char digit[2];
        itoa(key - 1, digit, 10);
        strcat(userInput, digit);
      }

      // עדכון התצוגה עם הקלט
      LcdTouch.fillRect(50, 150, 220, 30, BLACK);
      LcdTouch.setCursor(50, 150);
      LcdTouch.print(userInput);
    }
    delay(100);
  }

  return atof(userInput);
}