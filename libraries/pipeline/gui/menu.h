#pragma once

namespace Menu {
    extern bool init;
    extern bool firstRender;
    extern ImFont* chineseFont;  // 添加这行声明

    void Init();
    void Render();
}