#include "pch-il2cpp.h"

#include "UnityEngine/GameObject.h"

#include "wrapper.h"
#include <helpers.h>
#include <iostream>

app::Component* GameObject::GetComponentByName(app::GameObject* go, const char* type)
{
	app::Component* component = app::GameObject_GetComponentByName(go, ConvertToSystemString(type), nullptr);

	return component ? component : nullptr;
}

app::Component__Array* GameObject::LogComponents(app::GameObject* go)
{
    if (go == nullptr) {
        std::cout << "GameObject is nullptr!" << std::endl;
        return nullptr;
    }

    // Wrapper objesi oluþturuluyor, DLL ve sýnýf kontrol ediliyor
    Wrapper obj("UnityEngine.CoreModule.dll");

    Il2CppObject* object = obj.find_class("UnityEngine", "Component").get_class();

    if (!object) {
        std::cout << "Failed to find UnityEngine.Component class!" << std::endl;
        return nullptr;
    }

    // app::Type türüne cast yapýlýyor, ardýndan nullptr kontrolü
    app::Type* type = reinterpret_cast<app::Type*>(object);
    if (!type) {
        std::cout << "Failed to cast object to app::Type!" << std::endl;
        return nullptr;
    }

    // Komponentler alýnýyor
    app::Component__Array* __components = app::GameObject_GetComponents(go, type, nullptr);
    if (!__components) {
        std::cout << "No components found on GameObject!" << std::endl;
        return nullptr;
    }

    // Komponentlerin içinde geçerli olanlarý döndür
    for (size_t i = 0; i < __components->max_length; ++i) {
        if (__components->vector[i] == nullptr) {
            std::cout << "Found nullptr component in array!" << std::endl;
        }
    }

    return __components;
}
