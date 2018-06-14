#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define SEED_RAND srand(time(NULL))
#define MAX_LEVELS 5

typedef enum button {B1 = 4, B2 = 25, B3 = 27} BUTTON;
typedef enum led {L1 = 22, L2 = 23, L3 = 24} LED;

volatile unsigned *virtualGpio;

volatile unsigned *setupGpioMap() {
  int memoryFile;
  if ((memoryFile = open("/dev/mem", O_RDWR|O_SYNC)) < 0) {
    return NULL;
  }
  #define PHYSICAL_GPIO 0x20200000
  #define BLOCK_SIZE (4*1024)
  void *gpioMap = mmap(
    NULL,
    BLOCK_SIZE,
    PROT_READ|PROT_WRITE,
    MAP_SHARED,
    memoryFile,
    PHYSICAL_GPIO
  );
  if (close(memoryFile) == -1) {
    return NULL;
  }
  if (gpioMap == MAP_FAILED) {
    return NULL;
  }
  return (volatile unsigned *)gpioMap;
}

void inputPin(int pin) {
  *(virtualGpio + (pin / 10)) &= ~(7 << ((pin % 10) * 3));
}

void outputPin(int pin) {
  *(virtualGpio + (pin / 10)) |=  (1 << ((pin % 10) * 3));
}

bool getPin(int pin) {
  return *(virtualGpio + 13) & (1 << pin);
}

void setPin(int pin) {
  *(virtualGpio+7) = 1 << pin;
}

void clearPin(int pin) {
  *(virtualGpio+10) = 1 << pin;
}

void flashLed(LED led) {
  setPin(led);
  sleep(1);
  clearPin(led);
}

void flashAll(void) {
  setPin(0);
  setPin(1);
  setPin(2);
  sleep(1);
  clearPin(0);
  clearPin(1);
  clearPin(2);
}

void initPins(void) {
  inputPin(B1);
  inputPin(B2);
  inputPin(B3);
  inputPin(L1);
  inputPin(L2);
  inputPin(L3);
  outputPin(L1);
  outputPin(L2);
  outputPin(L3);
  for (int i = 0; i < 3; i++) {
    flashLed(i);
  }
  flashAll();
}

void flashArray(LED *array, int size) {
  for (int i = 0; i < size; i++) {
    flashLed(array[i] + 22);
    sleep(1);
  }
}

bool pollButton(BUTTON expected) {
  bool one = false;
  bool two = false;
  bool three = false;
  switch (expected) {
    case B1:
      one = true;
      break;
    case B2:
      two = true;
      break;
    case B3:
      three = true;
      break;
  }
  do {
    if (getPin(B1)) return one;
    if (getPin(B2)) return two;
    if (getPin(B3)) return three;
  } while (true);
}

LED getRandomLed() {
  switch (rand() % 3) {
    case 1: return L1;
    case 2: return L2;
    case 3: return L3;
    default: return -1;
  }
}

BUTTON ledToButton(LED led) {
  switch (led) {
    case L1: return B1;
    case L2: return B2;
    case L3: return B3;
    default: return -1;
  }
}

bool checkOrder(LED *ledArray, int size) {
  for (int i = 0; i < size; i++) {
    if (!pollButton(ledToButton(ledArray[i]))) {
      return false;
    }
  }
  return true;
}

int main(int argc, char **argv)
{
  if ((virtualGpio = setupGpioMap()) == NULL) {
    return -1;
  }
  SEED_RAND;
  initPins();
  sleep(1);
  int score = 0;
  LED *ledArray = NULL;
  bool running;
  do {
    ledArray = realloc(ledArray, (score + 1) * sizeof(int));
    if (ledArray == NULL) {
      return -1;
    }
    ledArray[score] = getRandomLed();
    flashArray(ledArray, score + 1);
    if ((running = checkOrder(ledArray, score + 1))) {
      score++;
    }
  } while (score < MAX_LEVELS && running);
  for (int i = 0; i < score; i++) {
    flashAll();
  }
  free(ledArray);
  return 0;
}
