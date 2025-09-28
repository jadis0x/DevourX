#include "pch-il2cpp.h"

#include "esp.h"

#include "helpers.h"
#include <algorithm>
#include <pipeline/settings.h>
#include <vector>
#include <player/player.h>
#include <unordered_map>

static UCamera uCamera;

AzazelEsp::AzazelEsp() : valid(false) {}

void AzazelEsp::Update(app::Object_1* instance) {
	valid = false;
	entities.clear();

	if (!instance || !SafePtr::IsValid(instance))
		return;

	auto* behaviour = reinterpret_cast<app::SurvivalAzazelBehaviour*>(instance);
	app::GameObject* go = app::Component_get_gameObject(reinterpret_cast<app::Component*>(behaviour), nullptr);
	app::Transform* tf = app::Component_get_transform(reinterpret_cast<app::Component*>(behaviour), nullptr);

	if (!SafePtr::IsValid(go) || !SafePtr::IsValid(reinterpret_cast<app::Object_1*>(tf)))
		return;

	if (!app::GameObject_get_activeInHierarchy(go, nullptr))
		return;

	app::Vector3 pos = app::Transform_get_position(tf, nullptr);

	EspEntity e;
	e.gameObject = go;
	e.transform = tf;
	e.position = pos;
	e.headOffset = 1.8f;
	e.footOffset = -0.25f;
	e.label = std::string("Azazel");
	e.color = IM_COL32(255, 0, 0, 255);

	entities.push_back(e);
	valid = true;
}

void AzazelEsp::Draw() {
	if (!valid) return;

	ImGuiIO& io = ImGui::GetIO();
	auto drawList = ImGui::GetBackgroundDrawList();

	for (const auto& entity : entities) {
		bool success = false;

		float distance = uCamera.DistanceTo(entity.position);
		if (distance < 1.5f || distance > 80.f) continue;

		app::Vector3 head = entity.position;
		head.y += entity.headOffset;

		app::Vector3 foot = entity.position;
		foot.y += entity.footOffset;

		app::Vector3 screenHead = uCamera.WorldToScreen(head, &success);
		app::Vector3 screenFoot = uCamera.WorldToScreen(foot, &success);

		auto IsOnScreen = [](const app::Vector3& screenPos, const ImVec2& screenSize) {
			return screenPos.x >= 0 && screenPos.x <= screenSize.x &&
				screenPos.y >= 0 && screenPos.y <= screenSize.y &&
				screenPos.z > 0;
			};

		bool isOnScreen = success && IsOnScreen(screenHead, io.DisplaySize) && IsOnScreen(screenFoot, io.DisplaySize);

		if (isOnScreen) {
			screenHead.y = io.DisplaySize.y - screenHead.y;
			screenFoot.y = io.DisplaySize.y - screenFoot.y;

			float height = screenFoot.y - screenHead.y;
			float width = height / 2.8f;

			ImVec2 topLeft = { screenHead.x - width / 2, screenHead.y };
			ImVec2 bottomRight = { screenHead.x + width / 2, screenFoot.y };

			drawList->AddRect(topLeft, bottomRight, entity.color, 0, 0, 2.f);

			ImVec2 labelSize = ImGui::CalcTextSize(entity.label.c_str());
			ImVec2 labelPos = { (topLeft.x + bottomRight.x) / 2 - labelSize.x / 2, topLeft.y - labelSize.y - 2.f };
			drawList->AddText(labelPos, entity.color, entity.label.c_str());

			int distInt = static_cast<int>(distance);
			std::string distanceStr = std::to_string(distInt) + "m";
			ImVec2 distSize = ImGui::CalcTextSize(distanceStr.c_str());
			ImVec2 distPos = { (topLeft.x + bottomRight.x) / 2 - distSize.x / 2, bottomRight.y + 2.f };
			drawList->AddText(distPos, entity.color, distanceStr.c_str());
		}
		else if (settings.bAzazelLine) {
			app::Vector3 camPos = uCamera.GetPosition();
			app::Vector3 toEntity = LuridLabMathLibrary::Vector3Utils::Normalize(LuridLabMathLibrary::Vector3Utils::Sub(entity.position, camPos));
			app::Vector3 camRight = LuridLabMathLibrary::Vector3Utils::Normalize(uCamera.GetRight());
			app::Vector3 camUp = LuridLabMathLibrary::Vector3Utils::Normalize(uCamera.GetUp());

			float rightDot = LuridLabMathLibrary::Vector3Utils::Dot(toEntity, camRight);
			float upDot = LuridLabMathLibrary::Vector3Utils::Dot(toEntity, camUp);

			ImVec2 center = ImVec2(io.DisplaySize.x / 2.f, io.DisplaySize.y / 2.f);
			ImVec2 offset = ImVec2(rightDot, -upDot);
			float len = io.DisplaySize.y * 0.45f;

			float mag = std::sqrt(offset.x * offset.x + offset.y * offset.y);
			if (mag > 0.01f) {
				offset.x = (offset.x / mag) * len;
				offset.y = (offset.y / mag) * len;

				ImVec2 end = ImVec2(center.x + offset.x, center.y + offset.y);
				drawList->AddLine(center, end, entity.color, 2.f);
				drawList->AddCircleFilled(end, 4.f, entity.color);

				// Mesafeyi çizgi ortasına yaz
				int distInt = static_cast<int>(distance);
				std::string text = entity.label + std::string(" - ") + std::to_string(distInt) + "m";

				ImVec2 mid = ImVec2((center.x + end.x) / 2.f, (center.y + end.y) / 2.f);
				ImVec2 dir = ImVec2(offset.y, -offset.x); // çizgiye dik yön
				float norm = std::sqrt(dir.x * dir.x + dir.y * dir.y);
				if (norm > 0.001f) {
					dir.x /= norm;
					dir.y /= norm;
				}

				ImVec2 offsetText = { dir.x * 14.f, dir.y * 14.f };
				ImVec2 textPos = { mid.x + offsetText.x, mid.y + offsetText.y };

				ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
				textPos.x -= textSize.x / 2.f;
				textPos.y -= textSize.y / 2.f;

				drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 180), text.c_str()); // shadow
				drawList->AddText(textPos, entity.color, text.c_str());
			}
		}
	}
}

const char* AzazelEsp::GetLabel() const {
	return "Azazel";
}

InteractableEsp::InteractableEsp() : valid(false), initialized(false) {}

void InteractableEsp::Update(app::Object_1* instance)
{
	if (!initialized)
	{
		UObject<app::ObjectInteractable> interactSearch;
		Il2cppData il2cppData;

		il2cppData.SetImage(ASSEMBLY_CSHARP);
		il2cppData.FindClass(NS_EMPTY, "ObjectInteractable");

		auto& rawObjects = interactSearch.FindAllObjects(il2cppData);
		if (rawObjects.empty()) return;

		std::vector<app::ObjectInteractable*> objects;
		for (auto* obj : rawObjects)
		{
			if (SafePtr::IsValid(reinterpret_cast<app::Object_1*>(obj)))
				objects.push_back(obj);
		}
		if (objects.empty()) return;

		static const std::unordered_map<std::string, std::pair<const char*, ImU32>> map =
		{
			// === ITEMS ===
			{ "SurvivalHay(Clone)",       { "Hay",       IM_COL32(80, 255, 80, 255) } },      
			{ "SurvivalGasoline(Clone)",  { "Gas",       IM_COL32(255, 120, 0, 255) } },      
			{ "SurvivalBattery(Clone)",   { "Battery",   IM_COL32(255, 255, 40, 255) } },     
			{ "SurvivalBleach(Clone)",    { "Bleach",    IM_COL32(255, 255, 60, 255) } },     
			{ "SurvivalFirstAid(Clone)",  { "FirstAid",  IM_COL32(255, 40, 40, 255) } },      
			{ "Key(Clone)",               { "Key",       IM_COL32(255, 230, 0, 255) } },      
			{ "SurvivalMatchbox(Clone)",  { "Matchbox",  IM_COL32(255, 160, 0, 255) } },      
			{ "SurvivalRottenFood(Clone)",{ "Food",      IM_COL32(200, 80, 0, 255) } },       
			{ "SurvivalFuse(Clone)",      { "Fuse",      IM_COL32(0, 200, 255, 255) } },      
			{ "SurvivalCake(Clone)",      { "Cake",      IM_COL32(255, 120, 200, 255) } },    
			{ "SurvivalRitualBook(Clone)",{ "Book",      IM_COL32(255, 60, 180, 255) } },     
			{ "SurvivalHead(Clone)",      { "Head",      IM_COL32(255, 180, 160, 255) } },    
			{ "SurvivalEgg(Clone)",       { "Egg",       IM_COL32(160, 160, 255, 255) } },    
			{ "SurvivalSpade(Clone)",     { "Spade",     IM_COL32(160, 160, 255, 255) } },    
			
			// === ANIMALS ===
			{ "SurvivalGoat(Clone)",      { "Goat",      IM_COL32(255, 60, 255, 255) } },     
			{ "SurvivalRat(Clone)",       { "Rat",       IM_COL32(180, 180, 200, 255) } },    
			{ "SurvivalPig(Clone)",       { "Pig",       IM_COL32(220, 150, 100, 255) } },    
			
			// === MAP OBJECTS
			// ===
			{ "SurvivalAltar",            { "Altar",     IM_COL32(80, 120, 255, 255) } },     
			{ "SurvivalAltarTown",        { "Altar",     IM_COL32(80, 120, 255, 255) } },
			{ "SlaughterhouseAltar",      { "Altar",     IM_COL32(80, 120, 255, 255) } },
			{ "Ritual Book Plop Interactable", { "RitualPlop", IM_COL32(160, 100, 255, 255) } },
			{ "Rat Plop Interactable",         { "RitualPlop", IM_COL32(160, 100, 255, 255) } },
			{ "Pig Plop Interactable",         { "RitualPlop", IM_COL32(160, 100, 255, 255) } },
			{ "InnShrine(Clone)",        { "Shrine",    IM_COL32(0, 180, 255, 255) } },      
			{ "InnFountain",             { "Fountain",  IM_COL32(80, 220, 255, 255) } },
			{ "InnFountain 2",           { "Fountain",  IM_COL32(80, 220, 255, 255) } },
			{ "ManorFountain",           { "Fountain",  IM_COL32(80, 220, 255, 255) } },
			{ "FuseBox",                 { "FuseBox",   IM_COL32(80, 220, 255, 255) } },
			
			// === HIDING SPOT ===
			{ "TownHidingSpot - General Store",   { "HidingSpot", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Clothing Store",  { "HidingSpot", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Weapons Store",   { "HidingSpot", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Storage",         { "HidingSpot", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Saloon (Lower)",  { "HidingSpot", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Saloon (Upper)",  { "HidingSpot", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Hotel (Upper) A", { "HidingSpot", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Hotel (Upper) C", { "HidingSpot", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Sheriff (Upper)",{ "HidingSpot", IM_COL32(160, 160, 160, 255) } },
			
			// === WARDROBES ===
			{ "InnWardrobe Room 1",      { "Wardrobe", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 2",      { "Wardrobe", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 3",      { "Wardrobe", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 4",      { "Wardrobe", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 5",      { "Wardrobe", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 6",      { "Wardrobe", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 7",      { "Wardrobe", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 8",      { "Wardrobe", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Dining Room", { "Wardrobe", IM_COL32(255, 230, 120, 255) } },
			
			// Misc
			{ "TownPentagramCandle 1",   { "Candle", IM_COL32(255, 220, 100, 255) } },
			{ "TownPentagramCandle 2",   { "Candle", IM_COL32(255, 220, 100, 255) } },
			{ "TownPentagramCandle 3",   { "Candle", IM_COL32(255, 220, 100, 255) } },
			{ "Shrine",                  { "Shrine", IM_COL32(0, 180, 255, 255) } },
			{ "Soil",                    { "Soil", IM_COL32(140, 90, 50, 255) } },          
			{ "Grave",                   { "Grave", IM_COL32(160, 160, 160, 255) } },
		};

		for (auto* obj : objects)
		{
			if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(obj))) continue;

			Il2CppObject* il2obj = reinterpret_cast<Il2CppObject*>(obj);
			if (!il2obj || !il2obj->klass) continue;

			app::GameObject* go = nullptr;
			try {
				go = app::Component_get_gameObject(reinterpret_cast<app::Component*>(obj), nullptr);
			}
			catch (...) {
				continue;
			}
			if (!SafePtr::IsValid(go)) continue;

			auto* tf = app::Component_get_transform(reinterpret_cast<app::Component*>(obj), nullptr);
			if (!tf) continue;

			std::string name = il2cppi_to_string(app::Object_1_get_name(reinterpret_cast<app::Object_1*>(go), nullptr));
			if (name.empty()) continue;

			EspEntity e;
			e.gameObject = go;
			e.transform = tf;
			e.headOffset = 0.f;
			e.footOffset = 0.f;

			static const std::unordered_set<std::string_view> blacklist = {
				"RitualPlop", "Wardrobe", "Candle", "Altar", "Fountain", "FuseBox", "Shrine", "HidingSpot", "Grave", "Soil"
			};


			auto it = map.find(name);
			if (it != map.end()) {
				e.label = it->second.first;
				e.color = it->second.second;
			}
			else {
				e.label = name;
				e.color = IM_COL32(255, 255, 255, 180);
			}

			if (blacklist.count(e.label) > 0) continue;

			cachedEntities.push_back(e);
		}

		initialized = true;
	}

	entities.clear();

	for (auto& e : cachedEntities)
	{
		if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(e.gameObject))) continue;
		if (!app::GameObject_get_activeInHierarchy(e.gameObject, nullptr)) continue;

		entities.push_back(e); 
	}

	valid = !entities.empty();
}

void InteractableEsp::Draw()
{
	if (!valid) return;

	ImGuiIO& io = ImGui::GetIO();
	auto drawList = ImGui::GetBackgroundDrawList();

	for (auto& entity : entities)
	{
		if (!entity.gameObject) {
			continue;
		}
		if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(entity.gameObject))) {
			continue;
		}

		entity.position = app::Transform_get_position(entity.transform, nullptr);
		float distance = uCamera.DistanceTo(entity.position);
		if (distance < 2.0f) continue;

		bool success = false;
		app::Vector3 screen = uCamera.WorldToScreen(entity.position, &success);
		if (!success || screen.z <= 0.f) continue;

		// (2m close = 255, 30m far = 0)
		float minDist = 1.5f;
		float maxDist = 100.0f;
		float t = std::clamp((distance - minDist) / (maxDist - minDist), 0.0f, 1.0f);
		int alpha = static_cast<int>((1.0f - t) * 255.0f);

		ImU32 fadedColor = (entity.color & IM_COL32_MASK_RGB) | IM_COL32_A(alpha);

		screen.y = io.DisplaySize.y - screen.y;

		ImVec2 textSize = ImGui::CalcTextSize(entity.label.c_str());
		ImVec2 textPos = { screen.x - textSize.x / 2.f, screen.y - textSize.y / 2.f };

		drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, alpha), entity.label.c_str());
		drawList->AddText(textPos, fadedColor, entity.label.c_str());
	}
}

void InteractableEsp::Reset()
{
	initialized = false;
	cachedEntities.clear();
	entities.clear();

	valid = false;
}

const char* InteractableEsp::GetLabel() const
{
	return "Interactables";
}


PlayerEsp::PlayerEsp() {}

void PlayerEsp::Update(app::Object_1* instance) {
	entities.clear();
	valid = false;

	if (!instance) return;

	auto* survival = reinterpret_cast<app::Survival*>(instance);
	if (!survival) return;

	auto* playerList = survival->fields.m_PlayerNolanBehaviours;
	if (!playerList || playerList->fields._size == 0) return;

	for (int i = 0; i < playerList->fields._size; ++i) {
		auto* nolan = playerList->fields._items->vector[i];
		if (!nolan || PlayerHelper::IsLocalPlayer(nolan)) continue;
		if (!app::Behaviour_get_enabled(reinterpret_cast<app::Behaviour*>(nolan), nullptr)) continue;

		app::GameObject* go = app::Component_get_gameObject(reinterpret_cast<app::Component*>(nolan), nullptr);
		app::Transform* tf = app::Component_get_transform(reinterpret_cast<app::Component*>(nolan), nullptr);
		if (!SafePtr::IsValid(go) || !tf) continue;

		app::Vector3 pos = app::Transform_get_position(tf, nullptr);

		EspEntity e;
		e.gameObject = go;
		e.transform = tf;
		e.position = pos;
		e.label = "Player";
		e.color = IM_COL32(80, 200, 255, 255);
		e.headOffset = 1.9f;
		e.footOffset = -0.2f;

		entities.push_back(e);
	}

	valid = !entities.empty();
}

void PlayerEsp::Draw() {
	if (!valid || entities.empty()) return;

	ImGuiIO& io = ImGui::GetIO();
	auto drawList = ImGui::GetBackgroundDrawList();

	for (auto& entity : entities) {
		if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(entity.gameObject))) continue;
		if (!entity.transform) continue;

		entity.position = app::Transform_get_position(entity.transform, nullptr);

		bool success = false;
		float distance = uCamera.DistanceTo(entity.position);
		if (distance < 1.5f) continue;

		app::Vector3 head = entity.position; head.y += entity.headOffset;
		app::Vector3 foot = entity.position; foot.y += entity.footOffset;

		app::Vector3 screenHead = uCamera.WorldToScreen(head, &success);
		app::Vector3 screenFoot = uCamera.WorldToScreen(foot, &success);

		auto IsOnScreen = [](const app::Vector3& screenPos, const ImVec2& screenSize) {
			return screenPos.x >= 0 && screenPos.x <= screenSize.x &&
				screenPos.y >= 0 && screenPos.y <= screenSize.y &&
				screenPos.z > 0;
			};

		bool isOnScreen = success && IsOnScreen(screenHead, io.DisplaySize) && IsOnScreen(screenFoot, io.DisplaySize);

		if (isOnScreen) {
			screenHead.y = io.DisplaySize.y - screenHead.y;
			screenFoot.y = io.DisplaySize.y - screenFoot.y;

			float height = screenFoot.y - screenHead.y;
			float width = height / 2.5f;

			ImVec2 topLeft = { screenHead.x - width / 2.f, screenHead.y };
			ImVec2 bottomRight = { screenHead.x + width / 2.f, screenFoot.y };

			// Box
			drawList->AddRect(topLeft, bottomRight, entity.color, 0, 0, 2.f);

			// Label
			ImVec2 labelSize = ImGui::CalcTextSize(entity.label.c_str());
			ImVec2 labelPos = { (topLeft.x + bottomRight.x) / 2.f - labelSize.x / 2.f, topLeft.y - labelSize.y - 2.f };
			drawList->AddText(ImVec2(labelPos.x + 1, labelPos.y + 1), IM_COL32(0, 0, 0, 180), entity.label.c_str());
			drawList->AddText(labelPos, entity.color, entity.label.c_str());

			// m
			std::string distStr = std::to_string((int)distance) + "m";
			ImVec2 distSize = ImGui::CalcTextSize(distStr.c_str());
			ImVec2 distPos = { (topLeft.x + bottomRight.x) / 2.f - distSize.x / 2.f, bottomRight.y + 2.f };
			drawList->AddText(ImVec2(distPos.x + 1, distPos.y + 1), IM_COL32(0, 0, 0, 180), distStr.c_str());
			drawList->AddText(distPos, entity.color, distStr.c_str());
		}
		else if (settings.bPlayerLine) {
			app::Vector3 camPos = uCamera.GetPosition();
			app::Vector3 toEntity = LuridLabMathLibrary::Vector3Utils::Normalize(entity.position - camPos);
			app::Vector3 camRight = uCamera.GetRight();
			app::Vector3 camUp = uCamera.GetUp();

			float rightDot = LuridLabMathLibrary::Vector3Utils::Dot(toEntity, camRight);
			float upDot = LuridLabMathLibrary::Vector3Utils::Dot(toEntity, camUp);

			ImVec2 center = ImVec2(io.DisplaySize.x / 2.f, io.DisplaySize.y / 2.f);
			ImVec2 offset = ImVec2(rightDot, -upDot);
			float len = io.DisplaySize.y * 0.4f;

			float mag = std::sqrt(offset.x * offset.x + offset.y * offset.y);
			if (mag > 0.01f) {
				offset.x = (offset.x / mag) * len;
				offset.y = (offset.y / mag) * len;

				ImVec2 end = ImVec2(center.x + offset.x, center.y + offset.y);
				drawList->AddLine(center, end, entity.color, 2.f);
				drawList->AddCircleFilled(end, 4.f, entity.color);

				std::string text = entity.label + std::string(" - ") + std::to_string((int)distance) + "m";

				ImVec2 mid = ImVec2((center.x + end.x) / 2.f, (center.y + end.y) / 2.f);
				ImVec2 dir = ImVec2(offset.y, -offset.x);
				float norm = std::sqrt(dir.x * dir.x + dir.y * dir.y);
				if (norm > 0.001f) {
					dir.x /= norm;
					dir.y /= norm;
				}
				ImVec2 offsetText = { dir.x * 14.f, dir.y * 14.f };
				ImVec2 textPos = { mid.x + offsetText.x, mid.y + offsetText.y };

				ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
				textPos.x -= textSize.x / 2.f;
				textPos.y -= textSize.y / 2.f;

				drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 180), text.c_str()); // shadow
				drawList->AddText(textPos, entity.color, text.c_str());
			}
		}
	}
}

const char* PlayerEsp::GetLabel() const {
	return "Players";
}

void PlayerEsp::Reset()
{
	entities.clear();
	valid = false;
}

AzazelEsp& ESPManager::GetAzazel() {
	static AzazelEsp azazelEsp;
	return azazelEsp;
}

InteractableEsp& ESPManager::GetInteractables()
{
	static InteractableEsp instance;
	return instance;
}

PlayerEsp& ESPManager::GetPlayers() {
	static PlayerEsp playerEsp;
	return playerEsp;
}


void ESPManager::DrawAll() {
	if (settings.bAzazelEsp)
	{
		GetAzazel().Draw();
	}

	if (settings.bInteractableEsp)
	{
		GetInteractables().Draw();
	}

	if (settings.bPlayerEsp)
	{
		GetPlayers().Draw();
	}
}

void ESPManager::ResetAll()
{
	GetAzazel().Update(nullptr);
	GetInteractables().Reset();
	GetPlayers().Update(nullptr);
	GetPlayers().Reset();
}
