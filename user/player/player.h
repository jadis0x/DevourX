#pragma once

#include <vector>
#include <unordered_set>

namespace PlayerHelper {
    bool IsLocalPlayer(app::NolanBehaviour* player);
}

namespace Players {
    class PlayersManager {
    public:
        static PlayersManager& Instance() {
            static PlayersManager instance;
            return instance;
        }

        std::vector<app::GameObject*>& GetAllPlayers();
        void RefreshPlayers();
        void ClearCache();
    private:
        PlayersManager() = default;
        PlayersManager(const PlayersManager&) = delete;
        PlayersManager& operator=(const PlayersManager&) = delete;

        std::unordered_set<app::GameObject*> playersSet;
        std::vector<app::GameObject*> playersList;
    };

    extern std::vector<app::GameObject*>& GetPlayers(); // Get all players (extern function)
}

namespace LocalPlayer {
    class LocalPlayerManager {
    public:
        static LocalPlayerManager& Instance() {
            static LocalPlayerManager instance;
            return instance;
        }

        app::GameObject* GetLocalPlayer();
        app::NolanBehaviour* GetNolan();

        void ClearCache();
    private:
        LocalPlayerManager() = default;
        LocalPlayerManager(const LocalPlayerManager&) = delete;
        LocalPlayerManager& operator=(const LocalPlayerManager&) = delete;

        app::GameObject* cachedLocalPlayer = nullptr;
        app::NolanBehaviour* cachedNolan = nullptr;
    };

    extern app::GameObject* GetLocalPlayer();   // Get Local Player (extern function)
    extern app::NolanBehaviour* GetNolan();    // Get Nolan Behaviour (extern function)
}