/**
 * Flipper Toolbox - Electronics Calculator Module Implementation
 */

#include "electronics_calculator.h"
#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <math.h>

#define TAG "ElecCalc"

typedef struct {
    VariableItemList* var_list;
    ViewDispatcher* view_dispatcher;
    Widget* result_widget;
    CalcType current_type;
    double input1, input2, input3;
} ElecCalcView;

static const char* calc_types[] = {
    "Ohm's Law: V = I * R",
    "Ohm's Law: I = V / R",
    "Ohm's Law: R = V / I",
    "Power: P = V * I",
    "Series R: R = R1 + R2",
    "Parallel R: 1/R = 1/R1 + 1/R2",
    "LED Resistor",
    "Voltage Divider",
    "Capacitor Conversion",
    "Frequency <-> Period",
    "Duty Cycle",
};

static bool elec_calc_nav_callback(void* context) {
    ElecCalcView* view = context;
    view_dispatcher_stop(view->view_dispatcher);
    variable_item_list_free(view->var_list);
    widget_free(view->result_widget);
    free(view);
    return true;
}

static void elec_calc_type_changed(VariableItem* item) {
    ElecCalcView* view = variable_item_get_context(item);
    view->current_type = (CalcType)variable_item_get_current_value_index(item);
}

static void elec_calc_calculate_callback(VariableItem* item) {
    ElecCalcView* view = variable_item_get_context(item);
    ElectronicsCalc calc = {0};
    char buffer[512];
    
    switch(view->current_type) {
        case CALC_OHM_LAW_V:
            calc = electronics_calc_ohm_law_v(view->input1, view->input2);
            snprintf(buffer, sizeof(buffer), "V = I x R\nI = %.4g A\nR = %.4g Ohm\n\nResult: %s", view->input1, view->input2, calc.result_str);
            break;
        case CALC_OHM_LAW_I:
            calc = electronics_calc_ohm_law_i(view->input1, view->input2);
            snprintf(buffer, sizeof(buffer), "I = V / R\nV = %.4g V\nR = %.4g Ohm\n\nResult: %s", view->input1, view->input2, calc.result_str);
            break;
        case CALC_OHM_LAW_R:
            calc = electronics_calc_ohm_law_r(view->input1, view->input2);
            snprintf(buffer, sizeof(buffer), "R = V / I\nV = %.4g V\nI = %.4g A\n\nResult: %s", view->input1, view->input2, calc.result_str);
            break;
        case CALC_POWER:
            calc = electronics_calc_power(view->input1, view->input2);
            snprintf(buffer, sizeof(buffer), "P = V x I\nV = %.4g V\nI = %.4g A\n\nResult: %s", view->input1, view->input2, calc.result_str);
            break;
        case CALC_SERIES_R:
            calc = electronics_calc_series_r(view->input1, view->input2);
            snprintf(buffer, sizeof(buffer), "R = R1 + R2\nR1 = %.4g Ohm\nR2 = %.4g Ohm\n\nResult: %s", view->input1, view->input2, calc.result_str);
            break;
        case CALC_PARALLEL_R:
            calc = electronics_calc_parallel_r(view->input1, view->input2);
            snprintf(buffer, sizeof(buffer), "1/R = 1/R1 + 1/R2\nR1 = %.4g Ohm\nR2 = %.4g Ohm\n\nResult: %s", view->input1, view->input2, calc.result_str);
            break;
        case CALC_LED_RESISTOR:
            calc = electronics_calc_led_resistor(view->input1, view->input2, view->input3);
            snprintf(buffer, sizeof(buffer), "R = (Vsupply - Vf) / I\nVsupply = %.4g V\nVf = %.4g V\nI = %.4g A\n\nResult: %s", view->input1, view->input2, view->input3, calc.result_str);
            break;
        case CALC_VOLTAGE_DIVIDER:
            calc = electronics_calc_voltage_divider(view->input1, view->input2, view->input3);
            snprintf(buffer, sizeof(buffer), "Vout = Vin * R2/(R1+R2)\nVin = %.4g V\nR1 = %.4g Ohm\nR2 = %.4g Ohm\n\nResult: %s", view->input1, view->input2, view->input3, calc.result_str);
            break;
        case CALC_CAPACITOR_CONV:
            calc = electronics_calc_capacitor_conv(view->input1, (int)view->input2, (int)view->input3);
            snprintf(buffer, sizeof(buffer), "Capacitor Conversion\nValue = %.4g\nFrom: %d To: %d\n\nResult: %s", view->input1, (int)view->input2, (int)view->input3, calc.result_str);
            break;
        case CALC_FREQ_PERIOD:
            calc = electronics_calc_freq_period(view->input1);
            snprintf(buffer, sizeof(buffer), "T = 1 / f\nf = %.4g Hz\n\nResult: %s", view->input1, calc.result_str);
            break;
        case CALC_DUTY_CYCLE:
            calc = electronics_calc_duty_cycle(view->input1, view->input2);
            snprintf(buffer, sizeof(buffer), "Duty = Ton / T * 100\nTon = %.4g\nT = %.4g\n\nResult: %s", view->input1, view->input2, calc.result_str);
            break;
    }
    
    widget_reset(view->result_widget);
    widget_add_text_box_element(view->result_widget, 4, 4, 120, 56, AlignLeft, AlignTop, buffer, false);
    view_dispatcher_switch_to_view(view->view_dispatcher, 1);
}

static void result_back_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    ElecCalcView* view = context;
    view_dispatcher_switch_to_view(view->view_dispatcher, 0);
}

ElectronicsCalc electronics_calc_ohm_law_v(double i, double r) {
    ElectronicsCalc calc = {CALC_OHM_LAW_V, i, r, 0, i * r, "", "V"};
    snprintf(calc.result_str, sizeof(calc.result_str), "%.4g V", calc.result);
    return calc;
}

ElectronicsCalc electronics_calc_ohm_law_i(double v, double r) {
    ElectronicsCalc calc = {CALC_OHM_LAW_I, v, r, 0, 0, "", "A"};
    if(r != 0) {
        calc.result = v / r;
        snprintf(calc.result_str, sizeof(calc.result_str), "%.4g A", calc.result);
    } else {
        strcpy(calc.result_str, "Error: Div by 0");
    }
    return calc;
}

ElectronicsCalc electronics_calc_ohm_law_r(double v, double i) {
    ElectronicsCalc calc = {CALC_OHM_LAW_R, v, i, 0, 0, "", "Ohm"};
    if(i != 0) {
        calc.result = v / i;
        snprintf(calc.result_str, sizeof(calc.result_str), "%.4g Ohm", calc.result);
    } else {
        strcpy(calc.result_str, "Error: Div by 0");
    }
    return calc;
}

ElectronicsCalc electronics_calc_power(double v, double i) {
    ElectronicsCalc calc = {CALC_POWER, v, i, 0, v * i, "", "W"};
    snprintf(calc.result_str, sizeof(calc.result_str), "%.4g W", calc.result);
    return calc;
}

ElectronicsCalc electronics_calc_series_r(double r1, double r2) {
    ElectronicsCalc calc = {CALC_SERIES_R, r1, r2, 0, r1 + r2, "", "Ohm"};
    snprintf(calc.result_str, sizeof(calc.result_str), "%.4g Ohm", calc.result);
    return calc;
}

ElectronicsCalc electronics_calc_parallel_r(double r1, double r2) {
    ElectronicsCalc calc = {CALC_PARALLEL_R, r1, r2, 0, 0, "", "Ohm"};
    if(r1 != 0 && r2 != 0) {
        calc.result = (double)1.0 / ((double)1.0/r1 + (double)1.0/r2);
        snprintf(calc.result_str, sizeof(calc.result_str), "%.4g Ohm", calc.result);
    } else {
        strcpy(calc.result_str, "Error: R cannot be 0");
    }
    return calc;
}

ElectronicsCalc electronics_calc_led_resistor(double v_supply, double v_forward, double i_target) {
    ElectronicsCalc calc = {CALC_LED_RESISTOR, v_supply, v_forward, i_target, 0, "", "Ohm"};
    if(i_target != 0 && v_supply > v_forward) {
        calc.result = (v_supply - v_forward) / i_target;
        snprintf(calc.result_str, sizeof(calc.result_str), "%.4g Ohm (%.2g W)", calc.result, (v_supply - v_forward) * i_target);
    } else {
        strcpy(calc.result_str, "Error: Invalid values");
    }
    return calc;
}

ElectronicsCalc electronics_calc_voltage_divider(double v_in, double r1, double r2) {
    ElectronicsCalc calc = {CALC_VOLTAGE_DIVIDER, v_in, r1, r2, 0, "", "V"};
    if(r1 + r2 != 0) {
        calc.result = v_in * (r2 / (r1 + r2));
        snprintf(calc.result_str, sizeof(calc.result_str), "%.4g V", calc.result);
    } else {
        strcpy(calc.result_str, "Error: Div by 0");
    }
    return calc;
}

ElectronicsCalc electronics_calc_capacitor_conv(double value, int from_unit, int to_unit) {
    static const double units[] = {1e-12, 1e-9, 1e-6, 1e-3, 1.0};
    static const char* unit_names[] = {"pF", "nF", "uF", "mF", "F"};
    
    ElectronicsCalc calc = {CALC_CAPACITOR_CONV, value, from_unit, to_unit, 0, "", ""};
    if(from_unit >= 0 && from_unit < 5 && to_unit >= 0 && to_unit < 5) {
        double farads = value * units[from_unit];
        calc.result = farads / units[to_unit];
        snprintf(calc.unit, sizeof(calc.unit), "%s", unit_names[to_unit]);
        snprintf(calc.result_str, sizeof(calc.result_str), "%.4g %s", calc.result, unit_names[to_unit]);
    } else {
        strcpy(calc.result_str, "Error: Invalid unit");
    }
    return calc;
}

ElectronicsCalc electronics_calc_freq_period(double freq) {
    ElectronicsCalc calc = {CALC_FREQ_PERIOD, freq, 0, 0, 0, "", "s"};
    if(freq != 0) {
        calc.result = (double)1.0 / freq;
        snprintf(calc.result_str, sizeof(calc.result_str), "%.4g s (%.4g Hz)", calc.result, freq);
    } else {
        strcpy(calc.result_str, "Error: Freq cannot be 0");
    }
    return calc;
}

ElectronicsCalc electronics_calc_duty_cycle(double on_time, double period) {
    ElectronicsCalc calc = {CALC_DUTY_CYCLE, on_time, period, 0, 0, "", "%"};
    if(period != 0) {
        calc.result = (on_time / period) * (double)100.0;
        snprintf(calc.result_str, sizeof(calc.result_str), "%.2f%%", calc.result);
    } else {
        strcpy(calc.result_str, "Error: Period cannot be 0");
    }
    return calc;
}

void electronics_calculator_start(ViewDispatcher* view_dispatcher) {
    ElecCalcView* view = malloc(sizeof(ElecCalcView));
    view->view_dispatcher = view_dispatcher;
    view->current_type = CALC_OHM_LAW_V;
    view->input1 = view->input2 = view->input3 = 0;
    
    view->var_list = variable_item_list_alloc();
    
    VariableItem* item = variable_item_list_add(view->var_list, "Calculation", 11, elec_calc_type_changed, view);
    for(int i = 0; i < 11; i++) {
        variable_item_add_value(item, calc_types[i], NULL);
    }
    
    item = variable_item_list_add(view->var_list, "Calculate", 0, elec_calc_calculate_callback, view);
    variable_item_set_current_value_text(item, "Press OK");
    
    view->result_widget = widget_alloc();
    widget_add_button_element(view->result_widget, GuiButtonTypeRight, "Back", result_back_callback, view);
    
    view_dispatcher_add_view(view_dispatcher, 0, variable_item_list_get_view(view->var_list));
    view_dispatcher_add_view(view_dispatcher, 1, widget_get_view(view->result_widget));
    view_dispatcher_switch_to_view(view_dispatcher, 0);
    
    view_dispatcher_set_navigation_event_callback(view_dispatcher, elec_calc_nav_callback);
}