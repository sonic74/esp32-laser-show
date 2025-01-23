#ifndef _dac_renderer_h_
#define _dac_renderer_h_

#include "Renderer.h"

typedef struct spi_device_t *spi_device_handle_t; ///< Handle for a device on a SPI bus

class DACRenderer : public Renderer
{
private:
  TaskHandle_t spi_task_handle;
  void IRAM_ATTR draw();

public:
  DACRenderer(const std::vector<ILDAFile *> &ilda_files) : Renderer(ilda_files) {}
  void start();
  friend void draw_timer(void *para);
};

#endif