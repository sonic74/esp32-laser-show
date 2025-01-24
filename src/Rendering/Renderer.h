#ifndef _renderer_h_
#define _renderer_h_

#include "driver/gpio.h"
#include <vector>

#define PIN_NUM_LASER GPIO_NUM_32
//#define PIN_NUM_LASER GPIO_NUM_19
//#define PIN_NUM_LASER GPIO_NUM_27

class ILDAFile;

class Renderer
{
protected:
  const std::vector<ILDAFile *> &ilda_files;

public:
  Renderer(const std::vector<ILDAFile *> &ilda_files);

  virtual void start() = 0;
  void IRAM_ATTR set_laser(bool on);

  volatile int draw_position;
  volatile int frame_position;
  volatile int file_position;

  volatile int errors;
  volatile int rendered_frames;
  volatile int transactions;
  volatile int switches;
  volatile int switches_off;
};

#endif