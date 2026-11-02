/**
 * Flipper Toolbox - Electronics Calculator Module
 */

#ifndef ELECTRONICS_CALCULATOR_H
#define ELECTRONICS_CALCULATOR_H

#include "shared.h"

typedef enum {
    CALC_OHM_LAW_V = 0,
    CALC_OHM_LAW_I = 1,
    CALC_OHM_LAW_R = 2,
    CALC_POWER = 3,
    CALC_SERIES_R = 4,
    CALC_PARALLEL_R = 5,
    CALC_LED_RESISTOR = 6,
    CALC_VOLTAGE_DIVIDER = 7,
    CALC_CAPACITOR_CONV = 8,
    CALC_FREQ_PERIOD = 9,
    CALC_DUTY_CYCLE = 10,
} CalcType;

typedef struct {
    CalcType type;
    double input1;
    double input2;
    double input3;
    double result;
    char result_str[64];
    char unit[16];
} ElectronicsCalc;

ElectronicsCalc electronics_calc_ohm_law_v(double i, double r);
ElectronicsCalc electronics_calc_ohm_law_i(double v, double r);
ElectronicsCalc electronics_calc_ohm_law_r(double v, double i);
ElectronicsCalc electronics_calc_power(double v, double i);
ElectronicsCalc electronics_calc_series_r(double r1, double r2);
ElectronicsCalc electronics_calc_parallel_r(double r1, double r2);
ElectronicsCalc electronics_calc_led_resistor(double v_supply, double v_forward, double i_target);
ElectronicsCalc electronics_calc_voltage_divider(double v_in, double r1, double r2);
ElectronicsCalc electronics_calc_capacitor_conv(double value, int from_unit, int to_unit);
ElectronicsCalc electronics_calc_freq_period(double freq);
ElectronicsCalc electronics_calc_duty_cycle(double on_time, double period);

void electronics_calculator_start(ViewDispatcher* view_dispatcher);

#endif // ELECTRONICS_CALCULATOR_H