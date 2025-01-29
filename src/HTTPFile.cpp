#include "esp_err.h"
#include "esp_log.h"

#include "HTTPFile.h"

static const char *TAG = "http";

HTTPFile::HTTPFile() /*: stream({})*/
{
}

bool HTTPFile::open(const char *path)
{
    esp_http_client_config_t config = {
        .url = path,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;
    if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
//        free(buffer);
        return false;
    }
//    int content_length =  esp_http_client_fetch_headers(client);
    return true;
}

int HTTPFile::read(uint8_t *dst, size_t num_bytes)
{
        int /*total_read_len = 0,*/ read_len;
        read_len = esp_http_client_read(client, (char *)dst, num_bytes);
        if (read_len <= 0) {
            ESP_LOGE(TAG, "Error read data");
        }
        ESP_LOGD(TAG, "read_len = %d", read_len);
        return read_len;
}

void HTTPFile::close()
{
    ESP_LOGI(TAG, "HTTP Stream reader Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
//    free(buffer);
}
