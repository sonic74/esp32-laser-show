#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

class MyFile
{
//private:

public:
  MyFile();
  bool open(const char *path);
  int read(uint8_t *dst, size_t num_bytes);
  void close();
};