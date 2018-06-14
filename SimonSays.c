#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
//#include "linkedList.c"

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;
volatile unsigned *gpio;

#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

void setup_io() {
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );
   close(mem_fd); //No need to keep mem_fd open after mmap
   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }
   gpio = (volatile unsigned *)gpio_map;
}

void printButton(int g) {
  if (GET_GPIO(g)) // !=0 <-> bit is 1 <- port is HIGH=3.3V
    printf("Button pressed!\n");
  else // port is LOW=0V
    printf("Button released!\n");
}

/*
LED getRandomLed(void) {
  return (rand() % 3) + 22;
}
*/

#define getRandomPin (rand() % 3)
#define initRand srand(time(NULL))

inline void setPin(int pin) {
  GPIO_SET = 1 << (pin + 22);
}

inline void clearPin(int pin) {
  GPIO_CLR = 1 << (pin + 22);
}

inline void flashPin(int pin) {
  setPin(pin);
  sleep(1);
  clearPin(pin);
}

inline void initPins(void) {
  for (int i = 0; i < 4; i++) {
    INP_GPIO(i + 22);
    OUT_GPIO(i + 22);
    flashPin(i);
  }
}

inline void flashArray(int *array, int size) {
  for (int i = 0; i < size; i++) {
    flashPin(array[i]);
  }
}

int main(int argc, char **argv)
{
  int score = 0;
  int *array;
  initRand;
  initPins();
  do {
    array = realloc(array, (score + 1) * sizeof(int));
    array[score] = getRandomPin;
    flashArray(array, score + 1);
    sleep(1);
  } while (true);

  return 0;
}
