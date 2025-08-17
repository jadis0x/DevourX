#include "pch-il2cpp.h"

#include "player/player.h"

#include "resolver/il2cpp_resolver.h"

static UGameObject go_manager;
static UComponent lplayer;

namespace Players {
    std::vector<app::GameObject*>& PlayersManager::GetAllPlayers() {
        if (playersList.empty()) {
            RefreshPlayers();
        }

        // erase players that are nullptr
        for (auto it = playersList.begin(); it != playersList.end(); ) {
            if (!SafePtr::IsValid(*it)) {
                playersSet.erase(*it);
                it = playersList.erase(it);
            }
            else {
                ++it;
            }
        }

        // push new players
        app::GameObject__Array* currentGameObjects = go_manager.FindAllWithTag("Player");
        if (!currentGameObjects || currentGameObjects->max_length == 0 || !SafePtr::IsValid(currentGameObjects->vector[0])) {
            return playersList;
        }

        for (int i = 0; i < currentGameObjects->max_length; ++i) {
            app::GameObject* current_player = currentGameObjects->vector[i];
            if (SafePtr::IsValid(current_player) && !playersSet.contains(current_player)) {
                playersSet.insert(current_player);
                playersList.push_back(current_player);
            }
        }

        return playersList;
    }

    void PlayersManager::RefreshPlayers() {
        playersSet.clear();
        playersList.clear();

        const app::GameObject__Array* gameObjects = go_manager.FindAllWithTag("Player");
        if (gameObjects && gameObjects->max_length > 0) {
            for (il2cpp_array_size_t i = 0; i < gameObjects->max_length; ++i) {
                app::GameObject* currentPlayer = gameObjects->vector[i];
                if (SafePtr::IsValid(currentPlayer)) {
                    playersSet.insert(currentPlayer);
                    playersList.push_back(currentPlayer);
                }
            }
        }
    }

    void PlayersManager::ClearCache()
    {
        playersSet.clear();
        playersList.clear();
    }

    std::vector<app::GameObject*>& GetPlayers() {
        return PlayersManager::Instance().GetAllPlayers();
    }
}

namespace LocalPlayer {

    app::GameObject* LocalPlayerManager::GetLocalPlayer() {
        if (cachedLocalPlayer && SafePtr::IsValid(cachedLocalPlayer)) {
            return cachedLocalPlayer;
        }

        cachedLocalPlayer = nullptr;

        const std::vector<app::GameObject*>& playerList = Players::GetPlayers();
        for (auto& currentPlayer : playerList)
        {
            lplayer.Attach(currentPlayer); // attach currentPlayer to UComponent

            if (app::Component* nb_component = lplayer.GetComponentByName("NolanBehaviour"))
            {
                if (const auto nb = reinterpret_cast<app::NolanBehaviour*>(nb_component); nb && PlayerHelper::IsLocalPlayer(nb))
                {
                    cachedLocalPlayer = currentPlayer;
                    return cachedLocalPlayer;
                }
            }
        }

        return nullptr;
    }

    app::NolanBehaviour* LocalPlayerManager::GetNolan() {
        if (cachedNolan != nullptr && SafePtr::IsValid(reinterpret_cast<app::Object_1*>(cachedNolan))) {
            return cachedNolan;
        }

        app::GameObject* local_player = GetLocalPlayer();
        if (!SafePtr::IsValid(local_player)) {
            return nullptr;
        }

        UComponent nb_component;
        nb_component.Attach(local_player);

        if (app::Component* get_component = nb_component.GetComponentByName("NolanBehaviour"); SafePtr::IsValid(reinterpret_cast<app::Object_1*>(get_component))) {
            cachedNolan = reinterpret_cast<app::NolanBehaviour*>(get_component);
        }
        else {
            cachedNolan = nullptr;
        }

        return cachedNolan;
    }

    void LocalPlayerManager::ClearCache()
    {
        cachedLocalPlayer = nullptr;
        cachedNolan = nullptr;
    }

    app::GameObject* GetLocalPlayer() {
        return LocalPlayerManager::Instance().GetLocalPlayer();
    }

    app::NolanBehaviour* GetNolan() {
        return LocalPlayerManager::Instance().GetNolan();
    }
}

bool PlayerHelper::IsLocalPlayer(app::NolanBehaviour* player)
{
    if (const auto bolt_entity = app::EntityBehaviour_get_entity(reinterpret_cast<app::EntityBehaviour*>(player), nullptr); not bolt_entity
        || not app::BoltEntity_get_IsAttached(bolt_entity, nullptr)
        || not app::BoltEntity_get_IsOwner(bolt_entity, nullptr))
        return false;

    return true;
}
