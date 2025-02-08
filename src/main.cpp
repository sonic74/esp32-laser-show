#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <arpa/inet.h>
#include <vector>

#include "ILDAFile.h"
#include "WiFi.h"
#include "Rendering/DACRenderer.h"
//#include "Rendering/SPIRenderer.h"

static const char *TAG = "main";

#include "sd_card_example_main.h"

extern "C"
{
  void app_main(void);
}

static const char *files[] = {
    /*"/spiffs/065.ild.gz",
    "/spiffs/154.ild.gz",
    "/spiffs/033.ild.gz",*/
    "/spiffs/ildatest.ild.gz"/*,
    "/spiffs/Xwing.ild.gz",
    "/spiffs/LemmTumble.ild.gz",
    "/spiffs/Vader.ild.gz",
    "/spiffs/Enterprise2.ild.gz",
    "/spiffs/Ladylegs.ild.gz",
    "/spiffs/BARNEY19.ILD.gz",
    "/spiffs/CanadaFlag.ild.gz",
    "/spiffs/CanGoose.ild.gz",
    "/spiffs/HIPHOP18.ILD.gz",
    "/spiffs/Horse.ILD.gz",
    "/spiffs/Boxer.ILD.gz",*/
//    "https://www.laserfx.com/Backstage.LaserFX.com/Systems/ildatest.ild"
    };
static const int num_files = sizeof(files)/sizeof(files)[0];

void app_main()
{
  vTaskDelay(2000 / portTICK_PERIOD_MS);


//  sd_card_example_main();
/*wifi_init_sta();
start_webserver();*/


      ESP_LOGI(TAG, "num_files=%d", num_files);
  esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = num_files + 1,
      .format_if_mount_failed = false};

  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    }
    else if (ret == ESP_ERR_NOT_FOUND)
    {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    }
    else
    {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
    return;
  }
  // read all the files in
  std::vector<ILDAFile *> ilda_files;
  for (int i = 0; i < num_files; i++)
  {
    ILDAFile *ilda = new ILDAFile();
    ilda->read(files[i]);
    ilda_files.push_back(ilda);
    // feed the watchdog so we don't get a timeout
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  esp_vfs_spiffs_unregister(NULL);

  // Renderer *renderer = new SPIRenderer(ilda_files);
  Renderer *renderer = new DACRenderer(ilda_files);
  renderer->start();

  volatile int rendered_frames_old=renderer->rendered_frames;
  volatile int transactions_old=renderer->transactions;
  // run forever
  while (true)
  {
    vTaskDelay(/*600000*/1000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Rendered frames %d, FPS %d, PPS %d, Free RAM %d, switches=%d, switches_off=%d, errors=%d",
             renderer->rendered_frames,
             renderer->rendered_frames-rendered_frames_old,
             renderer->transactions-transactions_old,
             esp_get_free_heap_size(),
             renderer->switches,
             renderer->switches_off,
             renderer->errors
             );
    rendered_frames_old=renderer->rendered_frames;
    transactions_old=renderer->transactions;
  }
}