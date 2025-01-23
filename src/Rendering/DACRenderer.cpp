#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DACRenderer.h"
#include "driver/dac.h"
#include "driver/timer.h"

#include "ILDAFile.h"

void IRAM_ATTR draw_timer(void *para)
{
  timer_spinlock_take(TIMER_GROUP_0);
  DACRenderer *renderer = static_cast<DACRenderer *>(para);
  renderer->draw();
}

void IRAM_ATTR DACRenderer::draw()
{
  // Clear the interrupt
  timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
  // After the alarm has been triggered we need enable it again, so it is triggered the next time
  timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
  // do we still have things to draw?
  if (draw_position < ilda_files[file_position]->frames[frame_position].number_records)
  {
    const ILDA_Record_t &instruction = ilda_files[file_position]->frames[frame_position].records[draw_position];

    uint8_t output_y = 128 + ( instruction.x * 128) / 32768;
    uint8_t output_x = 128 + (-instruction.y * 128) / 32768;

    if ((instruction.status_code & 0b01000000) == 0)
    {
      set_laser(true);
    }
    else
    {
      set_laser(false);
    }

    dac_output_voltage(DAC_CHANNEL_1, output_x);
    dac_output_voltage(DAC_CHANNEL_2, output_y);

    draw_position++;
    transactions++;
  }
  else
  {
    draw_position = 0;
    frame_position++;
    if (frame_position == ilda_files[file_position]->num_frames)
    {
      frame_position = 0;
      file_position++;
      if (file_position == ilda_files.size())
      {
        file_position = 0;
      }
    }

    rendered_frames++;
  }
  timer_spinlock_give(TIMER_GROUP_0);
}

void timer_setup(void *param)
{
  // set up the renderer timer
  timer_config_t config = {
      .alarm_en = TIMER_ALARM_EN,
      .counter_en = TIMER_PAUSE,
      .intr_type = TIMER_INTR_LEVEL,
      .counter_dir = TIMER_COUNT_UP,
      .auto_reload = TIMER_AUTORELOAD_EN,
      .divider = 4000}; // default clock source is APB
  timer_init(TIMER_GROUP_0, TIMER_0, &config);

  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

  // Configure the alarm value and the interrupt on alarm.
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 0x00000001ULL);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, draw_timer,
                     param, ESP_INTR_FLAG_IRAM, NULL);

  timer_start(TIMER_GROUP_0, TIMER_0);
  while (true)
  {
    vTaskDelay(10000000);
  }
}

void DACRenderer::start()
{
  draw_position = 0;
  // enable the DAC channels for X and Y
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_enable(DAC_CHANNEL_2);

  // make sure to start the task on CPU 1
  TaskHandle_t timer_setup_handle;
  xTaskCreatePinnedToCore(timer_setup, "Draw Task", 4096, this, 0, &timer_setup_handle, 1);

  Renderer::start();
}
