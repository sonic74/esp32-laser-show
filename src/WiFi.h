#include <esp_http_server.h>

extern "C"
{
  void wifi_init_sta(void);
  httpd_handle_t start_webserver(void);
}