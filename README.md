# DevourX

DevourX is an mod menu for the game **Devour**. It works by replacing
`version.dll` in the game's directory with a proxy library that loads the real
system `version.dll` and then runs the injected code. The implementation can be
seen in `framework/version.cpp` where `load_version()` loads the system DLL and
proxies its exported functions.

## Features
- Fly
- Modify player speed
- Unlimited UV light
- Disable long interact
- Unlock all doors and robes
- Modify Azazel speed
- Fullbright mode
- ESP (Azazel, items, players)
- Post FX Override (experimental)
- Modify UV color
- Create lobby with no player limit
- Change room name
- Change Steam ID / player name
- Modify EXP gain
- Level spoofer
- Spawn items
- Kill any players
- Send jumpscare to players
- Instant win

One exclusive feature (**Ghost Join**) is not included in this release.  
Ghost Join allows you to join lobbies completely invisible to other players.  
This feature is only available to users who provide a small support payment.

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

## Contact
- Discord: Jadis0x
