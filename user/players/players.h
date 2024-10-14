#pragma once

#include <unordered_set>
#include <vector>

namespace Players {
    // Tüm oyuncularý döndürür
    const std::vector<app::GameObject*>& GetAllPlayers();

    // Burada sadece bildirim yapýyoruz (extern kullanarak)
    extern std::unordered_set<app::GameObject*> playersSet;
    extern std::vector<app::GameObject*> playersList;
    extern app::GameObject* cachedLocalPlayer;
    extern app::NolanBehaviour* cachedNolan;

    app::GameObject__Array* PlayersArray();

    // Oyuncu listesini yeniler
    void RefreshPlayers();

    // Yerel oyuncuyu döndürür
    app::GameObject* GetLocalPlayer();

    // Yerel oyuncunun NolanBehaviour bileþenini döndürür
    app::NolanBehaviour* GetNolan();
}