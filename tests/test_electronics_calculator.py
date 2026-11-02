#!/usr/bin/env python3
"""
Electronics Calculator Unit Tests
Tests electronics calculation functions independently from the Flipper UI.
"""

import sys
import os
import math

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# Calculator functions (mirroring C implementation)
def ohm_law_v(i, r):
    return i * r, "V"

def ohm_law_i(v, r):
    if r == 0:
        return None, "Error: Div by 0"
    return v / r, "A"

def ohm_law_r(v, i):
    if i == 0:
        return None, "Error: Div by 0"
    return v / i, "Ohm"

def power(v, i):
    return v * i, "W"

def series_r(r1, r2):
    return r1 + r2, "Ohm"

def parallel_r(r1, r2):
    if r1 == 0 or r2 == 0:
        return None, "Error: R cannot be 0"
    return 1.0 / (1.0/r1 + 1.0/r2), "Ohm"

def led_resistor(v_supply, v_forward, i_target):
    if i_target == 0 or v_supply <= v_forward:
        return None, "Error: Invalid values"
    result = (v_supply - v_forward) / i_target
    power_diss = (v_supply - v_forward) * i_target
    return result, f"Ohm ({power_diss:.2g} W)"

def voltage_divider(v_in, r1, r2):
    if r1 + r2 == 0:
        return None, "Error: Div by 0"
    return v_in * (r2 / (r1 + r2)), "V"

def capacitor_conv(value, from_unit, to_unit):
    units = {0: 1e-12, 1: 1e-9, 2: 1e-6, 3: 1e-3, 4: 1.0}
    names = {0: "pF", 1: "nF", 2: "uF", 3: "mF", 4: "F"}
    if from_unit not in units or to_unit not in units:
        return None, "Error: Invalid unit"
    farads = value * units[from_unit]
    result = farads / units[to_unit]
    return result, names[to_unit]

def freq_period(freq):
    if freq == 0:
        return None, "Error: Freq cannot be 0"
    return 1.0 / freq, "s"

def duty_cycle(on_time, period):
    if period == 0:
        return None, "Error: Period cannot be 0"
    return (on_time / period) * 100.0, "%"

# Test functions
def test_ohm_law_v():
    result, unit = ohm_law_v(2.0, 5.0)
    assert abs(result - 10.0) < 0.001
    assert unit == "V"
    print("PASS: test_ohm_law_v")

def test_ohm_law_i():
    result, unit = ohm_law_i(12.0, 4.0)
    assert abs(result - 3.0) < 0.001
    assert unit == "A"
    print("PASS: test_ohm_law_i")

def test_ohm_law_r():
    result, unit = ohm_law_r(9.0, 3.0)
    assert abs(result - 3.0) < 0.001
    assert unit == "Ohm"
    print("PASS: test_ohm_law_r")

def test_ohm_law_div_zero():
    result, unit = ohm_law_i(12.0, 0.0)
    assert result is None
    assert "Div by 0" in unit
    
    result, unit = ohm_law_r(9.0, 0.0)
    assert result is None
    assert "Div by 0" in unit
    print("PASS: test_ohm_law_div_zero")

def test_power():
    result, unit = power(5.0, 2.0)
    assert abs(result - 10.0) < 0.001
    assert unit == "W"
    print("PASS: test_power")

def test_series_r():
    result, unit = series_r(100.0, 220.0)
    assert abs(result - 320.0) < 0.001
    assert unit == "Ohm"
    print("PASS: test_series_r")

def test_parallel_r():
    result, unit = parallel_r(100.0, 100.0)
    assert abs(result - 50.0) < 0.001
    assert unit == "Ohm"
    print("PASS: test_parallel_r")

def test_parallel_zero():
    result, unit = parallel_r(0.0, 100.0)
    assert result is None
    assert "R cannot be 0" in unit
    
    result, unit = parallel_r(100.0, 0.0)
    assert result is None
    assert "R cannot be 0" in unit
    print("PASS: test_parallel_zero")

def test_led_resistor():
    result, unit = led_resistor(5.0, 2.0, 0.02)
    assert abs(result - 150.0) < 0.001
    assert "Ohm" in unit
    print("PASS: test_led_resistor")

def test_led_resistor_invalid():
    result, unit = led_resistor(3.0, 3.3, 0.02)
    assert result is None
    assert "Invalid values" in unit
    
    result, unit = led_resistor(5.0, 2.0, 0.0)
    assert result is None
    assert "Invalid values" in unit
    print("PASS: test_led_resistor_invalid")

def test_voltage_divider():
    result, unit = voltage_divider(12.0, 1000.0, 2000.0)
    assert abs(result - 8.0) < 0.001
    assert unit == "V"
    print("PASS: test_voltage_divider")

def test_voltage_divider_zero():
    result, unit = voltage_divider(12.0, 0.0, 0.0)
    assert result is None
    assert "Div by 0" in unit
    print("PASS: test_voltage_divider_zero")

def test_capacitor_pf_to_uf():
    result, unit = capacitor_conv(1000000.0, 0, 2)  # 1000000 pF -> uF
    assert abs(result - 1.0) < 0.001
    assert unit == "uF"
    print("PASS: test_capacitor_pf_to_uf")

def test_capacitor_uf_to_nf():
    result, unit = capacitor_conv(1.0, 2, 1)  # 1 uF -> nF
    assert abs(result - 1000.0) < 0.001
    assert unit == "nF"
    print("PASS: test_capacitor_uf_to_nf")

def test_capacitor_invalid():
    result, unit = capacitor_conv(1.0, 9, 1)
    assert result is None
    assert "Invalid unit" in unit
    print("PASS: test_capacitor_invalid")

def test_freq_period():
    result, unit = freq_period(1000.0)
    assert abs(result - 0.001) < 0.000001
    assert unit == "s"
    print("PASS: test_freq_period")

def test_freq_zero():
    result, unit = freq_period(0.0)
    assert result is None
    assert "Freq cannot be 0" in unit
    print("PASS: test_freq_zero")

def test_duty_cycle():
    result, unit = duty_cycle(2.0, 10.0)
    assert abs(result - 20.0) < 0.001
    assert unit == "%"
    print("PASS: test_duty_cycle")

def test_duty_cycle_zero_period():
    result, unit = duty_cycle(1.0, 0.0)
    assert result is None
    assert "Period cannot be 0" in unit
    print("PASS: test_duty_cycle_zero_period")

if __name__ == "__main__":
    test_ohm_law_v()
    test_ohm_law_i()
    test_ohm_law_r()
    test_ohm_law_div_zero()
    test_power()
    test_series_r()
    test_parallel_r()
    test_parallel_zero()
    test_led_resistor()
    test_led_resistor_invalid()
    test_voltage_divider()
    test_voltage_divider_zero()
    test_capacitor_pf_to_uf()
    test_capacitor_uf_to_nf()
    test_capacitor_invalid()
    test_freq_period()
    test_freq_zero()
    test_duty_cycle()
    test_duty_cycle_zero_period()
    print("\nAll electronics calculator tests PASSED!")