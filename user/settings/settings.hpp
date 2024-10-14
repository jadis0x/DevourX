#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <imgui/imgui.h>
#include <string>

namespace settings {
	extern int height;
	extern int width;

	extern bool fullBright;
	extern bool unlimited_uv;

	extern bool chat_spam;
	extern std::string message;
	extern bool spoof_level;
	extern int new_level;
	extern int player_count;
	extern bool steam_name_spoof;
	extern std::string new_name;
	extern bool server_name_spoof;
	extern std::string server_name;
	extern bool fly;
	extern float fly_speed;
	extern bool exp_modifier;
	extern int new_exp;
	extern bool change_player_speed;
	extern int new_speed;
	extern bool freeze_azazel;
	extern float new_azazel_speed;
	extern bool itemSpawner;
	
	// hooks
	extern bool disable_longInteract;
	extern bool robe_unlock;
	extern bool canInteractHook;
	extern bool retrievedItems;
	extern bool allPlayersReady;
	extern bool unlockCharacter;
	extern bool unlockEmote;
	extern bool unlockPerk;
	extern bool unlockFlashlight;
	extern bool lockedInteractable;
	extern bool lobbySize;
	extern bool mouseHook;
	extern bool autoRevive;
}

#endif
