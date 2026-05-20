# qmk_chunri87

春日键盘（CHUNRI 系列）的 **QMK 定制固件** 仓库。编译后烧录到键盘 MCU，控制键位、灯光、蓝牙/2.4G/USB 三模等行为。

> **说明**：本地目录可能叫 `spring80`，但本仓库键盘目标名为 **`chunri87`**（87 键配列）。仓库内 **没有** `chunri80` 目录；若你持有的是另一版 80 键固件，需在对应仓库中查找。

---

## 这是固件还是网页？

| 类型 | 是否在本仓库 | 说明 |
|------|--------------|------|
| **键盘固件（C）** | ✅ 是 | 主体代码，改完后需编译、烧录 |
| **独立网页/前端项目** | ❌ 否 | 无 React/Vue 等应用 |
| **VIA / QMK Configurator 描述** | ⚠️ 仅 JSON | `chunri87.json` 供外部工具识别键盘；改键界面在 VIA 或 [config.qmk.fm](https://config.qmk.fm) |
| **QMK 文档 HTML** | 仅 `docs/` | 上游 QMK 文档，与本键盘功能无关 |

**结论：要改键盘“具体功能”，主要改 C 固件；只有“纯改键位、不改逻辑”时可用 VIA 图形界面。**

---

## 硬件与编译目标

| 项目 | 值 |
|------|-----|
| 键盘名（QMK） | `chunri87` |
| 默认键映射 | `via`（支持 VIA） |
| MCU | WB32FQ95 |
| Bootloader | `wb32-dfu` |
| 固件版本号 | `SOFTWARE_VERSION = 4`（见 `rules.mk`） |

主要能力开关（`keyboards/chunri87/rules.mk`）：

- `MULTIMODE_ENABLE` — 三模（蓝牙 / 2.4G / USB）
- `RGB_MATRIX_ENABLE` / `RGBLIGHT_ENABLE` — 键面灯 + 侧/Logo 灯
- `RGB_MATRIX_BLINK_ENABLE` — 配对、低电等闪烁提示
- `VIA_ENABLE` — 见 `keymaps/via/rules.mk`

---

## 目录结构（与本键盘相关）

```
qmk_chunri87/
├── keyboards/
│   ├── chunri87/                 ← 【核心】本键盘所有定制
│   │   ├── config.h              硬件引脚、RGB、三模、休眠、电池等宏
│   │   ├── rules.mk              功能开关、驱动、multimode 引用
│   │   ├── chunri87.h            物理 LAYOUT 矩阵宏（6×17）
│   │   ├── chunri87.c            RGB Matrix 灯珠坐标映射 g_led_config
│   │   ├── chunri87.json         VIA / Configurator 键盘描述（非网页源码）
│   │   ├── halconf.h / mcuconf.h ChibiOS 底层配置
│   │   └── keymaps/
│   │       └── via/
│   │           ├── keymap.c      【最常改】键位 + 业务逻辑
│   │           └── rules.mk      VIA_ENABLE 等
│   └── multimode/                三模、休眠、配对等通用模块（多键盘共用）
│       ├── multimode.c / .h
│       ├── immobile/             用户钩子框架（im_*_user）
│       └── rgb_matrix_blink.c    灯效闪烁
├── quantum/                      QMK 核心
├── tmk_core/                     USB 协议等
└── docs/                         QMK 官方文档（可忽略）
```

---

## 改功能该改哪个文件？

按需求对照下表；**优先改 `keymap.c` 和 `config.h`**，除非涉及三模底层协议。

### 1. 键位、Fn 层、Win/Mac 层

**文件**：`keyboards/chunri87/keymaps/via/keymap.c`

- `keymaps[][]`：四层布局  
  - `WIN_BASE` / `WIN_FN`  
  - `MAC_BASE` / `MAC_FN`
- `LAYOUT(...)` 定义在 `chunri87.h`，改键数量或排布需同时改 `.h` 与 `keymap.c`

**示例**：Fn 层三模切换键在 `WIN_FN` / `MAC_FN` 中：

- `IM_BT1` / `IM_BT2` / `IM_BT3` — 蓝牙 1/2/3  
- `IM_2G4` — 2.4G  
- `IM_USB` — USB 有线  

### 2. 自定义按键逻辑（单按、组合、拦截默认行为）

**文件**：`keyboards/chunri87/keymaps/via/keymap.c`

| 函数 | 作用 |
|------|------|
| `im_process_record_user()` | 单键/短按：RGB 色相、侧灯模式、Win 键屏蔽、电量查询 `IM_BATQ` 等 |
| `im_lkey_process_user()` | 长按：`OS_SW`（Win↔Mac）、`OS_CLR`（恢复出厂） |
| `rgb_matrix_indicators_advanced_user()` | 指示灯：Caps、Scroll、USB 模式灯、侧灯颜色、低电灭灯等 |
| `im_init_user()` / `im_loop_user()` | 初始化、周期读充电脚、低电关机等 |
| `im_reset_settings_user()` | 恢复出厂时的 RGB 提示 |
| `lkey_define_user[]` | 长按时间与键码（如 `OS_SW` 3000ms） |

**本键映射自定义键码**（`enum user_keys` / `safe_key`）：

| 键码 | 说明 |
|------|------|
| `OS_SW` | 短按无效；长按切换 Win/Mac 默认层 |
| `OS_CLR` | 长按恢复出厂设置 |
| `GU_TOGG` | 切换 Win 键屏蔽（`confinfo.no_gui`） |
| `RGB_TOG` | **主背光**开关（键面 RGB Matrix，与 Logo 独立） |
| `RL_TOG` | **Logo/侧灯**开关（与主背光独立，状态写入 `confinfo.layer`） |
| `RL_MOD` / `RL_HUI` 等 | Logo 颜色/亮度（需先 `RL_HUI` 选色，`rgb_index≠0` 才会亮） |
| `RGB_HUI` | 切换主背光预设 HSV 色相 |
| `IM_BATQ` | 非 USB 模式下查询电量 |
| `US_TS1`~`US_TS3` / `US_STOP` | 射频测试（一般用户勿动） |

三模切换 `IM_BT*` / `IM_USB` 等的 **默认处理** 在 `keyboards/multimode/immobile/immobile.c`；若在 `im_process_record_user` 里 `return false` 可覆盖。

### 主背光与 Logo 灯独立开关

键面主背光（91 颗）与 Logo 侧灯（13 颗）共用灯驱，但逻辑已拆分：

| 操作 | 键码（Fn 层） | 说明 |
|------|----------------|------|
| 开关主背光 | `RGB_TOG` | 只改 `confinfo.rgb_enable`，Logo 亮着时不会断电 |
| 开关 Logo | `RL_TOG` | 只改 `confinfo.layer`（1=Logo 关） |
| Logo 选色 | `RL_HUI` | 循环 `rgb_index` 1~8；0 表示未选色/不亮 |
| Logo 关灯效 | `RL_MOD` | 将 `rgb_index` 置 0 |

实现见 `keymap.c` 中 `chunri_rgb_sync_hw()`：仅当主背光 **且** Logo 都不需要时才 `rgb_matrix_disable()` 并关闭 `RGB_DRIVER_EN_PIN`。

### VIA 中调节饱和度

`chunri87.json` 的 **Lighting** 菜单：

| 子菜单 | 控件 | 说明 |
|--------|------|------|
| Backlight | Brightness / Effect / Speed / Color / Saturation | 键面 `rgb_matrix`（通道 3） |
| Logo | Brightness / Effect / Speed / Color / Saturation | Logo `rgblight`（通道 2），与背光结构一致 |

Logo 的 **Effect** 含 Breathing、Rainbow、Snake 等；**Saturation** 滑条单独调 `rgblight` 饱和度。键盘 Fn 的 `RL_HUI` 预设色仅在 **rgblight 关闭** 时生效。

固件处理见 `keyboards/chunri87/via_custom.c`（自定义 VIA 通道 `0`，value id `16`/`17`）。改完后在 VIA 里点 **Save** 才会写入 EEPROM。

### 3. 蓝牙名、休眠、电池、引脚、RGB 能力

**文件**：`keyboards/chunri87/config.h`

常见宏示例：

```c
#define MM_BT1_NAME "CHUNRI87 BT1"      // 蓝牙广播名
#define MM_SLEEP_TIMEOUT (3 * 60000)    // 无操作休眠（ms）
#define BATTERY_CAPACITY_LOW 10         // 低电量阈值
#define MATRIX_ROWS 6 / MATRIX_COLS 17  // 矩阵规模
#define RGB_MATRIX_LED_COUNT (91+13)    // 键面 + Logo 灯数量
#define IM_LKEY_COUNT 4                 // 用户长按键数量
```

**文件**：`keyboards/chunri87/rules.mk` — 打开/关闭整个子系统（如 `MULTIMODE_ENABLE`、`AUDIO_ENABLE`）。

### 4. 灯珠与按键的对应关系（灯效错位时改这里）

**文件**：`keyboards/chunri87/chunri87.c`

- `g_led_config`：每个 LED 索引对应的 `{x, y}` 坐标与 `LAYOUT` 中的键位对应关系。

### 5. 三模、配对、休眠、UART 协议

**目录**：`keyboards/multimode/`

| 文件 | 作用 |
|------|------|
| `multimode.c` | 模式切换、休眠状态机 |
| `immobile/immobile_r1.c` | `IM_BT*`、`IM_USB` 默认逻辑、`process_record_kb` 调用链 |
| `immobile/immobile.h` | `im_*_user` 钩子声明 |
| `bts_lib.h` | 与无线模组通信 |
| `rgb_matrix_blink.c` | 配对/低电闪烁 |

`chunri87/rules.mk` 通过 `include .../multimode/multimode.mk` 链入上述模块。

**调用顺序**（理解调试用）：

```
process_record_kb (immobile)
  → im_lkey_process_record
  → im_process_record_kb
  → process_record_user (QMK 标准，本键盘多用 im_process_record_user)
```

### 6. 仅用图形界面改键（不改 C 逻辑）

1. 编译带 VIA 的固件（`keymap` 选 `via`）。  
2. 烧录后打开 [VIA](https://usevia.app/) 或支持 VID/PID 的 VIA 版本。  
3. 键盘描述来自 `chunri87.json`（`vendorId` `0x342d`，`productId` `0xe421`）。

**限制**：`OS_SW`、电量、三模指示灯等 **依赖固件逻辑**，VIA 只能改键位映射，不能替代 `keymap.c` 里的代码。

---

## 关于 `info.json` 与 “No LAYOUTs defined”

原厂上传的 `keyboards/chunri87/` **没有** `info.json`，物理布局在 **`chunri87.h` 的 `LAYOUT(...)` 宏**里（与 `keymap.c` 一致）。

新版 **QMK MSYS** 的 `qmk compile` 会解析该宏；若仍报 `No LAYOUTs defined`，多为误加了不完整的手写 `info.json`，或需使用本仓库已修正的 `lib/python/qmk/info.py`（允许仅 C 宏布局、不强制 `info.json`）。

**不要**为 chunri87 随意新建简化版 `info.json`；VIA 用的是 **`chunri87.json`**（另一套格式）。

可选：在 QMK MSYS 中生成参考用 `info.json`（开发者模式）：

```bash
qmk generate-info-json -kb chunri87 --overwrite
```

## 编译与烧录

需先按 [QMK 官方文档](https://docs.qmk.fm/#/newbs_getting_started) 配置环境（Windows 推荐 MSYS2 / QMK CLI）。

在本仓库根目录执行：

```bash
# 编译
qmk compile -kb chunri87 -km via

# 编译并烧录（键盘进入 DFU 后）
qmk flash -kb chunri87 -km via
```

可将默认键盘写入用户配置，避免每次带参数：

```bash
qmk config user.keyboard chunri87
qmk config user.keymap via
qmk compile
```

产物一般在 `.build/` 或 QMK 提示的路径，格式取决于 bootloader（本板为 `wb32-dfu`）。

---

## 配置文件速查

| 需求 | 主要文件 |
|------|----------|
| 改键位 | `keymaps/via/keymap.c` → `keymaps[][]` |
| 改 Fn 功能、RGB 行为、Win/Mac | `keymaps/via/keymap.c` → `im_*_user` |
| 改休眠/蓝牙名/引脚/灯数量 | `config.h` |
| 关掉蓝牙或 RGB 整个功能 | `rules.mk` |
| 灯效位置不对 | `chunri87.c` → `g_led_config` |
| 改矩阵行列数或走线 | `config.h` + `chunri87.h` |
| 三模配对/休眠底层 | `keyboards/multimode/` |
| VIA 识别名/菜单 | `chunri87.json` |

---

## 常见问题

**Q：我想改 chunri80，为什么只有 chunri87？**  
A：本仓库仅维护 `chunri87` 目标。路径中的 `spring80` 多为本地文件夹命名，与 QMK 键盘名无关。80 键版本若在其它仓库，需打开对应 `keyboards/<name>/`。

**Q：改完 `keymap.c` 要不要改 JSON？**  
A：只改逻辑或键码排列且仍用 VIA 时，通常 **不必** 改 `chunri87.json`；若增删物理键或改 matrix 尺寸，需同步 JSON 与 `chunri87.h`。

**Q：有没有网页可以在线编译？**  
A：可使用 QMK 在线 Configurator，但本 fork 含 `multimode` 等私有模块，**建议本地 `qmk compile`**，与官方 QMK 主仓不一定兼容。

---

## 许可

基于 QMK / GPL-2.0 等许可证（各文件头有版权声明）。修改与分发请遵守对应源文件中的 License 条款。
