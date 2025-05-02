#ifndef _ED047TC1_H_
#define _ED047TC1_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include <driver/gpio.h>

#include <stdint.h>

#ifndef IRAM_ATTR
#  define IRAM_ATTR
#endif

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

#if CONFIG_IDF_TARGET_ESP32

/* Config Reggister Control */
#define CFG_DATA GPIO_NUM_23
#define CFG_CLK GPIO_NUM_18
#define CFG_STR GPIO_NUM_0

/* Control Lines */
#define CKV GPIO_NUM_25
#define STH GPIO_NUM_26

/* Edges */
#define CKH GPIO_NUM_5

/* Data Lines */
#define D7 GPIO_NUM_22
#define D6 GPIO_NUM_21
#define D5 GPIO_NUM_27
#define D4 GPIO_NUM_2
#define D3 GPIO_NUM_19
#define D2 GPIO_NUM_4
#define D1 GPIO_NUM_32
#define D0 GPIO_NUM_33

#elif CONFIG_IDF_TARGET_ESP32S3

// #if LILYGO_T_EPD47_S_V1
/* Config Reggister Control */
#define CFG_DATA GPIO_NUM_2
#define CFG_CLK GPIO_NUM_42
#define CFG_STR GPIO_NUM_1

/* Control Lines */
#define CKV GPIO_NUM_39
#define STH GPIO_NUM_9

/* Edges */
#define CKH GPIO_NUM_10

/* Data Lines */
#define D7 GPIO_NUM_38
#define D6 GPIO_NUM_45
#define D5 GPIO_NUM_47
#define D4 GPIO_NUM_21
#define D3 GPIO_NUM_14
#define D2 GPIO_NUM_13
#define D1 GPIO_NUM_12
#define D0 GPIO_NUM_11

// #elif LILYGO_T5_EPD47_S3

// /* Config Reggister Control */
// #define CFG_DATA GPIO_NUM_13
// #define CFG_CLK GPIO_NUM_12
// #define CFG_STR GPIO_NUM_0

// /* Control Lines */
// #define CKV GPIO_NUM_38
// #define STH GPIO_NUM_40

// /* Edges */
// #define CKH GPIO_NUM_41

// /* Data Lines */
// #define D7 GPIO_NUM_7
// #define D6 GPIO_NUM_6
// #define D5 GPIO_NUM_5
// #define D4 GPIO_NUM_4
// #define D3 GPIO_NUM_3
// #define D2 GPIO_NUM_2
// #define D1 GPIO_NUM_1
// #define D0 GPIO_NUM_8

// #endif

#else
    #error "Unknown SOC"
#endif

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void old_epd_base_init(uint32_t epd_row_width);
void old_epd_poweron();
void old_epd_poweroff();

/**
 * @brief Start a draw cycle.
 */
void old_epd_start_frame();

/**
 * @brief End a draw cycle.
 */
void old_epd_end_frame();

/**
 * @brief output row data
 *
 * @note Waits until all previously submitted data has been written.
 *       Then, the following operations are initiated:
 *
 *           1. Previously submitted data is latched to the output register.
 *           2. The RMT peripheral is set up to pulse the vertical (gate) driver
 *              for `output_time_dus` / 10 microseconds.
 *           3. The I2S peripheral starts transmission of the current buffer to
 *              the source driver.
 *           4. The line buffers are switched.
 *
 *       This sequence of operations allows for pipelining data preparation and
 *       transfer, reducing total refresh times.
 */
void IRAM_ATTR old_epd_output_row(uint32_t output_time_dus);

/**
 * @brief Skip a row without writing to it.
 */
void IRAM_ATTR old_epd_skip();

/**
 * @brief Get the currently writable line buffer.
 */
uint8_t * IRAM_ATTR old_epd_get_current_buffer();

/**
 * @brief Switches front and back line buffer.
 *
 * @note If the switched-to line buffer is currently in use, this function
 *       blocks until transmission is done.
 */
void IRAM_ATTR old_epd_switch_buffer();

#ifdef __cplusplus
}
#endif

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/
