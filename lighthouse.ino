#include <HID-Project.h>    // Используем только HID-Project.h

// Пины энкодеров
#define CLK1 15
#define DT1 14
#define SW1 16

#define CLK2 10
#define DT2 9
#define SW2 8

// Пины матрицы кнопок
#define NUM_OUTPUTS 3
#define NUM_INPUTS 2

const int outputPins[NUM_OUTPUTS] = {7, 6, 5}; // Строки матрицы кнопок (ROWS)
const int inputPins[NUM_INPUTS] = {2, 4};      // Столбцы матрицы кнопок (COLS)

// Массив для отображения кнопок в матрице
const int buttonMapping[NUM_OUTPUTS][NUM_INPUTS] = {
  {0, 1}, // Кнопки, связанные с outputPins[0]
  {2, 3}, // Кнопки, связанные с outputPins[1]
  {4, 5}  // Кнопки, связанные с outputPins[2]
};

// Переменные для энкодеров
int lastStateCLK1;
int lastStateCLK2;
bool lastStateSW1;
bool lastStateSW2;

// Переменные для кнопок
bool lastButtonState[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

void setup() {
  // Настройка энкодеров
  pinMode(CLK1, INPUT_PULLUP);
  pinMode(DT1, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);

  pinMode(CLK2, INPUT_PULLUP);
  pinMode(DT2, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);

  // Настройка матрицы кнопок
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], HIGH); // Инициализируем строки в HIGH
  }

  for (int i = 0; i < NUM_INPUTS; i++) {
    pinMode(inputPins[i], INPUT_PULLUP); // Инициализируем столбцы с подтяжкой
  }

  Serial.begin(9600);

  // Чтение начального состояния энкодеров
  lastStateCLK1 = digitalRead(CLK1);
  lastStateCLK2 = digitalRead(CLK2);
  lastStateSW1 = digitalRead(SW1);
  lastStateSW2 = digitalRead(SW2);

  // Инициализация библиотеки HID-Project
  Keyboard.begin();     // Для клавиатурных функций
  Consumer.begin();     // Для мультимедийных функций
}

void loop() {
  // Обработка энкодера 1 (управление громкостью)
  handleEncoder1();

  // Обработка энкодера 2 (скролл страницы вверх и вниз)
  handleEncoder2();

  // Обработка кнопок
  handleButtons();
}

void handleEncoder1() {
  int currentStateCLK1 = digitalRead(CLK1);
  if (currentStateCLK1 != lastStateCLK1) {
    if (digitalRead(DT1) != currentStateCLK1) {
      // Увеличение громкости
      Consumer.write(MEDIA_VOLUME_UP);
    } else {
      // Уменьшение громкости
      Consumer.write(MEDIA_VOLUME_DOWN);
    }
    Serial.println("Encoder 1 rotated");
  }
  lastStateCLK1 = currentStateCLK1;

  // Обработка кнопки на энкодере 1 (Mute)
  bool currentStateSW1 = digitalRead(SW1);
  if (currentStateSW1 == LOW && lastStateSW1 == HIGH) {
    Consumer.write(MEDIA_VOLUME_MUTE); // Отключение звука
    Serial.println("Encoder 1 Button pressed (Mute)");
    delay(300);
  }
  lastStateSW1 = currentStateSW1;
}

void handleEncoder2() {
  int currentStateCLK2 = digitalRead(CLK2);
  if (currentStateCLK2 != lastStateCLK2) {
    if (digitalRead(DT2) != currentStateCLK2) {
      // Скролл вниз (клавиша PAGE_DOWN)
      Keyboard.press(KEY_PAGE_DOWN);
      delay(5);
      Keyboard.release(KEY_PAGE_DOWN);
    } else {
      // Скролл вверх (клавиша PAGE_UP)
      Keyboard.press(KEY_PAGE_UP);
      delay(5);
      Keyboard.release(KEY_PAGE_UP);
    }
    Serial.println("Encoder 2 rotated");
  }
  lastStateCLK2 = currentStateCLK2;

  // Обработка кнопки на энкодере 2 (например, HOME)
  bool currentStateSW2 = digitalRead(SW2);
  if (currentStateSW2 == LOW && lastStateSW2 == HIGH) {
    Keyboard.press(KEY_HOME);
    delay(300);
    Keyboard.release(KEY_HOME);
    Serial.println("Encoder 2 Button pressed (Home)");
  }
  lastStateSW2 = currentStateSW2;
}

void handleButtons() {
  checkButton(0, outputPins[0], inputPins[0], '1');
  checkButton(1, outputPins[0], inputPins[1], '2');
  checkButton(2, outputPins[1], inputPins[0], '3');
  checkButton(3, outputPins[1], inputPins[1], '4');
  checkButton(4, outputPins[2], inputPins[0], '5');
  checkButton(5, outputPins[2], inputPins[1], '6');
}

void checkButton(int index, int rowPin, int colPin, char keycode) {
  digitalWrite(rowPin, LOW); // Активируем строку
  bool buttonState = digitalRead(colPin);
  if (buttonState == LOW && lastButtonState[index] == HIGH) {
    Keyboard.write(keycode);
    delay(50);
  }
  lastButtonState[index] = buttonState;
  digitalWrite(rowPin, HIGH); // Деактивируем строку
}