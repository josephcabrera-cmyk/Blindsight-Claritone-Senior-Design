#ifndef TOF_FRONT_H
#define TOF_FRONT_H

#include <stdint.h>

/* 4 sensors: index by direction */
typedef enum {
    TOF_IDX_FRONT = 0,
    TOF_IDX_LEFT  = 1,
    TOF_IDX_RIGHT = 2,
    TOF_IDX_REAR  = 3,
    TOF_COUNT     = 4
} tof_idx_t;

typedef struct {
    uint16_t distance_mm[TOF_COUNT];   /* 0 = invalid / out of range */
    uint8_t  valid[TOF_COUNT];         /* 1 = sensor present and target detected */
    uint8_t  online[TOF_COUNT];        /* 1 = sensor responded at init time */
} tof_array_state_t;

/**
 * @brief Init all 4 ToF sensors. Skips any that don't respond (online=0).
 *        Always succeeds — caller can demo with whatever sensors are present.
 * @retval Number of sensors that came online (0-4).
 */
uint8_t ToF_Array_Init(void);

/**
 * @brief Non-blocking poll. Updates *out for each online sensor that has new data.
 *        Sensors offline at init are skipped permanently.
 * @retval Bitmask of sensors updated this call.
 */
uint8_t ToF_Array_Poll(tof_array_state_t *out);

/* Legacy API kept for back-compat — wraps front sensor. */
typedef struct {
    uint16_t distance_mm;
    int16_t  azimuth_q15;
    uint8_t  valid;
} tof_front_state_t;

uint8_t ToF_Front_Init(void);   /* now calls ToF_Array_Init */
uint8_t ToF_Front_Poll(tof_front_state_t *out);

#endif
