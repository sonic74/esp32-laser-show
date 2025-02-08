#include "Renderer.h"
#include "ILDAFile.h"
//#include "driver/gpio.h"
#define PWM
#ifdef PWM
#include "driver/ledc.h"
//#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
//#define LEDC_DUTY               (1) // Set duty to 25%. (2 ** 2 = 4) * 25% = 1
//#define LEDC_DUTY               (2) // Set duty to 100%. (2 ** 1 = 2) * 100% = 2
//#define LEDC_DUTY               (253) // Set duty to 99%. (2 ** 8 = 256) * 99% = 253
#define LEDC_DUTY               (255) // Set duty to 100%. (2 ** 8 = 256) * 100% = 256 - 1 for PWM
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#endif

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
#ifndef PWM
  // setup the laser output
  gpio_reset_pin(PIN_NUM_LASER);
  gpio_pad_select_gpio(PIN_NUM_LASER);
  if(gpio_set_direction(PIN_NUM_LASER, GPIO_MODE_OUTPUT) != ESP_OK) {
    errors++;
  }
//gpio_dump_io_configuration(stdout, (1ULL << 4) | (1ULL << 18) | (1ULL << 26));
#else
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = (100*1000),
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
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
#endif
  
  ESP_LOGI(TAG, "started");
}

void IRAM_ATTR Renderer::set_laser(bool on)
{
#ifndef PWM
  if(gpio_set_level(PIN_NUM_LASER, on ? 1 : 0) != ESP_OK) {
    errors++;
  } else {
    if(on) switches++;
    else switches_off++;
  }
#else
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, !on ? LEDC_DUTY : 0));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
#endif
}
