# MotionC

Standard C99 (ANSI C) 动画引擎，专为嵌入式 MCU 设计。支持无 FPU 的 Cortex-M0/M3、ESP32-C 系列等低成本芯片。

## 特点

| 特性 | 说明 |
|------|------|
| **Q16.16 固定点** | 默认 `int32_t` 固定点运算，零浮点性能开销 |
| **MC_USE_FLOAT** | 定义此宏即切换为原生 `float`，适配带 FPU 的 MCU |
| **零 math.h 依赖** | 缓动函数使用整数多项式 + 预计算 LUT，弹簧使用欧拉积分 |
| **外部分配器** | `mc_allocator_set()` 可替换 `malloc/free` 为内存池 |
| **自包含** | 无任何外部依赖，仅需 C99 编译器 |

## 模块

```
include/
  mc_types.h      — mc_real_t (Q16.16 / float 切换), 转换宏
  mc_math.h       — clamp, lerp, fp_mul/div/sqrt, vec2/vec4/rect
  mc_easing.h     — 12 个多项式缓动 + 18 个 LUT 预计算缓动函数
  mc_spring.h     — 弹簧物理 (欧拉积分)
  mc_animate.h    — 值/vec2/vec4 动画 (缓动 + 弹簧模式)
  mc_sequence.h   — 动画序列
  mc_color.h      — RGB888, HSV, 颜色混合
  mc_hal.h        — tick/delay 抽象
  mc_ringbuf.h    — 环形缓冲区
  mc_signal.h     — 观察者模式
  mc_pool.h       — 对象池
  mc_allocator.h  — 外部分配器接口
  mc_errors.h     — 错误码
  mc_config.h     — MC_USE_FLOAT 开关
```

## API 示例

```c
#include "mc.h"

// 缓动动画: 从 0 到 10，历时 1 秒
mc_animate_t anim;
mc_animate_init_easing(&anim, MC_FP_C(0), MC_FP_C(10), MC_FP_C(1));
anim.config.easing.easing = mc_ease_cubic_out;
anim.repeat = 255;

while (1) {
    bool done = mc_animate_update(&anim, MC_FP_C(1.0f / 60));
    int value = MC_REAL_TO_INT(anim.current);
    draw_ball(value, 233);
}

// 弹簧动画: 从 100 弹到 600
mc_animate_init_spring(&anim, MC_FP_C(100), MC_FP_C(600));
anim.config.spring.stiffness = MC_FP_C(120);
anim.config.spring.damping = MC_FP_C(10);

while (1) {
    mc_animate_update(&anim, MC_FP_C(1.0f / 60));
}
```

## 编译

```bash
# 默认固定点模式 (无 FPU MCU)
gcc -std=c99 -Iinclude -c src/mc_math.c

# 浮点模式 (带 FPU 的 MCU)
gcc -std=c99 -DMC_USE_FLOAT -Iinclude -c src/mc_math.c
```

## 运行 raylib 示例

```bash
git submodule update --init --depth 1 --branch 5.5 third_party/raylib
cmake -B build && cmake --build build -j
./build/example/c99_basic_animate
```

可用示例：`c99_animate`, `c99_animate_value`, `c99_animate_value_opts`, `c99_basic_animate`, `c99_bubbles`, `c99_multi_cursor`, `c99_easing_curves`

## 运行测试

```bash
cmake -B build && cmake --build build -j
cd build && ctest
```

## 库引入

### CMake 工程

```cmake
add_subdirectory(path/to/motionc)
target_link_libraries(your_project PUBLIC mc)
```

### ESP-IDF 工程

clone 仓库，丢到 `components` 目录即可。

### PlatformIO 工程

clone 仓库，丢到 `lib` 目录即可。

### Arduino 工程

clone 仓库，丢到 `libraries` 目录即可。

## 许可证

MIT License, Copyright (c) 2023 Forairaaaaa
