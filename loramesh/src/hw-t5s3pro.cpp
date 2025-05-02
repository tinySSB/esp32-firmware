// hw-t5s3pro.cpp

// collects hardware-specific init code (other than UI) for T5s3pro board

#include "lib/tinySSBlib.h"
#include "hardware.h"

// ---------------------------------------------------------------------------
#ifdef TINYSSB_BOARD_T5S3PRO

// https://github.com/Xinyuan-LilyGO/T5S3-4.7-e-paper-PRO/blob/H752-01/examples/factory/main/utilities.h
// #define UI_T5_EPARPER_S3_PRO_VERSION    "v1.6-250308"  // Software version
// #define BOARD_T5_EPARPER_S3_PRO_VERSION "v1.0-241224"  // Hardware version

struct board_s {
    uint8_t _SCL;
    uint8_t _SDA;
    uint8_t _SPI_MISO;
    uint8_t _SPI_MOSI;
    uint8_t _SPI_SCLK;
    uint8_t _TOUCH_SCL;
    uint8_t _TOUCH_SDA;
    uint8_t _TOUCH_INT;
    uint8_t _TOUCH_RST;
    uint8_t _RTC_SCL;
    uint8_t _RTC_SDA;
    uint8_t _RTC_IRQ;
    uint8_t _SD_MISO;
    uint8_t _SD_MOSI;
    uint8_t _SD_SCLK;
    uint8_t _SD_CS;
    uint8_t _LORA_MISO;
    uint8_t _LORA_MOSI;
    uint8_t _LORA_SCLK;
    uint8_t _LORA_CS  ;
    uint8_t _LORA_IRQ ;
    uint8_t _LORA_RST ;
    uint8_t _LORA_BUSY;
    uint8_t _GPS_RXD;
    uint8_t _GPS_TXD;
    uint8_t _BL_EN;
    uint8_t _BATT_PIN;
    uint8_t _BOOT_BTN;
    uint8_t _KEY_BTN;
    uint8_t _I2C_PORT;
    uint8_t _PCA9535_INT;
};

struct board_s board_preDec2024 = {
    ._SCL = 5,
    ._SDA = 6,

    ._SPI_MISO = 8,
    ._SPI_MOSI = 17,
    ._SPI_SCLK = 18,

    ._TOUCH_SCL = 5, // BOARD_SCL,
    ._TOUCH_SDA = 6, // BOARD_SDA,
    ._TOUCH_INT = 15,
    ._TOUCH_RST = 41,

    ._RTC_SCL   = 5, // BOARD_SCL,
    ._RTC_SDA   = 6, // BOARD_SDA,
    ._RTC_IRQ   = 7,

    ._SD_MISO   = 8,  // BOARD_SPI_MISO,
    ._SD_MOSI   = 17, // BOARD_SPI_MOSI,
    ._SD_SCLK   = 18, // BOARD_SPI_SCLK,
    ._SD_CS     = 16,

    ._LORA_MISO = 8,  // BOARD_SPI_MISO,
    ._LORA_MOSI = 17, // BOARD_SPI_MOSI,
    ._LORA_SCLK = 18, // BOARD_SPI_SCLK,
    ._LORA_CS   = 46,
    ._LORA_IRQ  = 3,
    ._LORA_RST  = 43,
    ._LORA_BUSY = 44,

    ._GPS_RXD   = 0,
    ._GPS_TXD   = 0,

    ._BL_EN     = 40,
    ._BATT_PIN  = 4,
    ._BOOT_BTN  = 0,
    ._KEY_BTN   = 48,

    ._I2C_PORT  = 255,  // disable
    ._PCA9535_INT = 0,  // disable
};

struct board_s board_Dec2024 = {
    ._SCL      = 40,
    ._SDA      = 39,

    ._SPI_MISO  = 21,
    ._SPI_MOSI  = 13,
    ._SPI_SCLK  = 14,

    ._TOUCH_SCL = 40, // BOARD_SCL,
    ._TOUCH_SDA = 39, // BOARD_SDA,
    ._TOUCH_INT = 3,
    ._TOUCH_RST = 9,

    ._RTC_SCL   = 40, // BOARD_SCL,
    ._RTC_SDA   = 39, // BOARD_SDA,
    ._RTC_IRQ   = 2,

    ._SD_MISO   = 21, // BOARD_SPI_MISO,
    ._SD_MOSI   = 13, // BOARD_SPI_MOSI,
    ._SD_SCLK   = 14, // BOARD_SPI_SCLK,
    ._SD_CS     = 12,

    ._LORA_MISO = 21, // BOARD_SPI_MISO,
    ._LORA_MOSI = 13, // BOARD_SPI_MOSI,
    ._LORA_SCLK = 14, // BOARD_SPI_SCLK,
    ._LORA_CS   = 46,
    ._LORA_IRQ  = 10,
    ._LORA_RST  = 1,
    ._LORA_BUSY = 47,

    ._GPS_RXD   = 44,
    ._GPS_TXD   = 43,

    ._BL_EN     = 11,
    ._BOOT_BTN  = 0,
    ._KEY_BTN   = 48,
    ._I2C_PORT  = 0,
    ._PCA9535_INT = 38,
};

// #define T5S3PRO_OLD_HARDWARE // old hardware (< 20241224)

#if defined(T5S3PRO_OLD_HARDWARE) // old hardware (< 20241224)

    #include "epd_driver.h"
    // #define EPD_REFRESH_TIME 150

    void pca9555_read_config(int a, int b) {};
    uint8_t pca9555_read_input(int a, int b) { return 0; };
    void pca9555_read_write(int a, int b) {};
    void pca9555_read_inversion(int a, int b) {};

#define epd_clear_area_cycles    old_epd_clear_area_cycles
#define epd_draw_grayscale_image old_epd_draw_grayscale_image
#define epd_full_screen          old_epd_full_screen
#define epd_init                 old_epd_init
#define epd_poweroff             old_epd_poweroff
#define epd_poweron              old_epd_poweron


#else // new hardware (v1.0-20241224)

    // #define CONFIG_EPD_DISPLAY_TYPE_ED047TC2
    #include "epdiy.h"

    #define WAVEFORM EPD_BUILTIN_WAVEFORM
    #define DEMO_BOARD epd_board_v7
    EpdiyHighlevelState hl;

    #define Rect_t EpdRect

    void epd_draw_grayscale_image(Rect_t a, uint8_t *packedpixels)
    {
        epd_copy_to_framebuffer(a, packedpixels, epd_hl_get_framebuffer(&hl));
        // epd_hl_update_area(&hl, (EpdDrawMode)((int)MODE_GL16 | PREVIOUSLY_WHITE),
        epd_hl_update_area(&hl, MODE_GC16, epd_ambient_temperature(), a);
    }

    #define EPD_HEIGHT          (ED047TC2.height)
    #define EPD_WIDTH           (ED047TC2.width)
    // #define EPD_REFRESH_TIME    (150)
    
#endif // new hardware (v1.0-20241224)

// first hypothesis is that this is a new board. If an online test
// fails, we will switch to the preDec2024 board:
struct board_s *board = &board_Dec2024;

// ---------------------------------------------------------------------------

#include <Arduino.h>
#include <driver/i2c.h>
#include <EEPROM.h>
#include <FS.h>
#include <SPI.h>
// #include <SD.h>
// #include <WiFi.h>
#include <Wire.h>

// #include "firasans.h"
#include "TouchDrvGT911.hpp"
#include "SensorPCF8563.hpp"
#include <RadioLib.h>
#include "bq27220.h"
#define XPOWERS_CHIP_BQ25896
#include <XPowersLib.h>
#include "lvgl.h"

// ---------------------------------------------------------------------------

XPowersPPM PPM;
BQ27220 bq27220;
SensorPCF8563 rtc;
TouchDrvGT911 touch;

bool touchOnline = false;
void (*home_button_fct)(void);

void home_btn_event(void *user_data)
{
    Serial.printf("home button pressed\r\n");
    if (home_button_fct)
        home_button_fct();
}

// ---------------------------------------------------------------------------

static char bl_level;

void set_backlight(int lvl)
{
    switch (lvl)
    {
        case 0: analogWrite(board->_BL_EN, 0); break;
        case 1: analogWrite(board->_BL_EN, 50); break;
        case 2: analogWrite(board->_BL_EN, 100); break;
        case 3: analogWrite(board->_BL_EN, 230); break;
        default:
            break;
    }
    bl_level = lvl;
}

void set_backlight_delta(int d)
{
    bl_level += d;
    if ( bl_level < 0) bl_level = 0;
    if ( bl_level > 3) bl_level = 3;

    set_backlight(bl_level);
}

void get_battery_status(int *isCharging, int *level, bool *vbusIn)
{
    Serial.printf("#  PPM.chargeStatus=%d, bq27220.input=%d\r\n",
                  PPM.chargeStatus(), bq27220.getIsCharging());

    if (isCharging) {
        int c = PPM.chargeStatus();
        *isCharging = c > 0 && c < 4;
    }

    if (level)
        *level = bq27220.getStateOfCharge();

    if (vbusIn)
        *vbusIn = PPM.isVbusIn();
}

// ---------------------------------------------------------------------------

SX1262 sx1262_radio = NULL;
int transmissionState = RADIOLIB_ERR_NONE;
volatile bool transmittedFlag = false;

void set_transmit_flag(void){
    transmittedFlag = true;
}

// receive
int receivedState = RADIOLIB_ERR_NONE;
volatile bool receivedFlag = false;

void set_receive_flag(void){
    receivedFlag = true;
}

bool lora_hw_init(void)
{
    sx1262_radio = new Module(board->_LORA_CS, board->_LORA_IRQ,
                              board->_LORA_RST, board->_LORA_BUSY);

    int state = sx1262_radio.begin();
    if (state != RADIOLIB_ERR_NONE)
        Serial.printf("radio.begin() failed, code %d\r\n", state);
    else
        sx1262_radio.setPacketSentAction(set_transmit_flag);

    // over current protection limit (accepted range is 45 - 240 mA)
    // NOTE: set value to 0 to disable overcurrent protection
    if (sx1262_radio.setCurrentLimit(140) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT)
        Serial.println("sx1262: selected current limit is invalid");

    /*
    // set LoRa sync word to 0xAB
    if (sx1262_radio.setSyncWord(0xAB) != RADIOLIB_ERR_NONE) {
        Serial.println(F("Unable to set sync word!"));
        while (true);
    }

    // set output power to 10 dBm (accepted range is -17 - 22 dBm)
    if (sx1262_radio.setOutputPower(22) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
        while (true);
    }


    // set LoRa preamble length to 15 symbols (accepted range is 0 - 65535)
    if (sx1262_radio.setPreambleLength(15) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
        Serial.println(F("Selected preamble length is invalid for this module!"));
        while (true);
    }

    // disable CRC
    if (sx1262_radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
        Serial.println(F("Selected CRC is invalid for this module!"));
        while (true);
    }
    */

    // Some SX126x modules have TCXO (temperature compensated crystal
    // oscillator). To configure TCXO reference voltage,
    // the following method can be used.
    if (sx1262_radio.setTCXO(2.4) == RADIOLIB_ERR_INVALID_TCXO_VOLTAGE)
        Serial.println("sx1262: selectd TCXO voltage is invalid");

    // Some SX126x modules use DIO2 as RF switch. To enable
    // this feature, the following method can be used.
    // NOTE: As long as DIO2 is configured to control RF switch,
    //       it can't be used as interrupt pin!
    if (sx1262_radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE)
        Serial.println("sx1262: failed to set DIO2 as RF switch");

    return true;
}


// ---------------------------------------------------------------------------

#include "../assets/t5s3pro/img_bground.h"
#include "../assets/t5s3pro/img_sleeping.h"

#define DISP_BUF_SIZE (EPD_WIDTH*EPD_HEIGHT)
uint8_t *epd_packedpixels = NULL;
// volatile bool disp_flush_enabled = true;
// bool disp_refr_is_busy = false;
// uint32_t epd_refr_data = 0;
uint16_t refr_cycle = 50;
uint16_t refr_times = 1;

void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    // if (disp_flush_enabled) {
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t w = lv_area_get_width(area);
    uint16_t h = lv_area_get_height(area);
    uint16_t h2 = (h % 2) ? (h+1) : h;
    lv_color_t *t32 = color_p; // (lv_color32_t *)color_p;

    Serial.printf("[disp_flush] area: %d/%d, %d*%d\r\n",
                  x1, y1, w, h);
    if (1) { // w == EPD_HEIGHT && h == EPD_WIDTH) {
        memset(epd_packedpixels, 255, w*h2/2);
        // map color pixels to grayscale value, rotate while filling in
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int row = w - 1 - x;
                int col = y;
                int ndx = row * h2 / 2 + col/2;
                int r = LV_COLOR_GET_R(*t32);
                int g = LV_COLOR_GET_G(*t32);
                int b = LV_COLOR_GET_B(*t32);
                uint8_t gray = (uint8_t)(r*0.1447 + g*0.1398 + b*0.0552) & 0x0f;
                uint8_t pix = epd_packedpixels[ndx];
                if (col % 2 == 1)
                    pix = (pix & 0x0f) | (gray << 4);
                else
                    pix = (pix & 0xf0) | gray;
                epd_packedpixels[ndx] = pix;
                t32++;
            }
        }
        Rect_t a;
        a.x = y1, a.y = EPD_HEIGHT - x1 - w, a.width = h, a.height = w;
        // Serial.printf(" %d/%d, %d*%d\r\n", a.x, a.y, a.width, a.height);

        epd_poweron();
        // epd_clear();
        // epd_clear_area_cycles(a, 1, 100);

#if defined(T5S3PRO_OLD_HARDWARE) // old hardware (< 20241224)
        // epd_clear();
        epd_clear_area_cycles(epd_full_screen(), 1, 50);
#else
        // epd_hl_set_all_white(&hl);
        // epd_hl_update_area(&hl, MODE_DU, epd_ambient_temperature(), a);
#endif
        epd_draw_grayscale_image(a, epd_packedpixels);
        // delay(500);
        epd_poweroff();
    }

    // buggy code from Lilygo
    /*
    for(int i = 0; i < (w * h) / 2; i++) {
        lv_color8_t ret;
        LV_COLOR_SET_R8(ret, LV_COLOR_GET_R(*t32) >> 5); // 8 - 3  = 5
        LV_COLOR_SET_G8(ret, LV_COLOR_GET_G(*t32) >> 5); // 8 - 3  = 5
        LV_COLOR_SET_B8(ret, LV_COLOR_GET_B(*t32) >> 6); // 8 - 2  = 6
        epd_packedpixels[i] = ret.full;
        t32++;
    }
    */

    lv_disp_flush_ready(disp);
}

/*
void disp_refresh_cb(lv_timer_t *t)
{
    lv_timer_del(t);
    disp_refr_is_busy = false;

    epd_poweron();
    // epd_clear();
    epd_clear_area_cycles(epd_full_screen(), refr_times, refr_cycle);
    epd_draw_grayscale_image(epd_full_screen(), epd_packedpixels);
    // epd_draw_image(epd_full_screen(), (uint8_t *)epd_packedpixels, BLACK_ON_WHITE);
    epd_poweroff();
}
*/
/*
void disp_refresh_cycle_cb(lv_timer_t *t)
{
    uint16_t cycle = (epd_refr_data >> 16) & 0x0000FFFF;
    uint16_t times = (epd_refr_data & 0x0000FFFF);

    lv_timer_del(t);
    disp_refr_is_busy = false;

    epd_poweron();
    // epd_clear();
    // Serial.printf("t=%d, c=%d\r\n", times, cycle);
    epd_clear_area_cycles(epd_full_screen(), times, cycle);
    epd_draw_grayscale_image(epd_full_screen(), (uint8_t *)epd_packedpixels);
    epd_poweroff();
}
*/

/*
void disp_manual_refr(uint16_t time)
{
    if (disp_refr_is_busy == false) {
        lv_timer_create(disp_refresh_cb, time, NULL);
        disp_refr_is_busy = true;
    }
}
*/

void my_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    int16_t  x, y;
    if (touchOnline) {
        if(touch.getPoint(&x, &y)){
            data->point.x = x;
            data->point.y = y;
            data->state = LV_INDEV_STATE_PRESSED;
            Serial.printf("input X:%d Y:%d\r\n", data->point.x, data->point.y);
        }else {
            data->state = LV_INDEV_STATE_RELEASED; 
        }
    }
}

static void scr_event_cb(lv_event_t *e)
{
    // Serial.printf("screen %d\r\n", lv_event_get_code(e));

    //
    // if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) {
    //     lv_event_stop_processing(e);
    // }
}

void lvgl_setup(void)
{
    lv_init();
    String LVGL_Arduino = String("# LVLG Arduino ") + lv_version_major() +
                          "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println( LVGL_Arduino );

    lv_group_set_default(lv_group_create());

    static lv_disp_draw_buf_t draw_buf;

    lv_color_t *buf1 = (lv_color_t *)ps_calloc(sizeof(lv_color_t), DISP_BUF_SIZE);
    // lv_color_t *buf2 = (lv_color_t *)ps_calloc(sizeof(lv_color_t), DISP_BUF_SIZE);
    // lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DISP_BUF_SIZE);
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, DISP_BUF_SIZE);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EPD_HEIGHT;
    disp_drv.ver_res = EPD_WIDTH;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = false; // 1
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_input_read;
    lv_indev_drv_register(&indev_drv);

    lv_obj_t *scr = lv_scr_act();
    lv_obj_add_event_cb(scr, scr_event_cb, LV_EVENT_ALL, NULL);

    // background
    lv_obj_t *img = lv_img_create(scr);
    lv_img_set_src(img, &img_bground);
    // lv_obj_set_pos(img, 0, 0);
    lv_refr_now(NULL);
    // lv_task_handler();
}

// ---------------------------------------------------------------------------

bool setupGPS()
{
    bool result = false;
    uint32_t startTimeout ;

    for (int i = 0; i < 3; ++i) {
        Serial2.write("$PCAS03,0,0,0,0,0,0,0,0,0,0,,,0,0*02\r\n");
        delay(5);
        // Get version information
        startTimeout = millis() + 3000;
        Serial.print("Try to init L76K . Wait stop .");
        while (Serial2.available()) {
            Serial.print(".");
            Serial2.readString();
            if (millis() > startTimeout) {
                Serial.println("Wait L76K stop NMEA timeout!");
                return false;
            }
        };
        Serial.println();
        Serial2.flush();
        delay(200);

        Serial2.write("$PCAS06,0*1B\r\n");
        startTimeout = millis() + 500;
        String ver = "";
        while (!Serial2.available()) {
            if (millis() > startTimeout) {
                Serial.println("Get L76K timeout!");
                return false;
            }
        }
        Serial2.setTimeout(10);
        ver = Serial2.readStringUntil('\n');
        if (ver.startsWith("$GPTXT,01,01,02")) {
            Serial.println("L76K GNSS init succeeded, using L76K GNSS Module\n");
            result = true;
            break;
        }
        delay(500);
    }
    // Initialize the L76K Chip, use GPS + GLONASS
    Serial2.write("$PCAS04,5*1C\r\n");
    delay(250);
    Serial2.write("$PCAS03,1,1,1,1,1,1,1,1,1,1,,,0,0*02\r\n");
    delay(250);
    // Switch to Vehicle Mode, since SoftRF enables Aviation < 2g
    Serial2.write("$PCAS11,3*1E\r\n");

    return result;
}

// ---------------------------------------------------------------------------

void hw_init()
{
    Serial.println("hw_init() in 3 sec");
    delay(3000);

    // discriminate the HW in place: pre-Dec-2024 (no GPS) and Dec-2024 (has GPS)
    // L76K GPS USE 9600 BAUDRATE
    Serial2.begin(9600, SERIAL_8N1, board->_GPS_RXD, board->_GPS_TXD);
    delay(200);
    if (!setupGPS()) {
        Serial2.end();

        Serial.println("failed to detect post-Dec-2024 hardware, switching to pre-Dec-2024");
        board = &board_preDec2024;

        Serial.println("init SPI bus");
        SPI.begin(board->_SPI_SCLK, board->_SPI_MISO, board->_SPI_MOSI);
        Serial.println("init i2c bus");
        Wire.begin(board->_SDA, board->_SCL);
    } else {
        Serial.println("found GPS, must be running on new HW");

        Serial.println("init SPI bus");
        SPI.begin(board->_SPI_SCLK, board->_SPI_MISO, board->_SPI_MOSI);
        Serial.println("init i2c bus");
        Wire.begin(board->_SDA, board->_SCL);
    }

    gpio_hold_dis((gpio_num_t)board->_TOUCH_RST);
    gpio_hold_dis((gpio_num_t)board->_LORA_RST);
    gpio_deep_sleep_hold_dis();

    // lora and sd use the same spi, in order to avoid mutual influence; 
    // before powering on, all CS signals should be pulled high and in an
    // unselected state;
    pinMode(board->_LORA_CS, OUTPUT);
    digitalWrite(board->_LORA_CS, HIGH);
    pinMode(board->_SD_CS, OUTPUT);
    digitalWrite(board->_SD_CS, HIGH);

    if (board->_PCA9535_INT > 0) { // or:    if (board == &board_Dec2024)
        pinMode(board->_PCA9535_INT, INPUT_PULLUP);

        Wire.beginTransmission(0x20); // PCA9555
        if (!Wire.endTransmission())
            Serial.println("successfully detected PCA9555 (post-Dec-2024 hardware)");
    }
    // quickly flash backlight to indicate boot progress
    pinMode(board->_BL_EN, OUTPUT);
    set_backlight(1);
    delay(300);
    set_backlight(0);
        
    // BQ27220 --- 0x55
    Wire.beginTransmission(0x55);
    if (Wire.endTransmission() == 0)
    {
        if (board == &board_Dec2024) { // do not init on old HW, it fails
            int rc = bq27220.init();
            Serial.printf("bq27220.init() returns %d\r\n", rc);
        } else
            Serial.println("skipping bq27220.init()\r\n");

        BQ27220BatteryStatus batt;
        bq27220.getBatteryStatus(&batt);
        Serial.printf("battery is_full=%d\r\n", batt.full);

        Serial.printf("battery  device=%d\r\n", bq27220.getDeviceNumber());
        Serial.printf("battery   input=%d\r\n", bq27220.getIsCharging());
        Serial.printf("battery  finish=%d\r\n", bq27220.getCharingFinish());
        Serial.printf("battery voltage=%d\r\n", bq27220.getVoltage());
        Serial.printf("battery current=%d\r\n", bq27220.getCurrent());
        Serial.printf("battery    temp=%d\r\n", bq27220.getTemperature());
        Serial.printf("battery fullCap=%d\r\n", bq27220.getFullChargeCapacity());
        Serial.printf("battery designC=%d\r\n", bq27220.getDesignCapacity());
        Serial.printf("battery remainC=%d\r\n", bq27220.getRemainingCapacity());
        Serial.printf("battery chargeS=%d\r\n", bq27220.getStateOfCharge());
        Serial.printf("battery healthS=%d\r\n", bq27220.getStateOfHealth());
    }
    
    // BQ25896 --- 0x6B
    Wire.beginTransmission(0x6B);
    if (Wire.endTransmission() == 0)
    {
        // bq25896_is_init = true;
        // battery_25896.begin();
        int rc = PPM.init(Wire, board->_SDA, board->_SCL, BQ25896_SLAVE_ADDRESS);
        Serial.printf("bq25896.init() returns %d\r\n", rc);

        // Set the minimum operating voltage. Below this voltage, the PPM will protect
        PPM.setSysPowerDownVoltage(3300);

        // Set input current limit, default is 500mA
        PPM.setInputCurrentLimit(3250);

        Serial.printf("getInputCurrentLimit: %d mA\r\n",
                      PPM.getInputCurrentLimit());

        // Disable current limit pin
        PPM.disableCurrentLimitPin();

        // Set the charging target voltage, Range:3840 ~ 4608mV ,step:16 mV
        PPM.setChargeTargetVoltage(4208);

        // Set the precharge current , Range: 64mA ~ 1024mA ,step:64mA
        PPM.setPrechargeCurr(64);

        // The premise is that Limit Pin is disabled, or it will only
        // follow the maximum charging current set by Limi tPin.
        // Set the charging current , Range:0~5056mA ,step:64mA
        PPM.setChargerConstantCurr(832);

        // Get the set charging current
        PPM.getChargerConstantCurr();
        Serial.printf("getChargerConstantCurr: %d mA\r\n",
                      PPM.getChargerConstantCurr());

        PPM.enableMeasure();

        PPM.enableCharge();
        Serial.printf("PPM ChargeStatus %s\r\n", PPM.getChargeStatusString());
        Serial.printf("PPM BusStatus    %s\r\n", PPM.getBusStatusString());
        Serial.printf("PPM NTCStatus    %s\r\n", PPM.getNTCStatusString());
        Serial.printf("PPM VBus         %g V\r\n", PPM.getVbusVoltage() *1.0 / 1000.0 );
        Serial.printf("PPM System       %g V\r\n", PPM.getSystemVoltage() * 1.0 / 1000.0);
        Serial.printf("PPM Battery      %g V\r\n", PPM.getBattVoltage() * 1.0 / 1000.0);
        Serial.printf("PPM ChargeTarget %g V\r\n", PPM.getChargeTargetVoltage() * 1.0 / 1000.0);
        Serial.printf("PPM %d mA\r\n", PPM.getChargeCurrent());
        Serial.printf("PPM %d mA\r\n", PPM.getPrechargeCurr());
    }

    // I2C Scan
    /**
     * 0x20 --- PCA9555 -- new board (>= Dec 2024) only
     * 0x51 --- RTC
     * 0x5D --- Touch
     * 0x68 --- TPS651851 ??
     * 0x6B --- BQ25896
     * 0x55 --- BQ27220
    */
    /*
    byte error, address;
    int nDevices = 0;
    for (address = 0x01; address < 0x7F; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) { // 0: success.
            nDevices++;
            Serial.printf("I2C device found at address 0x%x\r\n", address);
        }
    }
    */

    if (board->_I2C_PORT != 255) { // io_extend_lora_gps_power_on(true)
        pca9555_read_input(board->_I2C_PORT, 0);
        pca9555_read_input(board->_I2C_PORT, 1);

        pca9555_read_write(board->_I2C_PORT, 0);
        pca9555_read_inversion(board->_I2C_PORT, 0);
        pca9555_read_config(board->_I2C_PORT, 0);

        pca9555_read_write(board->_I2C_PORT, 1);
        pca9555_read_inversion(board->_I2C_PORT, 1);
        pca9555_read_config(board->_I2C_PORT, 1);

        uint8_t io_val0 = pca9555_read_input(board->_I2C_PORT, 0);
        uint8_t io_val1 = pca9555_read_input(board->_I2C_PORT, 1);
        if ( ~(io_val0 & 0x01) || ~(io_val1 & 0x04) ) {
            Serial.printf("io_extend failed: 0x%02x, 0x%02x\r\n", io_val0, io_val1);
        }
    }

    // SD
    // sd_is_init = SD.begin(board->_SD_CS);

    // LoRA
    lora_hw_init();

    // PCA9535PW --- 0x20
    // ...
 
    // Touch --- 0x5D
    pinMode(board->_RTC_IRQ, INPUT_PULLUP);
    touch.setPins(board->_TOUCH_RST, board->_TOUCH_INT);
    if (touch.begin(Wire, 0x5D, board->_TOUCH_SDA, board->_TOUCH_SCL))
    {
        touch.setMaxCoordinates(EPD_WIDTH, EPD_HEIGHT);
        touch.setSwapXY(false);
        touch.setMirrorXY(false, false);
        touchOnline = true;
        Serial.printf("touch is online\r\n");
        touch.setHomeButtonCallback(home_btn_event, NULL);
    }

    // RTC --- 0x51 
    pinMode(board->_RTC_IRQ, INPUT_PULLUP);
    Wire.beginTransmission(PCF8563_SLAVE_ADDRESS);
    if (Wire.endTransmission() == 0)
    {
        rtc.begin(Wire, PCF8563_SLAVE_ADDRESS, board->_RTC_SDA, board->_RTC_SCL);
        rtc.setDateTime(2022, 6, 30, 0, 0, 0);
        // rtc_is_init = true;
        Serial.printf("rtc init done\r\n");
    }

    /*
    touch.setPins(board->_TOUCH_RST, board->_TOUCH_INT);
    if (!touch.begin(Wire, GT911_SLAVE_ADDRESS_L, board->_SDA, board->_SCL))
        Serial.println("  touch init failed");

    pinMode(board->_RTC_IRQ, INPUT_PULLUP);
    if (!rtc.begin(Wire, PCF8563_SLAVE_ADDRESS, board->_RTC_SDA, board->_RTC_SCL))
        Serial.println("  rtc init failed");

    int result = PPM.init(Wire, board->_SDA, board->_SCL, BQ25896_SLAVE_ADDRESS);
    if (result == false)
        Serial.println("  PPM init failed");

    Serial.printf("bq27220.init() returns %d\r\n", bq27220.init());
    */
    
#if defined(T5S3PRO_OLD_HARDWARE)
    epd_init();
#else
    epd_init(&DEMO_BOARD, &ED047TC2, EPD_LUT_64K);
    epd_set_vcom(1560); // is default value
    // epd_set_vcom(1750); // TPS651851 VCOM output range 0.2-5.1v
    // epd_set_vcom(5100); // TPS651851 VCOM output range 0.2-5.1v
    hl = epd_hl_init(WAVEFORM);
#endif
    epd_packedpixels = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_HEIGHT*(EPD_WIDTH/2 + 1));
    epd_poweron();
    // epd_clear();
    epd_clear_area_cycles(epd_full_screen(), 1, 50);
    // memset(epd_packedpixels, 255, EPD_HEIGHT*EPD_WIDTH/2);
    // epd_draw_grayscale_image(epd_full_screen(), epd_packedpixels);

    // epd_draw_line(0, 0, EPD_WIDTH, EPD_HEIGHT, 0x0f,
    //               epd_hl_get_framebuffer(&hl));
    epd_poweroff();
    

#define EPD_COLOR_TEXT        0x000000
#define EPD_COLOR_BG          0xffffff

    lvgl_setup();
    /*
    lv_disp_t *disp = lv_disp_get_default();
    // disp->theme = lv_theme_mono_init(disp, false, LV_FONT_DEFAULT);
    disp->theme = lv_theme_mono_init(disp, true, LV_FONT_DEFAULT);

    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(EPD_COLOR_BG), LV_PART_MAIN);

    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_height(btn, 50);
    // lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 15, 15);
    lv_obj_center(btn);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, lv_color_hex(EPD_COLOR_BG), LV_PART_MAIN);
    // lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label2 = lv_label_create(btn);
    lv_obj_align(label2, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_color(label2, lv_color_hex(EPD_COLOR_TEXT), LV_PART_MAIN);
    lv_label_set_text(label2,
                      LV_SYMBOL_LEFT " Hello tinySSB " LV_SYMBOL_RIGHT );
    */
    // lv_task_handler();
    
    //     ui_epd47_entry();
    // disp_manual_refr(500);

    // pinMode(board->_BL_EN, OUTPUT);
    // set_backlight(0);

    pinMode(board->_BOOT_BTN, INPUT);
    // pinMode(board->_KEY_BTN, INPUT);

    pinMode(board->_TOUCH_RST, OUTPUT);
    pinMode(board->_LORA_RST, OUTPUT);

    /*
    while (1) {
        delay(1); // delay(1000);
        lv_task_handler();
        // Serial.println("loop()");
    }
    */
}

static void shutdown_cb(lv_timer_t *t)
{
    if (t != NULL)
        lv_timer_del(t);

#if !defined(T5S3PRO_OLD_HARDWARE)
    // EpdRect area = epd_full_screen();
    /*
    {
        .x = 0,
        .y = 0,
        .width = EPD_HEIGHT,
        .height = EPD_WIDTH
        };
    */
    /*
    epd_poweron();

    epd_clear_area_cycles(area, 3, 50);
    epd_draw_grayscale_image(area, epd_packedpixels);

    epd_poweroff();    
    */
#endif

    digitalWrite(board->_TOUCH_RST, LOW); 
    digitalWrite(board->_LORA_RST, LOW); 
    
    gpio_hold_en((gpio_num_t)board->_TOUCH_RST);
    gpio_hold_en((gpio_num_t)board->_LORA_RST);
    gpio_deep_sleep_hold_en();

    epd_poweroff();

    PPM.shutdown();
}

void hw_shutdown()
{
    Serial.println("shutting down now ..");

#if !defined(T5S3PRO_OLD_HARDWARE)
    // epd_set_vcom(5000);
#endif

    // epd_clear();
    // epd_hl_set_all_white(&hl);
    // epd_hl_update_area(&hl, MODE_DU, epd_ambient_temperature(),
    //                    epd_full_screen());
    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &img_sleeping);
    lv_refr_now(NULL);
 
    lv_timer_create(shutdown_cb, 500 /*EPD_REFRESH_TIME+500*/, NULL);
}

#endif // TINYSSB_BOARD_T5S3PRO

// eof
