#include "pch-il2cpp.h"
#include "pipeline/gui/widgets.h"
#include "pipeline/gui/tabs/ItemTab.h"
#include <devour/devourbase.h>
#include <imgui/imgui.h>
#include <iostream>
#include <array>

// 前向声明 ImFont
struct ImFont;

// 声明外部字体变量
namespace Menu {
    extern ImFont* chineseFont;
}

// 预制体定义结构
struct PrefabDefinition {
    const char* displayName; // 显示的中文名
    const char* label;       // 实际标签
    app::PrefabId app::BoltPrefabs__StaticFields::* field; // 字段指针
};

// 预制体列表 - 根据您提供的列表翻译
const PrefabDefinition kPrefabs[] = {
    {u8"生成祭坛", "Survival Altar", &app::BoltPrefabs__StaticFields::SurvivalAltar},
    {u8"生成祭坛-嘉年华", "Survival Altar Carnival", &app::BoltPrefabs__StaticFields::SurvivalAltarCarnival},
    {u8"生成祭坛-莫莉", "Survival Altar Molly", &app::BoltPrefabs__StaticFields::SurvivalAltarMolly},
    {u8"生成祭坛-屠宰场", "Survival Altar Slaughterhouse", &app::BoltPrefabs__StaticFields::SurvivalAltarSlaughterhouse},
    {u8"生成祭坛-城镇", "Survival Altar Town", &app::BoltPrefabs__StaticFields::SurvivalAltarTown},
    {u8"笼子", "Cage", &app::BoltPrefabs__StaticFields::Cage},
    {u8"垃圾桶", "Trash Can", &app::BoltPrefabs__StaticFields::TrashCan},
    {u8"钥匙", "Key", &app::BoltPrefabs__StaticFields::Key},
    {u8"生成电池", "Survival Battery", &app::BoltPrefabs__StaticFields::SurvivalBattery},
    {u8"生成漂白剂", "Survival Bleach", &app::BoltPrefabs__StaticFields::SurvivalBleach},
    {u8"生成骨头", "Survival Bone", &app::BoltPrefabs__StaticFields::SurvivalBone},
    {u8"生成蛋糕", "Survival Cake", &app::BoltPrefabs__StaticFields::SurvivalCake},
    {u8"生成硬币", "Survival Coin", &app::BoltPrefabs__StaticFields::SurvivalCoin},
    {u8"生成玩偶", "Survival Doll", &app::BoltPrefabs__StaticFields::SurvivalDoll},
    {u8"生成玩偶头", "Survival Doll Head", &app::BoltPrefabs__StaticFields::SurvivalDollHead},
    {u8"生成蛋", "Survival Egg", &app::BoltPrefabs__StaticFields::SurvivalEgg},
    {u8"生成急救包", "Survival First Aid", &app::BoltPrefabs__StaticFields::SurvivalFirstAid},
    {u8"生成保险丝", "Survival Fuse", &app::BoltPrefabs__StaticFields::SurvivalFuse},
    {u8"生成汽油", "Survival Gasoline", &app::BoltPrefabs__StaticFields::SurvivalGasoline},
    {u8"生成山羊", "Survival Goat", &app::BoltPrefabs__StaticFields::SurvivalGoat},
    {u8"生成干草", "Survival Hay", &app::BoltPrefabs__StaticFields::SurvivalHay},
    {u8"生成头颅", "Survival Head", &app::BoltPrefabs__StaticFields::SurvivalHead},
    {u8"生成火柴盒", "Survival Matchbox", &app::BoltPrefabs__StaticFields::SurvivalMatchbox},
    {u8"生成音乐盒", "Survival Music Box", &app::BoltPrefabs__StaticFields::SurvivalMusicBox},
    {u8"生成猪", "Survival Pig", &app::BoltPrefabs__StaticFields::SurvivalPig},
    {u8"生成老鼠", "Survival Rat", &app::BoltPrefabs__StaticFields::SurvivalRat},
    {u8"生成仪式书", "Survival Ritual Book", &app::BoltPrefabs__StaticFields::SurvivalRitualBook},
    {u8"生成玫瑰", "Survival Rose", &app::BoltPrefabs__StaticFields::SurvivalRose},
    {u8"生成腐烂食物", "Survival Rotten Food", &app::BoltPrefabs__StaticFields::SurvivalRottenFood},
    {u8"生成铲子", "Survival Spade", &app::BoltPrefabs__StaticFields::SurvivalSpade},
    {u8"宠物猫", "Pet Cat", &app::BoltPrefabs__StaticFields::PetCat},
    {u8"宠物乌鸦", "Pet Crow", &app::BoltPrefabs__StaticFields::PetCrow},
    {u8"宠物山羊", "Pet Goat", &app::BoltPrefabs__StaticFields::PetGoat},
    {u8"宠物猪", "Pet Pig", &app::BoltPrefabs__StaticFields::PetPig},
    {u8"宠物哈巴狗", "Pet Pug", &app::BoltPrefabs__StaticFields::PetPug},
    {u8"宠物老鼠", "Pet Rat", &app::BoltPrefabs__StaticFields::PetRat},
    {u8"宠物蜘蛛", "Pet Spider", &app::BoltPrefabs__StaticFields::PetSpider},
    {u8"生成-莫莉", "Survival Azazel Molly", &app::BoltPrefabs__StaticFields::SurvivalAzazelMolly},
    {u8"生成囚犯", "Survival Inmate", &app::BoltPrefabs__StaticFields::SurvivalInmate},
    {u8"凯", "Azazel Kai", &app::BoltPrefabs__StaticFields::AzazelKai},
    {u8"猴子", "Monkey", &app::BoltPrefabs__StaticFields::Monkey},
    {u8"生成安娜(新)", "Survival Anna New", &app::BoltPrefabs__StaticFields::SurvivalAnnaNew},
    {u8"生成恶魔", "Survival Demon", &app::BoltPrefabs__StaticFields::SurvivalDemon},
    {u8"扎拉", "Azazel Zara", &app::BoltPrefabs__StaticFields::AzazelZara},
    {u8"蜘蛛", "Spider", &app::BoltPrefabs__StaticFields::Spider},
    {u8"艾普丽尔", "Azazel April", &app::BoltPrefabs__StaticFields::AzazelApril},
    {u8"乌鸦", "Crow", &app::BoltPrefabs__StaticFields::Crow},
    {u8"内森", "Azazel Nathan", &app::BoltPrefabs__StaticFields::AzazelNathan},
    {u8"野猪", "Boar", &app::BoltPrefabs__StaticFields::Boar},
    {u8"尸体", "Corpse", &app::BoltPrefabs__StaticFields::Corpse},
    {u8"萨姆", "Azazel Sam", &app::BoltPrefabs__StaticFields::AzazelSam},
    {u8"鬼魂", "Ghost", &app::BoltPrefabs__StaticFields::Ghost},
    {u8"生成安娜", "Survival Anna", &app::BoltPrefabs__StaticFields::SurvivalAnna},
    {u8"生成艾普丽尔", "Survival April", &app::BoltPrefabs__StaticFields::SurvivalApril},
    {u8"生成邪教徒", "Survival Cultist", &app::BoltPrefabs__StaticFields::SurvivalCultist},
    {u8"生成弗兰克", "Survival Frank", &app::BoltPrefabs__StaticFields::SurvivalFrank},
    {u8"生成凯", "Survival Kai", &app::BoltPrefabs__StaticFields::SurvivalKai},
    {u8"生成莫莉", "Survival Molly", &app::BoltPrefabs__StaticFields::SurvivalMolly},
    {u8"生成内森", "Survival Nathan", &app::BoltPrefabs__StaticFields::SurvivalNathan},
    {u8"生成萨姆", "Survival Sam", &app::BoltPrefabs__StaticFields::SurvivalSam},
    {u8"生成扎拉", "Survival Zara", &app::BoltPrefabs__StaticFields::SurvivalZara},
    {u8"精神病院门", "Asylum Door", &app::BoltPrefabs__StaticFields::AsylumDoor},
    {u8"精神病院双开门", "Asylum Double Door", &app::BoltPrefabs__StaticFields::AsylumDoubleDoor},
    {u8"精神病院白门", "Asylum White Door", &app::BoltPrefabs__StaticFields::AsylumWhiteDoor},
    {u8"电梯门", "Elevator Door", &app::BoltPrefabs__StaticFields::Elevator_Door},
    {u8"嘉年华隧道门", "Carnival Tunnel Door", &app::BoltPrefabs__StaticFields::CarnivalTunnelDoor},
    {u8"吞噬后门", "Devour Door Back", &app::BoltPrefabs__StaticFields::DevourDoorBack},
    {u8"吞噬主门", "Devour Door Main", &app::BoltPrefabs__StaticFields::DevourDoorMain},
    {u8"吞噬房间门", "Devour Door Room", &app::BoltPrefabs__StaticFields::DevourDoorRoom},
    {u8"旅馆门", "Inn Door", &app::BoltPrefabs__StaticFields::InnDoor},
    {u8"旅馆双开门", "Inn Double Door", &app::BoltPrefabs__StaticFields::InnDoubleDoor},
    {u8"旅馆障子门", "Inn Shoji Door", &app::BoltPrefabs__StaticFields::InnShojiDoor},
    {u8"旅馆障子双开门", "Inn Shoji Double Door", &app::BoltPrefabs__StaticFields::InnShojiDoubleDoor},
    {u8"旅馆木门", "Inn Wooden Door", &app::BoltPrefabs__StaticFields::InnWoodenDoor},
    {u8"旅馆木双开门", "Inn Wooden Double Door", &app::BoltPrefabs__StaticFields::InnWoodenDoubleDoor},
    {u8"庄园谷仓门", "Manor Barn Door", &app::BoltPrefabs__StaticFields::ManorBarnDoor},
    {u8"庄园门", "Manor Door", &app::BoltPrefabs__StaticFields::ManorDoor},
    {u8"庄园大门", "Manor Gate", &app::BoltPrefabs__StaticFields::ManorGate},
    {u8"庄园陵墓门", "Manor Mausoleum Door", &app::BoltPrefabs__StaticFields::ManorMausoleumDoor},
    {u8"动物门", "Animal Gate", &app::BoltPrefabs__StaticFields::Animal_Gate},
    {u8"屠宰场消防逃生门", "Slaughterhouse Fire Escape Door", &app::BoltPrefabs__StaticFields::SlaughterhouseFireEscapeDoor},
    {u8"屠宰场办公室门", "Slaughterhouse Office Door", &app::BoltPrefabs__StaticFields::SlaughterhouseOfficeDoor},
    {u8"屠宰场推拉门", "Slaughterhouse Push Pull Door", &app::BoltPrefabs__StaticFields::SlaughterhousePushPullDoor},
    {u8"城镇牢房门", "Town Cell Door", &app::BoltPrefabs__StaticFields::TownCellDoor},
    {u8"城镇门", "Town Door", &app::BoltPrefabs__StaticFields::TownDoor},
    {u8"城镇门2", "Town Door 2", &app::BoltPrefabs__StaticFields::TownDoor2},
    {u8"城镇锁箱", "Town Lockbox", &app::BoltPrefabs__StaticFields::TownLockbox},
    {u8"生成爆裂气球", "Survival Popping Balloon", &app::BoltPrefabs__StaticFields::SurvivalPoppingBalloon},
    {u8"无法到达的房间", "Unreachable Room", &app::BoltPrefabs__StaticFields::UnreachableRoom},
    {u8"旅馆喷泉", "Inn Fountain", &app::BoltPrefabs__StaticFields::InnFountain},
    {u8"旅馆神社", "Inn Shrine", &app::BoltPrefabs__StaticFields::InnShrine},
    {u8"旅馆衣柜", "Inn Wardrobe", &app::BoltPrefabs__StaticFields::InnWardrobe},
    {u8"旅馆蜘蛛网", "Inn Web", &app::BoltPrefabs__StaticFields::InnWeb},
    {u8"电视", "TV", &app::BoltPrefabs__StaticFields::TV},
    {u8"庄园喷泉", "Manor Fountain", &app::BoltPrefabs__StaticFields::ManorFountain},
    {u8"庄园藏身点", "Manor Hiding Spot", &app::BoltPrefabs__StaticFields::ManorHidingSpot},
    {u8"庄园肿块", "Manor Lump", &app::BoltPrefabs__StaticFields::ManorLump},
    {u8"庄园镜子", "Manor Mirror", &app::BoltPrefabs__StaticFields::ManorMirror},
    {u8"庄园神社", "Manor Shrine", &app::BoltPrefabs__StaticFields::ManorShrine},
    {u8"冷冻室", "Freezer Room", &app::BoltPrefabs__StaticFields::Freezer_Room},
    {u8"猪粪便", "Pig Excrement", &app::BoltPrefabs__StaticFields::PigExcrement},
    {u8"生成爆裂管道", "Survival Burst Pipe", &app::BoltPrefabs__StaticFields::SurvivalBurstPipe},
    {u8"卡车卷帘门", "Truck Shutter", &app::BoltPrefabs__StaticFields::Truck_Shutter},
    {u8"生成可破坏窗户", "Survival Smashable Window", &app::BoltPrefabs__StaticFields::SurvivalSmashableWindow},
    {u8"生成可破坏窗户(多重)", "Survival Smashable Window Multi", &app::BoltPrefabs__StaticFields::SurvivalSmashableWindowMulti},
    {u8"城镇五角星", "Town Pentagram", &app::BoltPrefabs__StaticFields::TownPentagram},
    {u8"生成火柴盒02", "Survival Matchbox 02", &app::BoltPrefabs__StaticFields::SurvivalMatchbox_02}
};

void ItemTAB::Render()
{
    if (Menu::chineseFont) {
        ImGui::PushFont(Menu::chineseFont);
    }

    if (ImGui::BeginTabItem(u8"物品"))
    {
        ImGui::Spacing();
        ImGui::Text(u8"物品");

        // 定义物品结构体：显示名称和实际名称
        struct Item {
            const char* displayName; // 显示的中文名
            const char* actualName;  // 传递的英文名
        };

        Item items[] = {
            {u8"干草", "Hay"},
            {u8"急救包", "First aid"},
            {u8"铲子", "Spade"},
            {u8"蛋糕", "Cake"},
            {u8"电池", "Battery"},
            {u8"汽油", "Gasoline"},
            {u8"保险丝", "Fuse"},
            {u8"食物", "Food"},
            {u8"漂白剂", "Bleach"},
            {u8"骨头", "Bone"},
            {u8"盐", "Salt"},
            {u8"仪式书 (未激活)", "Ritual Book (in-active)"},
            {u8"仪式书 (已激活)", "Ritual Book (active)"},
            {u8"火柴盒", "Matchbox"},
            {u8"蛋-1", "Egg-1"},
            {u8"蛋-2", "Egg-2"},
            {u8"蛋-3", "Egg-3"},
            {u8"蛋-4", "Egg-4"},
            {u8"蛋-5", "Egg-5"},
            {u8"蛋-6", "Egg-6"},
            {u8"蛋-7", "Egg-7"},
            {u8"蛋-8", "Egg-8"},
            {u8"蛋-9", "Egg-9"},
            {u8"蛋-10", "Egg-10"},
            {u8"头颅 (肮脏/园丁)", "Head (Dirty/Gardener)"},
            {u8"头颅 (干净/园丁)", "Head (Clean/Gardener)"},
            {u8"头颅 (肮脏/管家)", "Head (Dirty/Butler)"},
            {u8"头颅 (干净/管家)", "Head (Clean/Butler)"},
            {u8"头颅 (肮脏/伴娘_01)", "Head (Dirty/Bridesmaid_01)"},
            {u8"头颅 (干净/伴娘_01)", "Head (Clean/Bridesmaid_01)"},
            {u8"头颅 (肮脏/伴娘_02)", "Head (Dirty/Bridesmaid_02)"},
            {u8"头颅 (干净/伴娘_02)", "Head (Clean/Bridesmaid_02)"},
            {u8"头颅 (肮脏/厨师)", "Head (Dirty/Cook)"},
            {u8"头颅 (干净/厨师)", "Head (Clean/Cook)"},
            {u8"头颅 (肮脏/伴郎_01)", "Head (Dirty/Groomsman_01)"},
            {u8"头颅 (干净/伴郎_01)", "Head (Clean/Groomsman_01)"},
            {u8"头颅 (肮脏/伴郎_02)", "Head (Dirty/Groomsman_02)"},
            {u8"头颅 (干净/伴郎_02)", "Head (Clean/Groomsman_02)"},
            {u8"头颅 (肮脏/女仆)", "Head (Dirty/Maid)"},
            {u8"头颅 (干净/女仆)", "Head (Clean/Maid)"},
            {u8"头颅 (肮脏/摄影师)", "Head (Dirty/Photographer)"},
            {u8"头颅 (干净/摄影师)", "Head (Clean/Photographer)"},
            {u8"头颅 (肮脏/牧师)", "Head (Dirty/Priest)"},
            {u8"头颅 (干净/牧师)", "Head (Clean/Priest)"},
            {u8"硬币", "Coin"},
            {u8"音乐盒 (闲置)", "Music Box (Idle)"},
            {u8"音乐盒 (已上弦)", "Music Box (Armed)"},
            {u8"玩偶头", "SurvivalDollHead"}
        };

        static int item_current = 0;

        // 创建只包含显示名称的数组用于Combo
        const char* display_names[IM_ARRAYSIZE(items)];
        for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
            display_names[i] = items[i].displayName;
        }

        ImGui::Combo("##i", &item_current, display_names, IM_ARRAYSIZE(items));
        if (ImGui::Button(u8"生成##i")) {
            Base::Gameplay::StartCarryItem(items[item_current].actualName);
        }

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Text(u8"动物");
        GuiWidgets::HelpMarker(u8"在生成动物前 必须先生成一次物品的东西 否则会造成闪退");
        struct Animal {
            const char* displayName;
            const char* actualName;
        };

        Animal animals[] = {
            {u8"老鼠", "Rat"},
            {u8"山羊", "Goat"},
            {u8"猪", "Pig"},
        };

        static int animal_current = 0;

        const char* animal_display_names[IM_ARRAYSIZE(animals)];
        for (int i = 0; i < IM_ARRAYSIZE(animals); i++) {
            animal_display_names[i] = animals[i].displayName;
        }

        ImGui::Combo("##an", &animal_current, animal_display_names, IM_ARRAYSIZE(animals));
        if (ImGui::Button(u8"生成##an")) {
            Base::Gameplay::StartCarryAnimal(animals[animal_current].actualName);
        }

        ImGui::Spacing();
        ImGui::Spacing();

        // 添加预制体生成部分
        ImGui::Text(u8"预制体");

        static int prefab_current = 0;

        // 创建只包含显示名称的数组用于Combo
        const char* prefab_display_names[IM_ARRAYSIZE(kPrefabs)];
        for (int i = 0; i < IM_ARRAYSIZE(kPrefabs); i++) {
            prefab_display_names[i] = kPrefabs[i].displayName;
        }

        ImGui::Combo("##pf", &prefab_current, prefab_display_names, IM_ARRAYSIZE(kPrefabs));
        if (ImGui::Button(u8"生成##pf")) {
            Base::Gameplay::InstantiatePrefab(
                kPrefabs[prefab_current].label,
                kPrefabs[prefab_current].field);
        }

        if (Menu::chineseFont) {
            ImGui::PopFont();
        }
        ImGui::EndTabItem();
    }
    else
    {
        if (Menu::chineseFont) {
            ImGui::PopFont();
        }
    }
}