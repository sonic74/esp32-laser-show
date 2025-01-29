#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "MyFile.h"
#include "esp_http_client.h"

class HTTPFile : public MyFile
{
private:
  esp_http_client_handle_t client;
//  uint8_t *in_buffer;

public:
  HTTPFile();
  bool open(const char *path);
  int read(uint8_t *dst, size_t num_bytes);
  void close();
};