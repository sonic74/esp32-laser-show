#include "Renderer.h"
#include "ILDAFile.h"
//#include "driver/gpio.h"
#include "driver/ledc.h"
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
//#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_DUTY               (4) // Set duty to 25%. (2 ** 2 = 4) * 25% = 1

#include "esp_log.h"
static const char *TAG = "Renderer";

Renderer::Renderer(const std::vector<ILDAFile *> &ilda_files) : ilda_files(ilda_files)
{
  file_position = 0;
  frame_position = 0;
  draw_position = 0;

  errors = 0;
  rendered_frames = 0;
  transactions = 0;
  switches = 0;
  switches_off = 0;
}

void Renderer::start()
{
  // setup the laser output
  gpio_reset_pin(PIN_NUM_LASER);
  gpio_pad_select_gpio(PIN_NUM_LASER);
  if(gpio_set_direction(PIN_NUM_LASER, GPIO_MODE_OUTPUT) != ESP_OK) {
    errors++;
  }
//gpio_dump_io_configuration(stdout, (1ULL << 4) | (1ULL << 18) | (1ULL << 26));

/*
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_TIMER_2_BIT,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = (400000),  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = PIN_NUM_LASER,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));*/
  
  ESP_LOGI(TAG, "started");
}

void IRAM_ATTR Renderer::set_laser(bool on)
{
  if(gpio_set_level(PIN_NUM_LASER, on ? 1 : 0) != ESP_OK) {
    errors++;
  } else {
    if(on) switches++;
    else switches_off++;
  }
 /*
    // Set duty to 50%
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, on ? LEDC_DUTY : 0));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));*/
}
