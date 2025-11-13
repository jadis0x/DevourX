#include "pch-il2cpp.h"
#include "devour/devourbase.h"
#include "esp.h"
#include "helpers.h"
#include <algorithm>
#include <pipeline/settings.h>
#include <vector>
#include <player/player.h>
#include <unordered_map>
#include <imgui/imgui.h>
// 前向声明 ImFont
struct ImFont;

// 声明外部字体变量
namespace Menu {
	extern ImFont* chineseFont;
}

static UCamera uCamera;

AzazelEsp::AzazelEsp() : valid(false) {}

void AzazelEsp::Update(app::Object_1* instance) {
	valid = false;
	entities.clear();

	if (!instance || !SafePtr::IsValid(instance))
		return;

	auto* component = reinterpret_cast<app::Component*>(instance);
	if (!component)
		return;

	auto* behaviour = reinterpret_cast<app::SurvivalAzazelBehaviour*>(instance);
	app::GameObject* go = app::Component_get_gameObject(component, nullptr);
	app::Transform* tf = app::Component_get_transform(component, nullptr);

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
	e.label = std::string(u8"恶魔");
	e.color = IM_COL32(255, 0, 0, 255);

	entities.push_back(e);
	valid = true;
}

void AzazelEsp::Draw() {
	if (!valid) return;

	ImGuiIO& io = ImGui::GetIO();
	auto drawList = ImGui::GetBackgroundDrawList();

	// 应用中文字体
	if (Menu::chineseFont) {
		ImGui::PushFont(Menu::chineseFont);
	}

	for (const auto& entity : entities) {
		bool success = false;

		float distance = uCamera.DistanceTo(entity.position);
		if (distance < 0.1f || distance > 80.f) continue;

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
			std::string distanceStr = std::to_string(distInt) + u8"米";
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

				// 距离文本
				int distInt = static_cast<int>(distance);
				std::string text = entity.label + std::string(u8" - ") + std::to_string(distInt) + u8"米";

				ImVec2 mid = ImVec2((center.x + end.x) / 2.f, (center.y + end.y) / 2.f);
				ImVec2 dir = ImVec2(offset.y, -offset.x); // 垂直线条方向
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

				drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 180), text.c_str()); // 阴影
				drawList->AddText(textPos, entity.color, text.c_str());
			}
		}
	}

	// 弹出字体
	if (Menu::chineseFont) {
		ImGui::PopFont();
	}
}

const char* AzazelEsp::GetLabel() const {
	return u8"恶魔";
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
			// === 物品 ===
			{ "SurvivalHay(Clone)",       { u8"干草",       IM_COL32(80, 255, 80, 255) } },
			{ "SurvivalGasoline(Clone)",  { u8"汽油",       IM_COL32(255, 120, 0, 255) } },
			{ "SurvivalBattery(Clone)",   { u8"电池",   IM_COL32(255, 255, 40, 255) } },
			{ "SurvivalBleach(Clone)",    { u8"漂白剂",    IM_COL32(255, 255, 60, 255) } },
			{ "SurvivalFirstAid(Clone)",  { u8"急救包",  IM_COL32(255, 40, 40, 255) } },
			{ "Key(Clone)",               { u8"钥匙",       IM_COL32(255, 230, 0, 255) } },
			{ "SurvivalMatchbox(Clone)",  { u8"火柴盒",  IM_COL32(255, 160, 0, 255) } },
			{ "SurvivalRottenFood(Clone)",{ u8"食物",      IM_COL32(200, 80, 0, 255) } },
			{ "SurvivalFuse(Clone)",      { u8"保险丝",      IM_COL32(0, 200, 255, 255) } },
			{ "SurvivalCake(Clone)",      { u8"蛋糕",      IM_COL32(255, 120, 200, 255) } },
			{ "SurvivalRitualBook(Clone)",{ u8"仪式书",      IM_COL32(255, 60, 180, 255) } },
			{ "SurvivalHead(Clone)",      { u8"头颅",      IM_COL32(255, 180, 160, 255) } },
			{ "SurvivalEgg(Clone)",       { u8"蛋",       IM_COL32(160, 160, 255, 255) } },
			{ "SurvivalSpade(Clone)",     { u8"铲子",     IM_COL32(160, 160, 255, 255) } },
			{ "SurvivalCoin(Clone)",     { u8"金币",     IM_COL32(160, 160, 255, 255) } },
			{ "SurvivalMusicBox(Clone)",     { u8"音乐盒",     IM_COL32(160, 160, 255, 255) } },
			// === 动物 ===
			{ "SurvivalGoat(Clone)",      { u8"羊",      IM_COL32(255, 60, 255, 255) } },
			{ "SurvivalRat(Clone)",       { u8"老鼠",       IM_COL32(180, 180, 200, 255) } },
			{ "SurvivalPig(Clone)",       { u8"猪",       IM_COL32(220, 150, 100, 255) } },

			// === 地图对象 ===
			{ "Coin Slot",            { u8"投币处",     IM_COL32(80, 120, 255, 255) } },
			{ "DollHead Plop Interactable",            { u8"放头处",     IM_COL32(80, 120, 255, 255) } },
			{ "SurvivalAltar",            { u8"祭坛",     IM_COL32(80, 120, 255, 255) } },
			{ "SurvivalAltarTown",        { u8"祭坛",     IM_COL32(80, 120, 255, 255) } },
			{ "SlaughterhouseAltar",      { u8"祭坛",     IM_COL32(80, 120, 255, 255) } },
			{ "Ritual Book Plop Interactable", { u8"仪式点", IM_COL32(160, 100, 255, 255) } },
			{ "Rat Plop Interactable",         { u8"仪式点", IM_COL32(160, 100, 255, 255) } },
			{ "Pig Plop Interactable",         { u8"仪式点", IM_COL32(160, 100, 255, 255) } },
			{ "InnShrine(Clone)",        { u8"神龛",    IM_COL32(0, 180, 255, 255) } },
			{ "InnFountain",             { u8"喷泉",  IM_COL32(80, 220, 255, 255) } },
			{ "InnFountain 2",           { u8"喷泉",  IM_COL32(80, 220, 255, 255) } },
			{ "ManorFountain",           { u8"喷泉",  IM_COL32(80, 220, 255, 255) } },
			{ "FuseBox",                 { u8"保险盒",   IM_COL32(80, 220, 255, 255) } },

			// === 藏身处 ===
			{ "TownHidingSpot - General Store",   { u8"藏身处", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Clothing Store",  { u8"藏身处", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Weapons Store",   { u8"藏身处", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Storage",         { u8"藏身处", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Saloon (Lower)",  { u8"藏身处", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Saloon (Upper)",  { u8"藏身处", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Hotel (Upper) A", { u8"藏身处", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Hotel (Upper) C", { u8"藏身处", IM_COL32(160, 160, 160, 255) } },
			{ "TownHidingSpot - Sheriff (Upper)",{ u8"藏身处", IM_COL32(160, 160, 160, 255) } },

			// === 衣柜 ===
			{ "InnWardrobe Room 1",      { u8"衣柜", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 2",      { u8"衣柜", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 3",      { u8"衣柜", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 4",      { u8"衣柜", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 5",      { u8"衣柜", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 6",      { u8"衣柜", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 7",      { u8"衣柜", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Room 8",      { u8"衣柜", IM_COL32(255, 230, 120, 255) } },
			{ "InnWardrobe Dining Room", { u8"衣柜", IM_COL32(255, 230, 120, 255) } },

			// 杂项
			{ "TownPentagramCandle 1",   { u8"蜡烛", IM_COL32(255, 220, 100, 255) } },
			{ "TownPentagramCandle 2",   { u8"蜡烛", IM_COL32(255, 220, 100, 255) } },
			{ "TownPentagramCandle 3",   { u8"蜡烛", IM_COL32(255, 220, 100, 255) } },
			{ "Soil",                    { u8"土壤", IM_COL32(140, 90, 50, 255) } },
			{ "Grave",                   { u8"坟墓", IM_COL32(160, 160, 160, 255) } },

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
				u8"仪式点", u8"衣柜", u8"蜡烛", u8"祭坛", u8"喷泉", u8"保险盒", u8"神龛", u8"藏身处", u8"坟墓", u8"土壤"
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
		// ==================== 在这里添加神龛搜索 ====================
	// 添加神龛搜索
		UObject<app::InnShrineController> shrineSearch;
		Il2cppData shrineData;

		shrineData.SetImage(ASSEMBLY_CSHARP);
		shrineData.FindClass(NS_EMPTY, "InnShrineController");

		// 直接尝试搜索对象，不检查类是否存在
		auto& shrineControllers = shrineSearch.FindAllObjects(shrineData);
		if (!shrineControllers.empty()) {
			for (auto* shrineController : shrineControllers) {
				if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(shrineController))) continue;

				app::GameObject* shrineGO = app::Component_get_gameObject(
					reinterpret_cast<app::Component*>(shrineController), nullptr);
				if (!shrineGO) continue;

				app::Transform* shrineTF = app::Component_get_transform(
					reinterpret_cast<app::Component*>(shrineController), nullptr);
				if (!shrineTF) continue;

				bool isActive = app::GameObject_get_activeInHierarchy(shrineGO, nullptr);
				if (!isActive) continue;

				EspEntity e;
				e.gameObject = shrineGO;
				e.transform = shrineTF;
				e.headOffset = 0.f;
				e.footOffset = 0.f;
				e.label = u8"神龛";
				e.color = IM_COL32(80, 120, 255, 255); // 蓝色

				cachedEntities.push_back(e);
			}
		}
		// ==================== 神龛搜索结束 ====================
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

	// 应用中文字体
	if (Menu::chineseFont) {
		ImGui::PushFont(Menu::chineseFont);
	}

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
		if (distance < 0.1f) continue;

		bool success = false;
		app::Vector3 screen = uCamera.WorldToScreen(entity.position, &success);
		if (!success || screen.z <= 0.f) continue;

		// (2m close = 255, 30m far = 0)
		float minDist = 0.1f;
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

	// 弹出字体
	if (Menu::chineseFont) {
		ImGui::PopFont();
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
	return u8"可交互物品";
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
		if (!nolan || !SafePtr::IsValid(reinterpret_cast<app::Object_1*>(nolan)) || PlayerHelper::IsLocalPlayer(nolan))
			continue;

		app::GameObject* go = app::Component_get_gameObject(reinterpret_cast<app::Component*>(nolan), nullptr);
		app::Transform* tf = app::Component_get_transform(reinterpret_cast<app::Component*>(nolan), nullptr);

		// 添加安全检查
		if (!go || !tf || !SafePtr::IsValid(go) || !SafePtr::IsValid(reinterpret_cast<app::Object_1*>(tf)))
			continue;

		// 检查游戏对象是否仍然活跃
		if (!app::GameObject_get_activeInHierarchy(go, nullptr))
			continue;

		app::Vector3 pos = app::Transform_get_position(tf, nullptr);

		EspEntity e;
		e.gameObject = go;
		e.transform = tf;
		e.position = pos;
		e.label = u8"玩家";
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

	// 应用中文字体
	if (Menu::chineseFont) {
		ImGui::PushFont(Menu::chineseFont);
	}
	// 在绘制前再次验证实体有效性
	std::vector<EspEntity> validEntities;
	for (auto& entity : entities) {
		if (SafePtr::IsValid(reinterpret_cast<app::Object_1*>(entity.gameObject)) &&
			entity.transform &&
			app::GameObject_get_activeInHierarchy(entity.gameObject, nullptr)) {
			validEntities.push_back(entity);
		}
	}

	// 更新实体列表
	if (validEntities.size() != entities.size()) {
		entities = validEntities;
		valid = !entities.empty();
		if (!valid) {
			if (Menu::chineseFont) ImGui::PopFont();
			return;
		}
	}
	for (auto& entity : entities) {
		if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(entity.gameObject))) continue;
		if (!entity.transform) continue;

		entity.position = app::Transform_get_position(entity.transform, nullptr);

		bool success = false;
		float distance = uCamera.DistanceTo(entity.position);
		if (distance < 0.1f) continue;

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

			// 方框
			drawList->AddRect(topLeft, bottomRight, entity.color, 0, 0, 2.f);

			// 标签
			ImVec2 labelSize = ImGui::CalcTextSize(entity.label.c_str());
			ImVec2 labelPos = { (topLeft.x + bottomRight.x) / 2.f - labelSize.x / 2.f, topLeft.y - labelSize.y - 2.f };
			drawList->AddText(ImVec2(labelPos.x + 1, labelPos.y + 1), IM_COL32(0, 0, 0, 180), entity.label.c_str());
			drawList->AddText(labelPos, entity.color, entity.label.c_str());

			// 距离
			std::string distStr = std::to_string((int)distance) + u8"米";
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

				std::string text = entity.label + std::string(u8" - ") + std::to_string((int)distance) + u8"米";

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

				drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 180), text.c_str()); // 阴影
				drawList->AddText(textPos, entity.color, text.c_str());
			}
		}
	}

	// 弹出字体
	if (Menu::chineseFont) {
		ImGui::PopFont();
	}
}

const char* PlayerEsp::GetLabel() const {
	return u8"玩家";
}



	SimpleDemonEsp::SimpleDemonEsp() : valid(false), initialized(false), frameCount(0), updateInterval(600) {}

	void SimpleDemonEsp::Update(app::Object_1* instance) {
		frameCount++;

		// 降低更新频率：不是每帧都更新
		if (frameCount % updateInterval != 0 && initialized) {
			// 使用缓存的实体，只更新位置
			UpdateCachedEntities();
			return;
		}

		valid = false;
		entities.clear();

		if (!initialized) {
			InitializeEntities();
			initialized = true;
		}
		else {
			UpdateCachedEntities();
		}

		valid = !entities.empty();
	}

	void SimpleDemonEsp::InitializeEntities() {
		cachedEntities.clear();

		// 获取当前场景名称
		std::string scene = Base::Gameplay::GetSceneName();
		bool isTown = (scene == "Town");
		bool isDevour = (scene == "Devour");
		bool isMolly = (scene == "Molly");
		bool isInn = (scene == "Inn");
		bool isSlaughterhouse = (scene == "Slaughterhouse");
		bool isManor = (scene == "Manor");
		bool isCarnival = (scene == "Carnival");

		// 根据场景确定要搜索的实体类型
		std::vector<std::pair<const char*, const char*>> entityTypes;

		if (isDevour || isMolly) {
			// Devour 或 Molly 场景：只搜索恶魔和仪式品
			entityTypes = {
				{"SurvivalDemonBehaviour", u8"恶魔"},
				{"GoatBehaviour", u8"仪式品"}
			};
		}
		else if (isInn) {
			// Inn 场景：只搜索蜘蛛和仪式品
			entityTypes = {
				{"SpiderBehaviour", u8"蜘蛛"},
			};
		}
		else if (isTown) {
			// Town 场景：只搜索幽灵和仪式品
			entityTypes = {
				{"GhostBehaviour", u8"幽灵"},
			};
		}
		else if (isSlaughterhouse) {
			// Slaughterhouse 场景：只搜索野猪、尸体和仪式品
			entityTypes = {
				{"BoarBehaviour", u8"野猪"},
				{"CorpseBehaviour", u8"尸体"},
				{"GoatBehaviour", u8"仪式品"}
			};
		}
		else if (isManor) {
			// Manor 场景：只搜索乌鸦和仪式品
			entityTypes = {
				{"CrowBehaviour", u8"乌鸦"},
				{"ManorLumpController", u8"肿块"},
				{"GoatBehaviour", u8"仪式品"}
			};
		}
		else if (isCarnival) {
			// Carnival 场景：只搜索猴子和仪式品
			entityTypes = {
				{"MonkeyBehaviour", u8"猴子"},
				{"GoatBehaviour", u8"仪式品"}
			};
		}


		// 搜索实体
		for (const auto& [className, label] : entityTypes) {
			UObject<app::Object_1> entitySearch;
			Il2cppData entityData;

			entityData.SetImage(ASSEMBLY_CSHARP);
			entityData.FindClass(NS_EMPTY, className);

			auto& objects = entitySearch.FindAllObjects(entityData);
			for (auto* obj : objects) {
				if (!SafePtr::IsValid(obj)) continue;

				app::GameObject* go = app::Component_get_gameObject(reinterpret_cast<app::Component*>(obj), nullptr);
				app::Transform* tf = app::Component_get_transform(reinterpret_cast<app::Component*>(obj), nullptr);

				if (!go || !tf) continue;
				if (!app::GameObject_get_activeInHierarchy(go, nullptr)) continue;

				EspEntity e;
				e.gameObject = go;
				e.transform = tf;
				e.position = app::Transform_get_position(tf, nullptr);
				e.label = label;

				if (strcmp(className, "GoatBehaviour") == 0) {
					e.color = IM_COL32(80, 120, 255, 255); // 仪式品用蓝色
				}
				else {
					e.color = IM_COL32(255, 0, 0, 255); // 其他实体用红色
				}

				cachedEntities.push_back(e);
			}
		}
		UpdateCachedEntities();
	}

	// 幽灵持书状态管理
	void SimpleDemonEsp::SetGhostCarryingBook(app::GameObject* ghostGO, bool carrying)
	{
		m_GhostBookState[ghostGO] = carrying;
	}

	bool SimpleDemonEsp::IsGhostCarryingBook(app::GameObject* ghostGO)
	{
		auto it = m_GhostBookState.find(ghostGO);
		return it != m_GhostBookState.end() && it->second;
	}

	// 乌鸦持头状态管理
	void SimpleDemonEsp::SetCrowCarryingHead(app::GameObject* crowGO, bool carrying)
	{
		m_CrowHeadState[crowGO] = carrying;
	}

	bool SimpleDemonEsp::IsCrowCarryingHead(app::GameObject* crowGO)
	{
		auto it = m_CrowHeadState.find(crowGO);
		return it != m_CrowHeadState.end() && it->second;
	}

	// 清理所有无效引用
	void SimpleDemonEsp::CleanupAllReferences()
	{
		// 清理蜘蛛引用
		std::vector<app::GameObject*> spidersToRemove;
		for (auto& pair : m_SpiderEggState) {
			if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(pair.first))) {
				spidersToRemove.push_back(pair.first);
			}
		}
		for (auto* go : spidersToRemove) {
			m_SpiderEggState.erase(go);
		}

		// 清理幽灵引用
		std::vector<app::GameObject*> ghostsToRemove;
		for (auto& pair : m_GhostBookState) {
			if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(pair.first))) {
				ghostsToRemove.push_back(pair.first);
			}
		}
		for (auto* go : ghostsToRemove) {
			m_GhostBookState.erase(go);
		}

		// 清理乌鸦引用
		std::vector<app::GameObject*> crowsToRemove;
		for (auto& pair : m_CrowHeadState) {
			if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(pair.first))) {
				crowsToRemove.push_back(pair.first);
			}
		}
		for (auto* go : crowsToRemove) {
			m_CrowHeadState.erase(go);
		}
	}

	void SimpleDemonEsp::UpdateCachedEntities() {
		entities.clear();
		// 清理所有无效引用
		CleanupAllReferences();
		for (auto& e : cachedEntities) {
			if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(e.gameObject))) continue;
			if (!app::GameObject_get_activeInHierarchy(e.gameObject, nullptr)) continue;

			// 更新位置
			e.position = app::Transform_get_position(e.transform, nullptr);
			entities.push_back(e);
		}
	}

	void SimpleDemonEsp::Reset() {
		initialized = false;
		cachedEntities.clear();
		entities.clear();
		//m_SpiderEggState.clear(); // 重置蜘蛛持蛋状态
		//m_GhostBookState.clear();    // 重置幽灵持书状态
		//m_CrowHeadState.clear();     // 重置乌鸦持头状态
		valid = false;
		frameCount = 0;
	}


	// 设置蜘蛛持蛋状态
	void SimpleDemonEsp::SetSpiderCarryingEgg(app::GameObject* spiderGO, bool carrying)
	{
		m_SpiderEggState[spiderGO] = carrying;
	}

	// 检查蜘蛛是否拿着蛋
	bool SimpleDemonEsp::IsSpiderCarryingEgg(app::GameObject* spiderGO)
	{
		auto it = m_SpiderEggState.find(spiderGO);
		return it != m_SpiderEggState.end() && it->second;
	}


	void SimpleDemonEsp::Draw() {
		if (!valid) return;

		ImGuiIO& io = ImGui::GetIO();
		auto drawList = ImGui::GetBackgroundDrawList();

		// 应用中文字体
		if (Menu::chineseFont) {
			ImGui::PushFont(Menu::chineseFont);
		}

		for (auto& entity : entities) {
			if (!entity.gameObject) continue;
			if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(entity.gameObject))) continue;

			entity.position = app::Transform_get_position(entity.transform, nullptr);
			float distance = uCamera.DistanceTo(entity.position);

			// 距离过滤
			if (distance < 0.1f || distance > 50.0f) continue;

			bool success = false;
			app::Vector3 screen = uCamera.WorldToScreen(entity.position, &success);
			if (!success || screen.z <= 0.f) continue;

			// 距离淡出效果
			float minDist = 0.1f;
			float maxDist = 50.0f;
			float t = std::clamp((distance - minDist) / (maxDist - minDist), 0.0f, 1.0f);
			int alpha = static_cast<int>((1.0f - t) * 255.0f);

			// 根据生物类型检查状态
			std::string displayLabel = entity.label;
			ImU32 displayColor = entity.color;

			if (entity.label == u8"蜘蛛") {
				if (IsSpiderCarryingEgg(entity.gameObject)) {
					displayLabel = u8"蜘蛛(持蛋)";
					displayColor = IM_COL32(255, 165, 0, alpha); // 橙色
				}
			}
			else if (entity.label == u8"幽灵") {
				if (IsGhostCarryingBook(entity.gameObject)) {
					displayLabel = u8"幽灵(持书)";
					displayColor = IM_COL32(0, 255, 255, alpha); // 青色
				}
			}
			else if (entity.label == u8"乌鸦") {
				if (IsCrowCarryingHead(entity.gameObject)) {
					displayLabel = u8"乌鸦(持头)";
					displayColor = IM_COL32(255, 0, 255, alpha); // 紫色
				}
			}

			// 应用颜色（如果状态没有改变，使用原始颜色）
			if (displayLabel == entity.label) {
				displayColor = (entity.color & IM_COL32_MASK_RGB) | IM_COL32_A(alpha);
			}

			screen.y = io.DisplaySize.y - screen.y;

			ImVec2 textSize = ImGui::CalcTextSize(displayLabel.c_str());
			ImVec2 textPos = { screen.x - textSize.x / 2.f, screen.y - textSize.y / 2.f };

			// 绘制文字阴影
			//drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, alpha), displayLabel.c_str());
			// 绘制文字
			drawList->AddText(textPos, displayColor, displayLabel.c_str());
		}

		// 弹出字体
		if (Menu::chineseFont) {
			ImGui::PopFont();
		}
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

// === 在这里添加 SimpleDemonEsp 相关的方法 ===

SimpleDemonEsp& ESPManager::GetSimpleDemons() {
	static SimpleDemonEsp simpleDemonEsp;
	return simpleDemonEsp;
}

void ESPManager::DrawAll() {
	if (settings.bAzazelEsp) {
		GetAzazel().Draw();
	}

	if (settings.bInteractableEsp) {
		GetInteractables().Draw();
	}

	if (settings.bPlayerEsp) {
		GetPlayers().Draw();
	}

	if (settings.bSimpleDemonEsp) {
		GetSimpleDemons().Draw();
	}
}

void ESPManager::ResetAll() {
	GetAzazel().Update(nullptr);
	GetInteractables().Reset();
	GetPlayers().Reset();  // 先重置玩家
	GetPlayers().Update(nullptr);  // 然后更新
	GetSimpleDemons().Reset();
	GetSimpleDemons().Update(nullptr);  // 添加这行以保持一致性
}