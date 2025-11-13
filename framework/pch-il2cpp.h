#ifndef PCH_IL2CPP_H
#define PCH_IL2CPP_H

// ====================================================
// ImGui 稳定性配置 - 防止随机崩溃
// ====================================================
#define IMGUI_DISABLE_DEBUG_TOOLS           // 禁用调试工具
 #define IMGUI_DISABLE_ASSERTS               // 注释掉这行 - 重要！
// #define IMGUI_DISABLE_OBSOLETE_FUNCTIONS    // 注释掉这行 - 重要！
#define IMGUI_DISABLE_OBSOLETE_KEYIO        // 禁用过时键盘IO
#define IMGUI_DISABLE_DEMO_WINDOWS          // 禁用演示窗口

// ====================================================
// 预编译头文件
// ====================================================

// add headers that you want to pre-compile here
#include "il2cpp-appdata.h"

#endif //PCH_IL2CPP_H