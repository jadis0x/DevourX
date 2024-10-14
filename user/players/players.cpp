#include "pch-il2cpp.h"

#include "players.h"
#include <helpers.h>
#include "UnityEngine/GameObject.h"
#include "UnityEngine/Object.h"
#include "ClientHelper.h"

namespace Players {

	std::unordered_set<app::GameObject*> playersSet;
	std::vector<app::GameObject*> playersList;
	app::GameObject* cachedLocalPlayer = nullptr;
	app::NolanBehaviour* cachedNolan = nullptr;

	app::GameObject__Array* Players::PlayersArray()
	{
		return Object::FindGameObjectsWithTag("Player");
	}

	void RefreshPlayers() {
		playersSet.clear();
		playersList.clear();
		app::GameObject__Array* gameObjects = Object::FindGameObjectsWithTag("Player");
		if (gameObjects && gameObjects->max_length > 0) {
			for (il2cpp_array_size_t i = 0; i < gameObjects->max_length; ++i) {
				app::GameObject* currentPlayer = gameObjects->vector[i];
				if (!Object::IsNull((app::Object_1*)currentPlayer)) {
					playersSet.insert(currentPlayer);
					playersList.push_back(currentPlayer);
				}
			}
		}
	}

	const std::vector<app::GameObject*>& GetAllPlayers() {
		RefreshPlayers(); // Oyuncu listesini her çaðrýda yenile
		return playersList;
	}

	app::GameObject* GetLocalPlayer() {
		if (cachedLocalPlayer != nullptr && !Object::IsNull((app::Object_1*)cachedLocalPlayer)) {
			return cachedLocalPlayer;
		}

		// Eðer cachedLocalPlayer geçersizse, sýfýrla ve yenile
		cachedLocalPlayer = nullptr;

		const std::vector<app::GameObject*>& playerList = GetAllPlayers();

		for (auto& currentPlayer : playerList) {
			app::Component* nbComponent = GameObject::GetComponentByName(currentPlayer, "NolanBehaviour");
			if (nbComponent) {
				app::NolanBehaviour* nb = reinterpret_cast<app::NolanBehaviour*>(nbComponent);
				if (nb && IsLocalPlayer(nb)) {
					cachedLocalPlayer = currentPlayer;
					return cachedLocalPlayer;
				}
			}
		}

		return nullptr;
	}

	app::NolanBehaviour* GetNolan() {
		// Eðer cachedNolan geçerli ise ve null deðilse doðrudan geri döndür
		if (cachedNolan != nullptr && !Object::IsNull((app::Object_1*)cachedNolan)) {
			return cachedNolan;
		}

		// Yerel oyuncuyu al
		app::GameObject* localPlayer = GetLocalPlayer();
		if (!localPlayer || Object::IsNull((app::Object_1*)localPlayer)) {
			return nullptr;
		}

		// "NolanBehaviour" bileþenini al ve geçerli olup olmadýðýný kontrol et
		app::Component* nbComponent = GameObject::GetComponentByName(localPlayer, "NolanBehaviour");
		if (nbComponent != nullptr && !Object::IsNull((app::Object_1*)nbComponent)) {
			cachedNolan = reinterpret_cast<app::NolanBehaviour*>(nbComponent);
		}
		else {
			cachedNolan = nullptr;
		}

		return cachedNolan;
	}

}