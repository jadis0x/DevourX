#pragma once

#include "resolver/il2cpp_resolver.h"
#include <imgui/imgui.h>


// ENTITY LIST
// Object -> GoatBehaviour
// Object -> SurvivalInteractable
// Object -> KeyBehaviour
// Object -> SurvivalDemonBehaviour
// Object -> SpiderBehaviour
// Object -> GhostBehaviour
// Object -> BoarBehaviour
// Object -> SurvivalAzazelBehaviour

#define IM_COL32_A_MASK      0xFF000000
#define IM_COL32_MASK_RGB    0x00FFFFFF
#define IM_COL32_A(a)        ((ImU32)((a) << 24))

class UGameObject;

struct EspEntity {
    app::GameObject* gameObject;
    app::Transform* transform;
    app::Vector3 position;

    float headOffset = 1.9f;
    float footOffset = -0.25f;
    std::string label;
    ImU32 color = IM_COL32(255, 0, 0, 255);
};

class BaseEsp {
public:
    virtual void Update(app::Object_1* instance) = 0;
    virtual void Draw() = 0;
    virtual const char* GetLabel() const = 0;
    virtual ~BaseEsp() {}
};

class AzazelEsp : public BaseEsp {
private:
    std::vector<EspEntity> entities;
    bool valid;

public:
    AzazelEsp();
    void Update(app::Object_1* instance) override;
    void Draw() override;
    const char* GetLabel() const override;
};

class InteractableEsp {
public:
    InteractableEsp();
    void Update(app::Object_1* instance);
    void Draw();
    void Reset();
    const char* GetLabel() const;

private:
    bool valid;
    std::vector<EspEntity> entities;
    std::vector<EspEntity> cachedEntities;
    bool initialized = false;
};


class PlayerEsp : public BaseEsp {
private:
    std::vector<EspEntity> entities;
    bool valid = false;

public:
    PlayerEsp();
    void Update(app::Object_1* instance) override;
    void Draw() override;
    void Reset();
    const char* GetLabel() const override;
};

class ESPManager {
public:
    static AzazelEsp& GetAzazel();
    static InteractableEsp& GetInteractables();
    static PlayerEsp& GetPlayers();

    static void DrawAll();
    static void ResetAll();
};

