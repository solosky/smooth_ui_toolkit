# Smooth UI Toolkit (C99 Port)

Standard C99 (ANSI C) 实现的高性能动画引擎，专为嵌入式 MCU 设计。支持无 FPU 的 Cortex-M0/M3、ESP32-C 系列等低成本芯片。

## 特点

| 特性 | 说明 |
|------|------|
| **Q16.16 固定点** | 默认 `int32_t` 固定点运算，零浮点性能开销 |
| **SUT_USE_FLOAT** | 定义此宏即切换为原生 `float`，适配带 FPU 的 MCU |
| **零 math.h 依赖** | 缓动函数使用整数多项式 + 预计算 LUT，弹簧使用欧拉积分 |
| **外部分配器** | `sut_allocator_set()` 可替换 `malloc/free` 为内存池 |
| **自包含** | 无任何外部依赖，仅需 C99 编译器 |

## 模块

```
include/
  sut_types.h      — sut_real_t (Q16.16 / float 切换), 转换宏
  sut_math.h       — clamp, lerp, fp_mul/div/sqrt, vec2/vec4/rect
  sut_easing.h     — 12 个多项式缓动 + 18 个 LUT 预计算缓动函数
  sut_spring.h     — 弹簧物理 (欧拉积分)
  sut_animate.h    — 值/vec2/vec4 动画 (缓动 + 弹簧模式)
  sut_sequence.h   — 动画序列
  sut_color.h      — RGB888, HSV, 颜色混合
  sut_hal.h        — tick/delay 抽象
  sut_ringbuf.h    — 环形缓冲区
  sut_signal.h     — 观察者模式
  sut_pool.h       — 对象池
sut_allocator.h    — 外部分配器接口
sut_errors.h       — 错误码
sut_config.h       — SUT_USE_FLOAT 开关
```

## API 示例

```c
#include "sut.h"

// 缓动动画: 从 0 到 10，历时 1 秒
sut_animate_t anim;
sut_animate_init_easing(&anim, SUT_FP_C(0), SUT_FP_C(10), SUT_FP_C(1));
anim.config.easing.easing = sut_ease_cubic_out;
anim.repeat = 255;

while (1) {
    bool done = sut_animate_update(&anim, SUT_FP_C(1.0f / 60));
    int value = SUT_REAL_TO_INT(anim.current);
    draw_ball(value, 233);
}

// 弹簧动画: 从 100 弹到 600
sut_animate_init_spring(&anim, SUT_FP_C(100), SUT_FP_C(600));
anim.config.spring.stiffness = SUT_FP_C(120);
anim.config.spring.damping = SUT_FP_C(10);

while (1) {
    sut_animate_update(&anim, SUT_FP_C(1.0f / 60));
}
```

## 编译

```bash
# 默认固定点模式 (无 FPU MCU)
gcc -std=c99 -Iinclude -c src/sut_math.c

# 浮点模式 (带 FPU 的 MCU)
gcc -std=c99 -DSUT_USE_FLOAT -Iinclude -c src/sut_math.c
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
add_subdirectory(path/to/smooth_ui_toolkit)
target_link_libraries(your_project PUBLIC sut)
```

### ESP-IDF 工程

clone 仓库，丢到 `components` 目录即可，构建系统会自动注册。

### PlatformIO 工程

clone 仓库，丢到 `lib` 目录即可。

### Arduino 工程

clone 仓库，丢到 `libraries` 目录即可。

## 文档

详细设计与移植计划见 [docs/superpowers/](docs/superpowers/)。

## 许可证

MIT License, Copyright (c) 2023 Forairaaaaa
