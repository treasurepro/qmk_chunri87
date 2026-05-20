// Copyright 2026
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"

#ifdef VIA_ENABLE

#    include "via.h"

enum chunri_via_value_id {
    id_chunri_matrix_sat = 0x10,
    id_chunri_logo_sat   = 0x11,
};

typedef union {
    uint32_t raw;
    struct {
        uint8_t flag : 1;
        uint8_t rgb_enable : 1;
        uint8_t no_gui : 1;
        uint8_t layer : 1;
        uint8_t rgb_index : 4;
        uint8_t os_mode : 2;
        uint8_t hui_index : 3;
        uint8_t logo_sat;
    };
} confinfo_t;

extern confinfo_t confinfo;

bool via_extra_custom_value_command(uint8_t *data, uint8_t length) {
    (void)length;

    uint8_t *command_id = &(data[0]);
    uint8_t *channel_id = &(data[1]);
    uint8_t *value_id   = &(data[2]);
    uint8_t *value_data = &(data[3]);

    if (*channel_id != id_custom_channel) {
        return false;
    }

    switch (*value_id) {
        case id_chunri_matrix_sat:
        case id_chunri_logo_sat: {
            switch (*command_id) {
                case id_custom_get_value: {
                    if (*value_id == id_chunri_matrix_sat) {
#    if defined(RGB_MATRIX_ENABLE)
                        value_data[0] = rgb_matrix_get_sat();
#    else
                        value_data[0] = 0;
#    endif
                    } else {
#    if defined(RGBLIGHT_ENABLE)
                        value_data[0] = rgblight_get_sat();
#    else
                        value_data[0] = confinfo.logo_sat;
#    endif
                    }
                    return true;
                }
                case id_custom_set_value: {
                    if (*value_id == id_chunri_matrix_sat) {
#    if defined(RGB_MATRIX_ENABLE)
                        rgb_matrix_sethsv_noeeprom(rgb_matrix_get_hue(), value_data[0], rgb_matrix_get_val());
#    endif
                    } else {
#    if defined(RGBLIGHT_ENABLE)
                        rgblight_sethsv_noeeprom(rgblight_get_hue(), value_data[0], rgblight_get_val());
#    else
                        confinfo.logo_sat = value_data[0];
#    endif
                    }
                    return true;
                }
                case id_custom_save: {
                    if (*value_id == id_chunri_matrix_sat) {
#    if defined(RGB_MATRIX_ENABLE)
                        eeconfig_update_rgb_matrix();
#    endif
                    } else {
#    if defined(RGBLIGHT_ENABLE)
                        eeconfig_update_rgblight_current();
#    else
                        eeconfig_update_user(confinfo.raw);
#    endif
                    }
                    return true;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    return false;
}

#endif
