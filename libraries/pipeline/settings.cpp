#include "pch-il2cpp.h"
#include "settings.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

Settings settings;

// 简单的配置文件读写函数
void Settings::SaveFilterSettings()
{
    try {
        std::ofstream file("filter_settings.cfg");
        if (file.is_open()) {
            // 保存滤镜设置
            file << bBrightnessFilter << "\n";
            file << contrast << "\n";
            file << gamma << "\n";
            file << saturation << "\n";
            file << temperature << "\n";
            file.close();
        }
    }
    catch (...) {
        // 保存失败处理
    }
}

void Settings::LoadFilterSettings()
{
    try {
        std::ifstream file("filter_settings.cfg");
        if (!file.is_open())
            return;

        std::string line;

        // 读取 bBrightnessFilter
        if (std::getline(file, line)) {
            bBrightnessFilter = (line == "1");
        }

        // 读取 contrast
        if (std::getline(file, line)) {
            contrast = std::stof(line);
        }

        // 读取 gamma
        if (std::getline(file, line)) {
            gamma = std::stof(line);
        }

        // 读取 saturation
        if (std::getline(file, line)) {
            saturation = std::stof(line);
        }

        // 读取 temperature
        if (std::getline(file, line)) {
            temperature = std::stof(line);
        }

        file.close();
    }
    catch (...) {
        // 加载失败处理，使用默认值
        bBrightnessFilter = true;
        contrast = 1.2f;
        gamma = 1.8f;
        saturation = 1.0f;
        temperature = 0.0f;
    }
}

// 传送点保存函数
void Settings::SaveTeleportLocations()
{
    try {
        std::ofstream file("position_settings.cfg");
        if (file.is_open()) {
            // 第一行：传送点数量
            file << customTeleportLocations.size() << "\n";

            for (const auto& location : customTeleportLocations) {
                // 名称
                file << location.name << "\n";
                // 坐标 (x y z)
                file << location.position.x << " " << location.position.y << " " << location.position.z << "\n";
                // 是否有效
                file << (location.isValid ? 1 : 0) << "\n";
            }
            file.close();
            std::cout << "[Teleport] 已保存 " << customTeleportLocations.size() << " 个传送点到 position_settings.cfg" << std::endl;
        }
    }
    catch (...) {
        std::cout << "[Teleport] 错误: 无法保存传送点设置" << std::endl;
    }
}

// 传送点加载函数
void Settings::LoadTeleportLocations()
{
    try {
        std::ifstream file("position_settings.cfg");
        if (!file.is_open()) {
            std::cout << "[Teleport] 未找到保存的传送点设置文件" << std::endl;
            return;
        }

        std::string line;
        customTeleportLocations.clear();

        // 第一行：传送点数量
        if (!std::getline(file, line)) return;
        int count = std::stoi(line);

        for (int i = 0; i < count; i++) {
            CustomTeleportLocation location;

            // 读取名称
            if (!std::getline(file, line)) break;
            location.name = line;

            // 读取坐标
            if (!std::getline(file, line)) break;
            std::istringstream coordStream(line);
            coordStream >> location.position.x >> location.position.y >> location.position.z;

            // 读取有效状态
            if (!std::getline(file, line)) break;
            location.isValid = (line == "1");

            customTeleportLocations.push_back(location);
        }

        file.close();
        std::cout << "[Teleport] 已加载 " << customTeleportLocations.size() << " 个传送点" << std::endl;
    }
    catch (...) {
        std::cout << "[Teleport] 加载传送点设置时出错，使用默认设置" << std::endl;
        customTeleportLocations.clear();
    }
}