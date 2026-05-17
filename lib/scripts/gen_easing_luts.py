import math

SCALE = 65536

def to_fp(v):
    return int(round(v * SCALE))


def gen_lut(name, func, n=257):
    print(f"const sut_real_t sut_ease_{name}_lut[{n}] = {{")
    for i in range(n):
        t = i / (n - 1)
        v = func(t)
        print(f"    {to_fp(v)},")
    print("};")
    print()


def sine_in(t):
    return 1 - math.cos(t * math.pi / 2)

def sine_out(t):
    return math.sin(t * math.pi / 2)

def sine_in_out(t):
    return -(math.cos(math.pi * t) - 1) / 2


def expo_in(t):
    return 0 if t == 0 else 2 ** (10 * (t - 1))

def expo_out(t):
    return 1 if t == 1 else 1 - 2 ** (-10 * t)

def expo_in_out(t):
    if t == 0: return 0
    if t == 1: return 1
    if t < 0.5:
        return 2 ** (10 * (2 * t - 1)) / 2
    return (2 - 2 ** (-10 * (2 * t - 1))) / 2


def circ_in(t):
    return 1 - math.sqrt(1 - t * t)

def circ_out(t):
    return math.sqrt(1 - (t - 1) * (t - 1))

def circ_in_out(t):
    if t < 0.5:
        return (1 - math.sqrt(1 - 4 * t * t)) / 2
    return (1 + math.sqrt(1 - 4 * (t - 1) * (t - 1))) / 2


C1 = 1.70158
C3 = C1 + 1

def back_in(t):
    return C3 * t ** 3 - C1 * t ** 2

def back_out(t):
    return 1 + C3 * (t - 1) ** 3 + C1 * (t - 1) ** 2

C2 = C1 * 1.525

def back_in_out(t):
    if t < 0.5:
        return ((C2 + 1) * (2 * t) ** 3 - C2 * (2 * t) ** 2) / 2
    return ((2 * t - 2) ** 2 * ((C2 + 1) * (2 * t - 2) + C2) + 2) / 2


C4 = 2 * math.pi / 3

def elastic_in(t):
    if t == 0: return 0
    if t == 1: return 1
    return -(2 ** (10 * t - 10)) * math.sin((t * 10 - 10.75) * C4)

def elastic_out(t):
    if t == 0: return 0
    if t == 1: return 1
    return 2 ** (-10 * t) * math.sin((t * 10 - 0.75) * C4) + 1

C5 = 2 * math.pi / 4.5

def elastic_in_out(t):
    if t == 0: return 0
    if t == 1: return 1
    if t < 0.5:
        return -(2 ** (20 * t - 10) * math.sin((20 * t - 11.125) * C5)) / 2
    return (2 ** (-20 * t + 10) * math.sin((20 * t - 11.125) * C5)) / 2 + 1


def bounce_out(t):
    n1 = 7.5625
    d1 = 2.75
    if t < 1 / d1:
        return n1 * t * t
    elif t < 2 / d1:
        t -= 1.5 / d1
        return n1 * t * t + 0.75
    elif t < 2.5 / d1:
        t -= 2.25 / d1
        return n1 * t * t + 0.9375
    else:
        t -= 2.625 / d1
        return n1 * t * t + 0.984375

def bounce_in(t):
    return 1 - bounce_out(1 - t)

def bounce_in_out(t):
    if t < 0.5:
        return (1 - bounce_out(1 - 2 * t)) / 2
    return (1 + bounce_out(2 * t - 1)) / 2


print('#ifndef SUT_USE_FLOAT')
print('#include "sut_easing.h"')
print()

gen_lut("sine_in", sine_in)
gen_lut("sine_out", sine_out)
gen_lut("sine_in_out", sine_in_out)

gen_lut("expo_in", expo_in)
gen_lut("expo_out", expo_out)
gen_lut("expo_in_out", expo_in_out)

gen_lut("circ_in", circ_in)
gen_lut("circ_out", circ_out)
gen_lut("circ_in_out", circ_in_out)

gen_lut("back_in", back_in)
gen_lut("back_out", back_out)
gen_lut("back_in_out", back_in_out)

gen_lut("elastic_in", elastic_in)
gen_lut("elastic_out", elastic_out)
gen_lut("elastic_in_out", elastic_in_out)

gen_lut("bounce_in", bounce_in)
gen_lut("bounce_out", bounce_out)
gen_lut("bounce_in_out", bounce_in_out)

print('#endif')
