#pragma once
#pragma once


namespace Misc {
	void ForceStart();
	void CarryItem(const char* itemName);
	void CarryAnimal(const char* animalName);
	void InstantWin();
	void FullBright(app::NolanBehaviour* nb);
	void Revive(app::GameObject* selectedPlayer);
	void Jumpscare(app::GameObject* selectedPlayer);
	void Kill(app::GameObject* selectedPlayer);
	void RankSpoofer(int value);
	void TpToAzazel();
	void Teleport(app::GameObject* selectedPlayer);
	app::BoltEntity* SafeInstantiate(const char* prefabName);
	app::BoltEntity* InstantiateWithSEH(app::GameObject* id, app::Vector3 pos);
}