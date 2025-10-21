# DevourX

DevourX is an open-source C++ mod menu developed specifically for the co-op horror game Devour.

![MenuShowcase](img/Screenshot.png)
![MenuShowcase](img/Screenshot2.jpg)
![MenuShowcase](img/Screenshot3.jpg)

## Requirements
- Windows 10/11 64-bit
- A legitimate copy of Devour installed via Steam or another supported platform
- Microsoft Visual C++ Redistributable (x64) installed [Download here](https://www.techpowerup.com/download/visual-c-redistributable-runtime-package-all-in-one/)

## Quick Start Installation

#### 1. Download the files
Get the latest release from this repository's [Releases](https://github.com/jadis0x/DevourX/releases) section.

The package now includes:
- version.dll
- localization/ folder
- localization/config.json

#### 2. Locate your Devour installation folder
 - Default Steam path (might differ):
```
C:\Program Files (x86)\Steam\steamapps\common\Devour
```

#### 3. Copy all files
 - Extract everything into the main game directory, the same folder as Devour.exe

#### 4. Launch the game
 - Start Devour normally through Steam or a shortcut.
 - The game will automatically detect and load DevourX.

#### 5. Activate the menu
 - Once in-game, press `TAB` key to open or close the DevourX menu.

## Available Features
- Fullbright
- Unlimited UV Light
- UV Light Color Customizer
- Prefab Spawner
- Force Lobby Start
- Unlock Robes
- Unlock Doors
- Disable Long Interact
- Instant Win
- Azazel Speed Modifier
- Player Speed Modifier
- Fly
- Kill / Revive / Jumpscare Players
- ESP (Players, Azazel, Objects)
- EXP Modifier
- Create Lobby Without Player Limit
- SteamID & Name Hook
- Ghost Join

## Language Support
DevourX includes menu text in several languages:

- 🇺🇸 English (`en-US`)
- 🇹🇷 Turkish (`tr-TR`)
- 🇨🇳 Simplified Chinese (`zh-CN`)
- 🇰🇷 Korean (`ko-KR`)

The game asks for your preferred language the first time it runs. You can change it later from **Settings → Language** in the menu or by editing `localization/config.json` and updating `localizationCulture`.

### Add another language
1. Copy `localization/en-US.json` (or the closest file) and rename it with your locale, for example `localization/es-ES.json`.
2. Translate the values, but leave the JSON keys and any placeholders like `%s` or `%d` unchanged.
3. Add your language name to the `languages` section at the top of the file so it shows up in the menu.
4. (Optional) Set it as default in `localization/config.json` while testing.

When you are done, send the new JSON file in a pull request.

## Uninstalling
1. Close Devour.
2. Delete the `version.dll`.

## Security and Ethics
- Respect the experience of other players in online lobbies.
- This repository does not encourage cheating in any way; it is intended solely for modding and technical research.

## Support & Contact
If you'd like to support future development, you can donate here:

- [BuyMeACoffe/jadis0x](https://buymeacoffee.com/jadis0x)
- [PayPal/jadis0x](https://www.paypal.com/paypalme/jadis0x)
- Discord: Jadis0x

