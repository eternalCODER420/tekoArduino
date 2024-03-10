#ifndef MEMORYMANAGEMENT_H //check if already defined/included in compiler
#define MEMORYMANAGEMENT_H //only define if not defined before

const unsigned int FreeMemoryLimit = 1000;
bool memoryLimitReached;

// Author: User "ec2021" on Arduino forum
// Original: https://forum.arduino.cc/t/how-to-create-and-free-dynamic-arrays-with-arduino/934662
// Formatted: https://forum.arduino.cc/t/how-to-create-and-free-dynamic-arrays-with-arduino/934662/12

extern unsigned int __bss_end;
extern void *__brkval;

int getFreeMemory()
{
  int free_memory;
  if ((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
}

#endif