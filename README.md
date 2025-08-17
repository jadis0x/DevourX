# DevourX

DevourX is an mod menu for the game **Devour**. It works by replacing
`version.dll` in the game's directory with a proxy library that loads the real
system `version.dll` and then runs the injected code. The implementation can be
seen in `framework/version.cpp` where `load_version()` loads the system DLL and
proxies its exported functions.

## Prerequisites

- Windows 10/11 x64
- Visual Studio 2019 or later with the Desktop C++ workload
- DirectX 11 SDK (the project links against `d3d11` and related libraries)
- A copy of Devour installed on your machine

All other dependencies (Detours, ImGui, etc.) are included in this repository.

## Building

1. Open `DevourX.sln` in Visual Studio.
2. Select the **x64** configuration (Debug or Release).
3. Build the solution. The output is `version.dll`.

The Debug configuration is preconfigured to place the compiled DLL directly into
the game folder:

```xml
$(OutDir) = D:\SteamLibrary\steamapps\common\Devour\
```

as shown in `DevourX.vcxproj`.
Adjust this path if your installation differs or if you build in Release mode.

## Installing / Injecting

After building, copy the resulting `version.dll` next to `Devour.exe` in your
Devour installation directory. When the game starts it will load this DLL which
in turn loads the real system `version.dll` and starts the cheat logic defined
in `user/main.cpp`.
