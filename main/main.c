/* I2C example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "driver/i2c.h"


static const char *TAG = "main";


#define I2C_EXAMPLE_MASTER_SCL_IO           5                /*!< gpio number for I2C master clock */
#define I2C_EXAMPLE_MASTER_SDA_IO           4               /*!< gpio number for I2C master data  */
#define I2C_EXAMPLE_MASTER_NUM              I2C_NUM_0        /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE   0                /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE   0                /*!< I2C master do not need buffer */

#define WRITE_BIT                           I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                            I2C_MASTER_READ  /*!< I2C master read */

static esp_err_t i2c__master_init()
{
    int i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = 0;
    conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = 0;
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode));
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    return ESP_OK;
}


static void i2c_task_example(void *arg)
{
	i2c__master_init();
        int i;
        esp_err_t espRc;
        printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
        printf("00:         ");
        for (i=3; i< 0x78; i++) {
                i2c_cmd_handle_t cmd = i2c_cmd_link_create();
                i2c_master_start(cmd);
                i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
                i2c_master_stop(cmd);

                espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
                if (i%16 == 0) {
                        printf("\n%.2x:", i);
                }
                if (espRc == 0) {
                        printf(" %.2x", i);
                } else {
                        printf(" --");
                }
                //ESP_LOGD(tag, "i=%d, rc=%d (0x%x)", i, espRc, espRc);
                i2c_cmd_link_delete(cmd);
        }
        printf("\n");
        vTaskDelete(NULL);


}

void app_main(void)
{
    //start i2c task
    xTaskCreate(i2c_task_example, "i2c_task_example", 2048, NULL, 10, NULL);
}
