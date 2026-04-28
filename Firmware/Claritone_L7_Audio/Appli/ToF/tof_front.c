#include "tof_front.h"
#include "platform.h"
#include "vl53l7cx_api.h"
#include "i2c_bb.h"
#include "main.h"
#include <string.h>

static VL53L7CX_Configuration s_dev;
static uint8_t s_initialized = 0;

static inline uint8_t zone_valid(uint8_t status) {
    return (status == 5U) || (status == 6U) || (status == 9U);
}

static void sensor_power_on(void) {
    HAL_GPIO_WritePin(TOF_I2C_RST_GPIO_Port, TOF_I2C_RST_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TOF_LPN_GPIO_Port, TOF_LPN_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(TOF_LPN_GPIO_Port, TOF_LPN_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
}

uint8_t ToF_Front_Init(void) {
    i2c_bb_init();
    sensor_power_on();

    s_dev.platform.address  = 0x52;
    s_dev.platform.lpn_port = TOF_LPN_GPIO_Port;
    s_dev.platform.lpn_pin  = TOF_LPN_Pin;
    s_dev.platform.rst_port = TOF_I2C_RST_GPIO_Port;
    s_dev.platform.rst_pin  = TOF_I2C_RST_Pin;
    s_dev.platform.int_port = TOF_INT_GPIO_Port;
    s_dev.platform.int_pin  = TOF_INT_Pin;

    uint8_t alive = 0;
    if (vl53l7cx_is_alive(&s_dev, &alive) != 0 || !alive) return 1;
    if (vl53l7cx_init(&s_dev) != 0) return 2;
    if (vl53l7cx_set_resolution(&s_dev, VL53L7CX_RESOLUTION_4X4) != 0) return 3;
    if (vl53l7cx_set_ranging_frequency_hz(&s_dev, 15) != 0) return 4;
    if (vl53l7cx_start_ranging(&s_dev) != 0) return 5;

    s_initialized = 1;
    return 0;
}

uint8_t ToF_Front_Poll(tof_front_state_t *out) {
    if (!s_initialized || !out) return 0;

    uint8_t data_ready = 0;
    if (vl53l7cx_check_data_ready(&s_dev, &data_ready) != 0) return 0;
    if (!data_ready) return 0;

    VL53L7CX_ResultsData results;
    if (vl53l7cx_get_ranging_data(&s_dev, &results) != 0) return 0;

    /* Per-column closest, used for both distance and azimuth */
    uint16_t col_closest[4] = { 0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU };
    uint8_t  col_has_valid[4] = { 0, 0, 0, 0 };

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int zone = row * 4 + col;
            if (!zone_valid(results.target_status[zone])) continue;
            int16_t d = results.distance_mm[zone];
            if (d <= 0) continue;
            if ((uint16_t)d < col_closest[col]) {
                col_closest[col] = (uint16_t)d;
                col_has_valid[col] = 1;
            }
        }
    }

    int best_col = -1;
    uint16_t best_dist = 0xFFFFU;
    for (int col = 0; col < 4; col++) {
        if (col_has_valid[col] && col_closest[col] < best_dist) {
            best_dist = col_closest[col];
            best_col = col;
        }
    }

    if (best_col < 0) {
        out->valid = 0;
        out->distance_mm = 0;
        out->azimuth_q15 = 0;
        return 1;
    }

    /* Sensor's column 0 is on its left side. User behind the sensor: sensor's left = user's right. */
    static const int16_t col_to_azimuth_q15[4] = {
        +32767,  /* col 0: user right full */
        +10922,  /* col 1: user right partial */
        -10922,  /* col 2: user left partial */
        -32767,  /* col 3: user left full */
    };

    out->azimuth_q15 = col_to_azimuth_q15[best_col];
    out->distance_mm = best_dist;
    out->valid = 1;
    return 1;
}
