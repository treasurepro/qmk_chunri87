// Copyright 2023 JoyLee (@itarze)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "usb_main.h"
#include "immobile.h"
#include "config.h"

enum layers {
    WIN_BASE = 0,
    WIN_FN,
    MAC_BASE,
    MAC_FN
};

#define LAY_WBS TO(WIN_BASE)
#define LAY_WFN MO(WIN_FN)
#define LAY_MBS TO(MAC_BASE)
#define LAY_MFN MO(MAC_FN)

uint8_t Fn_Key_Press_flag = 0;
uint8_t RL_Togg_flag = 0;
uint8_t usb_led_flag = 0;

extern uint16_t qbat_send;
extern uint8_t qbat_save;
enum user_keys {
    OS_SW = EX_USER,
    OS_CLR,
};

#define RGB_HSV_MAX 7
static uint8_t rgb_hsvs[7][3] = {
    {HSV_RED},
    {HSV_YELLOW},
    {HSV_GREEN},
    {HSV_CYAN},
    {HSV_BLUE},
    {HSV_PINK},
    {HSV_WHITE},
};
static uint8_t rgb_light[9][3] = {
    {0,0,0},
    {0,255,0},
    {64,128,0},
    {128,255,0},
    {255,0,0},
    {255,0,255},
    {0,0,255},
    {0,128,255},        
    {255,255,255},     
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [WIN_BASE] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_CALC, KC_PSCR, KC_SCRL, KC_PAUSE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUHS, KC_ENT,
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,          KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                    KC_RALT, LAY_WFN, KC_APP,  KC_RCTL,                   KC_LEFT, KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT( /* Base */
        OS_CLR,  KC_BRID, KC_BRIU, LGUI(KC_TAB), KC_MYCM, KC_MAIL, KC_WHOM, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU,_______,_______, _______,  RL_TOG,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, IM_BATQ, _______, RGB_HUI,  RGB_SAI,
        OS_SW,   IM_BT1,  IM_BT2,  IM_BT3,  IM_2G4,  IM_USB,  _______, _______, _______, _______, _______, RL_VAI,  RL_VAD,  _______, RGB_MOD, RGB_TOG,  RGB_SAD,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          RGB_VAI,
        RL_MOD,  GU_TOGG, RL_HUI,                    _______,                            _______, _______, _______, _______,          RGB_SPD, RGB_VAD,  RGB_SPI
        ),

    [MAC_BASE] = LAYOUT(  /* Base */
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_CALC, KC_PSCR, KC_SCRL, KC_PAUSE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUHS, KC_ENT,
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,          KC_UP,
        KC_LCTL, KC_LALT, KC_LGUI,                   KC_SPC,                    KC_RGUI, LAY_MFN, KC_RALT, KC_RCTL,                   KC_LEFT, KC_DOWN,  KC_RGHT
        ),

    [MAC_FN] = LAYOUT(  /* FN */
        OS_CLR,  KC_BRID, KC_BRIU, KC_MCTL, KC_CPNL, _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU,_______,_______, _______,  RL_TOG,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, IM_BATQ, _______, RGB_HUI,  RGB_SAI,
        OS_SW,   IM_BT1,  IM_BT2,  IM_BT3,  IM_2G4,  IM_USB,  _______, _______, _______, _______, _______, RL_VAI,  RL_VAD, _______, RGB_MOD, RGB_TOG,  RGB_SAD,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          RGB_VAI,
        RL_MOD,  RL_HUI,  _______,                   _______,                            _______, _______, _______, _______,          RGB_SPD, RGB_VAD,  RGB_SPI
        )
};

// clang-format on
#ifdef RGB_MATRIX_BLINK_ENABLE
void bat_indicators_hook(uint8_t index);

blink_rgb_t blink_rgbs[RGB_MATRIX_BLINK_COUNT] = {
    {.index = IM_MM_RGB_BLINK_INDEX_BT1, .interval = 250, .times = 1, .color = {.r = 0x00, .g = 0xFF, .b = 0x00}, .blink_cb = im_mm_rgb_blink_cb},
    {.index = IM_MM_RGB_BLINK_INDEX_BT2, .interval = 250, .times = 1, .color = {.r = 0x00, .g = 0xFF, .b = 0x00}, .blink_cb = im_mm_rgb_blink_cb},
    {.index = IM_MM_RGB_BLINK_INDEX_BT3, .interval = 250, .times = 1, .color = {.r = 0x00, .g = 0xFF, .b = 0x00}, .blink_cb = im_mm_rgb_blink_cb},
    {.index = IM_MM_RGB_BLINK_INDEX_2G4, .interval = 250, .times = 1, .color = {.r = 0x00, .g = 0xFF, .b = 0x00}, .blink_cb = im_mm_rgb_blink_cb},
    //{.index = IM_MM_RGB_BLINK_INDEX_USB, .interval = 250, .times = 1, .color = {.r = 0x00, .g = 0xFF, .b = 0x00}, .blink_cb = im_mm_rgb_blink_cb},
    {.index = RGB_MATRIX_BLINK_INDEX_BAT, .interval = 250, .times = 3, .color = {.r = 100, .g = 100, .b = 100}, .blink_cb = bat_indicators_hook},
    {.index = RGB_MATRIX_BLINK_INDEX_ALL, .interval = 250, .times = 3, .color = {.r = 100, .g = 100, .b = 100}, .blink_cb = NULL},
    {.index = 41, .interval = 250, .times = 3, .color = {.r = 100, .g = 100, .b = 100}, .blink_cb = NULL},
};
#endif

#ifndef MATRIX_LKEY_DISABLE
im_lkey_t lkey_define_user[IM_LKEY_COUNT] = {
    {.keycode = OS_SW, .hole_time = 3000}, // 参数：按键值，长按时间
    {.keycode = OS_CLR, .hole_time = 3000}, // 参数：按键值，长按时间
};
#endif

typedef enum {
    BAT_NORMAL,
    BAT_LOW,
    BAT_CHRGING,
    BAT_FULL,
} bat_statue_t;
bat_statue_t bat_statue = BAT_NORMAL;

static bool bat_blink = false;
static uint8_t battery_full_flag = 1;
static uint8_t battery_chrg_flag = 1;
static bool full_flag = false;
static uint16_t laste_time_off = 0;

#ifdef RGB_MATRIX_BLINK_INDEX_BAT

void bat_indicators_hook(uint8_t index) {

    if (mm_eeconfig.devs != DEVS_USB) {

        if ((!mm_eeconfig.charging)&& (qbat_save <= BATTERY_CAPACITY_LOW)) {
            /* 低电提醒 */          
            rgb_matrix_blink_set_color(RGB_MATRIX_BLINK_INDEX_BAT, RGB_GREEN);
            rgb_matrix_blink_set_interval_times(index, 500, 0x3);
            bat_blink = true;
            bat_statue = BAT_LOW;
        } else {
            bat_blink = false;
        }
        if ((qbat_save < 1U) && (!mm_eeconfig.charging)) {
            if (laste_time_off == 0) laste_time_off = timer_read();
            if (timer_elapsed(laste_time_off)>10000) {
                laste_time_off=0;
                im_set_power_off();
            }
        } else {
            laste_time_off = 0;
        }
    } else {
        bat_blink = false;
    }

    rgb_matrix_blink_set(index);
}

bool rgb_matrix_blink_user(blink_rgb_t *blink_rgb) {
    if (blink_rgb->index == RGB_MATRIX_BLINK_INDEX_BAT) {
        if (bat_blink != true) {
            return false;
        }
    }
    return true;
}

#endif

typedef union {
    uint32_t raw;
    struct {
        uint8_t flag: 1;
        uint8_t rgb_enable: 1;  /* 主背光（键面 RGB Matrix 效果）开关 */
        uint8_t no_gui: 1;
        uint8_t layer: 1;      /* 复用：Logo 灯强制关闭（RL_TOG），1=关 */
        uint8_t rgb_index: 4;
        uint8_t os_mode: 2;
        uint8_t hui_index: 3;
        uint8_t logo_sat;       /* Logo 饱和度 0~255，VIA 可调 */
    };
} confinfo_t;
confinfo_t confinfo;

#ifdef RGB_MATRIX_ENABLE

/** Logo 侧灯处于“点亮”状态：未强制关闭，且 rgblight 开启或已选预设色 */
static bool chunri_logo_is_active(void) {
    if (confinfo.layer) {
        return false;
    }
#    ifdef RGBLIGHT_ENABLE
    if (rgblight_is_enabled()) {
        return true;
    }
#    endif
    return confinfo.rgb_index != 0;
}

/** 主背光或 Logo 任一需要亮则保持灯驱与 rgb_matrix 任务运行 */
static bool chunri_rgb_hw_needed(void) {
    return confinfo.rgb_enable || chunri_logo_is_active();
}

static void chunri_rgb_sync_hw(void) {
#    ifndef RGB_DRIVER_EN_STATE
#        define RGB_DRIVER_EN_STATE 1
#    endif
#    ifdef RGB_DRIVER_EN_PIN
    if (chunri_rgb_hw_needed()) {
        writePin(RGB_DRIVER_EN_PIN, RGB_DRIVER_EN_STATE);
#        ifdef RGB_DRIVER_EN2_PIN
        writePin(RGB_DRIVER_EN2_PIN, RGB_DRIVER_EN_STATE);
#        endif
        if (!rgb_matrix_is_enabled()) {
            rgb_matrix_enable_noeeprom();
        }
    } else {
        rgb_matrix_disable_noeeprom();
        writePin(RGB_DRIVER_EN_PIN, !RGB_DRIVER_EN_STATE);
#        ifdef RGB_DRIVER_EN2_PIN
        writePin(RGB_DRIVER_EN2_PIN, !RGB_DRIVER_EN_STATE);
#        endif
    }
#    else
    if (chunri_rgb_hw_needed()) {
        if (!rgb_matrix_is_enabled()) {
            rgb_matrix_enable_noeeprom();
        }
    } else {
        rgb_matrix_disable_noeeprom();
    }
#    endif
}

bool mm_get_rgb_enable(void) {
#    ifdef RGBLIGHT_ENABLE
    return confinfo.rgb_enable;
#    else
    return rgb_matrix_config.enable;
#    endif
}

// 此函数不需要改动
void mm_set_rgb_enable(bool state) {
#    ifdef RGBLIGHT_ENABLE
    confinfo.rgb_enable = state;
    eeconfig_update_user(confinfo.raw);
#    else
    rgb_matrix_config.enable = state;
#    endif
}

#endif

void eeconfig_confinfo_default(void) {
    confinfo.flag = false;
    confinfo.rgb_enable = true;
    confinfo.no_gui = false;
    confinfo.layer = false;
    confinfo.rgb_index = 0;
    confinfo.logo_sat = 255;
    confinfo.os_mode = WIN_BASE;
    eeconfig_update_user(confinfo.raw);
}
bool im_led_deinit_user(void) {

    writePin(RGB_DRIVER_EN_PIN, 0);

    writePin(POWER_DCDC_EN_PIN, 0);

    return true;
}

bool im_led_init_user() {

    writePin(POWER_DCDC_EN_PIN, 1);

    writePin(RGB_DRIVER_EN_PIN, 1);
    return true;
}

// 初始化一些GPIO PIN 相关的操作
bool im_pre_init_user(void) {
    setPinInput(CHRG_PIN);
    setPinOutput(POWER_DCDC_EN_PIN);
    writePin(POWER_DCDC_EN_PIN, 1);

    return true;
}

static uint32_t readbat = 0x00;

// 初始化和参数相关的操作，在恢复出厂设置时此函数会被调用
bool im_init_user(void) {
    setPinOutput(RGB_DRIVER_EN_PIN);
    writePin(RGB_DRIVER_EN_PIN, 1);
    setPinInputHigh(CHRG_PIN);
    setPinInputHigh(FULL_PIN);
    confinfo.raw = eeconfig_read_user();
    if (!confinfo.raw) {
        eeconfig_confinfo_default();
    }
    RL_Togg_flag = confinfo.layer;
#    ifdef RGBLIGHT_ENABLE
    if (!confinfo.layer && !rgblight_is_enabled()) {
        rgblight_enable_noeeprom();
        rgblight_mode_noeeprom(RGBLIGHT_DEFAULT_MODE);
    }
#    endif
    chunri_rgb_sync_hw();
    readbat = timer_read32();

#ifdef RGB_MATRIX_BLINK_INDEX_BAT
     rgb_matrix_blink_set(RGB_MATRIX_BLINK_INDEX_BAT);
#endif
    return true;
}

// 恢复出厂设置回调函数
bool im_reset_settings_user(void) {


    rgb_matrix_blink_set_color(RGB_MATRIX_BLINK_INDEX_ALL, 0x0, RGB_MATRIX_MAXIMUM_BRIGHTNESS, 0x0);
    rgb_matrix_blink_set_interval_times(RGB_MATRIX_BLINK_INDEX_ALL, 500, 3);
    rgb_matrix_blink_set(RGB_MATRIX_BLINK_INDEX_ALL);

    return true;
}
bool chrg_flag = false;
// 无限循环
bool im_loop_user(void) {

    if (timer_elapsed32(readbat) >= 3000) {
        readbat = timer_read32();
        battery_chrg_flag = readPin(CHRG_PIN);
        battery_full_flag = readPin(FULL_PIN);

    }

    if ((!mm_eeconfig.charging) && full_flag ) {
        full_flag = false;
    }

    if ((chrg_flag) && (!battery_full_flag)) {
        if (bat_statue != BAT_FULL) bts_send_vendor(v_bat_full);
        full_flag = true;
        bat_statue = BAT_FULL;
    }

    if ((!mm_eeconfig.charging) && (bat_statue != BAT_LOW)) {
        if (bat_statue != BAT_NORMAL) bts_send_vendor(v_bat_stop_charging);
        bat_statue = BAT_NORMAL;
    }

    return true;
}

bool im_process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LAY_WFN:
        case LAY_MFN:
        {
            if (record->event.pressed)
            {
                Fn_Key_Press_flag = 1;
            }
            else
            {
                Fn_Key_Press_flag = 0;
            }
            return true;
        }
        break;
        case US_TS1: { // 低频
            if (record->event.pressed) {
                bts_send_vendor(0x60);
                bts_rf_send_carrier(0, 5, 0x01);
            } break;
            return false;
        } break;
        case US_TS2: { // 中频
            if (record->event.pressed) {
                bts_send_vendor(0x60);
                bts_rf_send_carrier(19, 5, 0x01);
            } break;
            return false;
        } break;
        case US_TS3: { // 高频
            if (record->event.pressed) {
                bts_send_vendor(0x60);
                bts_rf_send_carrier(38, 5, 0x01);
            } break;
            return false;
        } break;
        case US_STOP: { // 停止
            if (record->event.pressed) {
                bts_rf_send_stop();
            } break;
            return false;
        } break;
        case IM_USB: {
            if(record->event.pressed){
                usb_led_flag  = 1;
            }
            else{
                usb_led_flag  = 0;
            }
            return true;
        }break;
        case GU_TOGG: {
            if(record->event.pressed){
                confinfo.no_gui = !confinfo.no_gui;
                eeconfig_update_user(confinfo.raw);
            }
            return true;
        }break;
        case OS_SW: {
            return false;
        }
        case OS_CLR:
            return false;
            break;
        case RGB_TOG: {
            /* 拦截 multimode 默认逻辑，避免关主背光时一并 rgb_matrix_disable */
#    ifdef RGB_TRIGGER_ON_KEYDOWN
            if (record->event.pressed) {
#    else
            if (!record->event.pressed) {
#    endif
                mm_set_rgb_enable(!mm_get_rgb_enable());
                chunri_rgb_sync_hw();
            }
            return false;
        } break;
        case RGB_HUI:{
            if (record->event.pressed) {
                confinfo.hui_index = (confinfo.hui_index + 1) % RGB_HSV_MAX;
                rgb_matrix_sethsv(rgb_hsvs[confinfo.hui_index][0],
                                rgb_hsvs[confinfo.hui_index][1],
                                rgb_matrix_get_val());
                eeconfig_update_user(confinfo.raw);
                chunri_rgb_sync_hw();
            }
            return false;
        } break;
        case RL_TOG: {
            if (record->event.pressed) {
                confinfo.layer = !confinfo.layer;
                RL_Togg_flag   = confinfo.layer;
                eeconfig_update_user(confinfo.raw);
                chunri_rgb_sync_hw();
            }
            return false;
        } break;
        case RL_HUI: {         
            if ((record->event.pressed) && !confinfo.layer) {
                confinfo.rgb_index++;
                if (confinfo.rgb_index >= 9) {
                    confinfo.rgb_index = 1;
                }
                eeconfig_update_user(confinfo.raw);
                chunri_rgb_sync_hw();
            }
            return false;
        } break;
        case RL_MOD: {
             if ((record->event.pressed) && !confinfo.layer) {
                rgblight_increase();
                confinfo.rgb_index = 0;
                eeconfig_update_user(confinfo.raw);
                chunri_rgb_sync_hw();
            }
            return false;
        } break;
         
    case IM_BATQ:{
        return (mm_eeconfig.devs != DEVS_USB);
    }break;
        default:
            break;
    }
    return true;
}
bool im_lkey_process_user(uint16_t keycode, bool pressed) {

    switch (keycode) {
        case OS_SW: {
            if (pressed) {
                if (confinfo.os_mode == WIN_BASE) {
                    set_single_persistent_default_layer(MAC_BASE);
                    confinfo.os_mode = MAC_BASE;
                    rgb_matrix_blink_set_color(41, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
                    rgb_matrix_blink_set_interval_times(41, 250, 3);
                    rgb_matrix_blink_set(41);
                } else {
                    set_single_persistent_default_layer(WIN_BASE);
                    confinfo.os_mode = WIN_BASE;
                    rgb_matrix_blink_set_color(41, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
                    rgb_matrix_blink_set_interval_times(41, 250, 3);
                    rgb_matrix_blink_set(41);
                }
                eeconfig_update_user(confinfo.raw);
            }
            return false;
        } break;
        case OS_CLR: {
            if (pressed) {
                im_reset_settings();
            }
            return false;
        } break;
    }
    return true;
}

#ifdef RGB_MATRIX_ENABLE

#ifndef RGB_MATRIX_BAT_VAL
#    define RGB_MATRIX_BAT_VAL RGB_MATRIX_MAXIMUM_BRIGHTNESS
#endif

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {

    if(confinfo.no_gui == 1 )
    {
        rgb_matrix_set_color(2, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    }

    if(usb_led_flag == 1 )
    {
        rgb_matrix_set_color(46, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    }

 
    if (confinfo.layer) {
        for (uint8_t i = 0; i < 13; i++) {
            rgb_matrix_set_color(i + RGB_MATRIX_LOGO_LED, 0, 0, 0);
        }
#    ifdef RGBLIGHT_ENABLE
    } else if (!rgblight_is_enabled() && (confinfo.rgb_index != 0)) {
        /* Fn 层 RL_HUI 预设色（未用 VIA/rgblight 时） */
        const uint8_t val = rgblight_get_val();
        const uint8_t sat = rgblight_get_sat();
        const uint8_t idx = confinfo.rgb_index;
        for (uint8_t i = 0; i < 13; i++) {
            uint8_t r = (uint16_t)rgb_light[idx][0] * val / RGBLIGHT_LIMIT_VAL * sat / 255;
            uint8_t g = (uint16_t)rgb_light[idx][1] * val / RGBLIGHT_LIMIT_VAL * sat / 255;
            uint8_t b = (uint16_t)rgb_light[idx][2] * val / RGBLIGHT_LIMIT_VAL * sat / 255;
            rgb_matrix_set_color(i + RGB_MATRIX_LOGO_LED, r, g, b);
        }
#    endif
    }       

    // if(Fn_Key_Press_flag == 1)
    // {
    //     // if(confinfo.os_mode == MAC_BASE)
    //     // {
    //     //     rgb_matrix_set_color(11, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    //     // }
    //     // else
    //     // {
    //     //     rgb_matrix_set_color(10, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    //     // }

    //     if((mm_eeconfig.devs ==  DEVS_USB))
    //     {
    //         rgb_matrix_set_color(42, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    //     }
    //     else if((mm_eeconfig.devs ==  DEVS_BT1))
    //     {
    //         rgb_matrix_set_color(IM_MM_RGB_BLINK_INDEX_BT1, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    //     }
    //     else if((mm_eeconfig.devs ==  DEVS_BT2))
    //     {
    //         rgb_matrix_set_color(IM_MM_RGB_BLINK_INDEX_BT2, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    //     }
    //     else if((mm_eeconfig.devs ==  DEVS_BT3))
    //     {
    //         rgb_matrix_set_color(IM_MM_RGB_BLINK_INDEX_BT3, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    //     }
    //     else if((mm_eeconfig.devs ==  DEVS_2G4))
    //     {
    //         rgb_matrix_set_color(IM_MM_RGB_BLINK_INDEX_2G4, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    //     }

    // }
    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(IM_MM_INDICATOR_CAPS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    }
    
    if (host_keyboard_led_state().scroll_lock) {
        rgb_matrix_set_color(IM_MM_INDICATOR_SCROLL, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    }


    // if ((!battery_chrg_flag) && (!full_flag)) {
    //     if (bat_statue != BAT_CHRGING) bts_send_vendor(v_bat_charging);
    //     rgb_matrix_set_color(RGB_MATRIX_BLINK_INDEX_BAT, RGB_BLUE);
    //     chrg_flag = true;
    //     bat_statue = BAT_CHRGING;
    // }


    if ((bat_blink) && (battery_chrg_flag)) {
        for (uint8_t i = 0; i < RGB_MATRIX_LOGO_LED; i++) {
            rgb_matrix_set_color(i, 0, 0, 0);
        }
        if (!chunri_logo_is_active()) {
            for (uint8_t i = 0; i < 13; i++) {
                rgb_matrix_set_color(i + RGB_MATRIX_LOGO_LED, 0, 0, 0);
            }
        }
    }
      
      


    return true;
}
#endif
