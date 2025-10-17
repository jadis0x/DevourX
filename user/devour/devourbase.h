#pragma once

#include <string>

namespace Base {

	namespace GlobalVar {
		extern app::ServerConnectToken* __g_connectToken;
	}

	namespace System {
		std::string generate_random_unique_id(int length = 16);
		uint64_t GenerateRandomSteamID64();
	}

	namespace Steam {
		uint64_t GetUserID();
	}

	namespace Game {
		bool IsInGame();
	}

	class DevourNet {
	public:
		static bool IsHost();
		static float GetPing();
		static uint32_t GetConnectionID();
	};

	namespace Gameplay {
		std::string GetSceneName();
		bool IsPlayerCrawling(app::NolanBehaviour* nolan);
		bool IsSequencePlaying();

		void FullBright(app::NolanBehaviour* localPlayer_nolanBehaviour);
		void ForceLobbyStart();
		void SetProgressTo(int32_t progress);
		void CreateLobby(const char* region_code, int lobby_limit, bool is_private);
		void StartCarryItem(const char* itemName);
		void StartCarryAnimal(const char* animal_name);
		void InstantiatePrefab(const char* prefabName, app::PrefabId app::BoltPrefabs__StaticFields::* prefabField);

		// events
		void Knockout(app::GameObject* target_player);
		void Revive(app::GameObject* target_player);
		void Jumpscare(app::GameObject* target_player);
		void TP(app::GameObject* target_player);
		void TPAzazel(app::GameObject* target_player);
		void Shoot(app::GameObject* target_player);

		void setRank(app::GameObject* local_player, int32_t new_rank);
	}
}

