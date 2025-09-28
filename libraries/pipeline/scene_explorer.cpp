#include "pch-il2cpp.h"

#include "scene_explorer.h"

#include "helpers.h"
#include "resolver/il2cpp_resolver.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace SceneExplorer {
namespace {
        constexpr size_t kMaxValuePreviewLength = 256;

        std::string Indent(int depth)
        {
                return std::string(static_cast<size_t>(depth) * 2U, ' ');
        }

        std::string Sanitize(const std::string& value)
        {
                std::string cleaned = value;
                cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '\r'), cleaned.end());
                std::replace(cleaned.begin(), cleaned.end(), '\n', ' ');
                if (cleaned.size() > kMaxValuePreviewLength)
                {
                        cleaned.resize(kMaxValuePreviewLength);
                        cleaned.append("...");
                }
                return cleaned;
        }

        std::string FormatTypeName(const Il2CppType* type)
        {
                if (!type)
                {
                        return "<unknown>";
                }

                std::string result = "<unknown>";
                if (char* raw = il2cpp_type_get_name(type))
                {
                        result.assign(raw);
                        il2cpp_free(raw);
                }
                return result;
        }

        std::string FormatClassName(Il2CppClass* klass)
        {
                if (!klass)
                {
                        return "<unknown>";
                }

                const char* ns = il2cpp_class_get_namespace(klass);
                const char* name = il2cpp_class_get_name(klass);

                std::string formatted;
                if (ns && *ns)
                {
                        formatted.append(ns);
                        formatted.append("::");
                }

                if (name && *name)
                {
                        formatted.append(name);
                }
                else
                {
                        formatted.append("<unnamed>");
                }

                return formatted;
        }

        std::string FormatAccessModifiers(int32_t flags)
        {
                std::vector<std::string> parts;

                const auto accessMask = static_cast<int32_t>(app::FieldAttributes__Enum::FieldAccessMask);
                const auto access = static_cast<app::FieldAttributes__Enum>(flags & accessMask);
                switch (access)
                {
                case app::FieldAttributes__Enum::Private: parts.emplace_back("private"); break;
                case app::FieldAttributes__Enum::Public: parts.emplace_back("public"); break;
                case app::FieldAttributes__Enum::Family: parts.emplace_back("protected"); break;
                case app::FieldAttributes__Enum::Assembly: parts.emplace_back("internal"); break;
                case app::FieldAttributes__Enum::FamORAssem: parts.emplace_back("protected internal"); break;
                case app::FieldAttributes__Enum::FamANDAssem: parts.emplace_back("private protected"); break;
                default: parts.emplace_back("private"); break;
                }

                if ((flags & static_cast<int32_t>(app::FieldAttributes__Enum::Static)) != 0)
                {
                        parts.emplace_back("static");
                }

                if ((flags & static_cast<int32_t>(app::FieldAttributes__Enum::InitOnly)) != 0)
                {
                        parts.emplace_back("readonly");
                }

                if ((flags & static_cast<int32_t>(app::FieldAttributes__Enum::Literal)) != 0)
                {
                        parts.emplace_back("const");
                }

                std::ostringstream oss;
                for (size_t i = 0; i < parts.size(); ++i)
                {
                        if (i > 0)
                        {
                                oss << ' ';
                        }
                        oss << parts[i];
                }

                return oss.str();
        }

        std::string ValueToString(Il2CppObject* value)
        {
                if (!value)
                {
                        return "null";
                }

                std::string result = "<error>";
                const bool ok = SafePtr::safe_call([&]() {
                        app::Object* unityObject = reinterpret_cast<app::Object*>(value);
                        if (auto* str = app::Object_ToString(unityObject, nullptr))
                        {
                                result = il2cppi_to_string(str);
                        }
                        else
                        {
                                result = "<null string>";
                        }
                });

                if (!ok)
                {
                        return "<exception>";
                }

                return Sanitize(result);
        }

        std::string ExtractFieldValue(FieldInfo* field, app::Component* instance, bool isStatic)
        {
                if (!field)
                {
                        return "<invalid field>";
                }

                std::string result = "<unavailable>";
                const bool ok = SafePtr::safe_call([&]() {
                        Il2CppObject* boxed = il2cpp_field_get_value_object(field,
                                isStatic ? nullptr : reinterpret_cast<Il2CppObject*>(instance));
                        result = ValueToString(boxed);
                });

                if (!ok)
                {
                        result = "<exception>";
                }

                return result;
        }

        ComponentEntry BuildComponentEntry(app::Component* component)
        {
                ComponentEntry entry;
                if (!component || !component->klass)
                {
                        entry.typeName = "<invalid component>";
                        return entry;
                }

                entry.typeName = FormatClassName(reinterpret_cast<Il2CppClass*>(component->klass));

                void* iter = nullptr;
                while (FieldInfo* field = il2cpp_class_get_fields(reinterpret_cast<Il2CppClass*>(component->klass), &iter))
                {
                        FieldEntry fieldEntry;
                        if (const char* rawName = il2cpp_field_get_name(field))
                        {
                                fieldEntry.name = rawName;
                        }
                        else
                        {
                                fieldEntry.name = "<unnamed>";
                        }
                        fieldEntry.typeName = FormatTypeName(il2cpp_field_get_type(field));
                        const int32_t flags = il2cpp_field_get_flags(field);
                        fieldEntry.modifiers = FormatAccessModifiers(flags);
                        const bool isStatic = (flags & static_cast<int32_t>(app::FieldAttributes__Enum::Static)) != 0;
                        fieldEntry.value = ExtractFieldValue(field, component, isStatic);

                        entry.fields.emplace_back(std::move(fieldEntry));
                }

                return entry;
        }

        GameObjectEntry BuildGameObjectEntry(app::Transform* transform)
        {
                GameObjectEntry entry;
                if (!transform || !SafePtr::IsValid(reinterpret_cast<app::Object_1*>(transform)))
                {
                        entry.name = "<invalid transform>";
                        return entry;
                }

                app::GameObject* gameObject = app::Component_get_gameObject(reinterpret_cast<app::Component*>(transform), nullptr);
                if (!SafePtr::IsValid(gameObject))
                {
                        entry.name = "<missing game object>";
                        return entry;
                }

                if (app::String* name = app::Object_1_get_name(reinterpret_cast<app::Object_1*>(gameObject), nullptr))
                {
                        entry.name = il2cppi_to_string(name);
                }
                else
                {
                        entry.name = "<unnamed>";
                }

                if (app::Object__Array* components = app::GameObject_GetComponents_2(gameObject, nullptr))
                {
                        for (il2cpp_array_size_t i = 0; i < components->max_length; ++i)
                        {
                                app::Object* raw = components->vector[i];
                                if (!raw)
                                {
                                        continue;
                                }

                                auto* component = reinterpret_cast<app::Component*>(raw);
                                if (!component || !component->klass)
                                {
                                        continue;
                                }

                                entry.components.emplace_back(BuildComponentEntry(component));
                        }
                }

                const int32_t childCount = app::Transform_get_childCount(transform, nullptr);
                for (int32_t i = 0; i < childCount; ++i)
                {
                        app::Transform* child = app::Transform_GetChild(transform, i, nullptr);
                        if (!child)
                        {
                                continue;
                        }
                        entry.children.emplace_back(BuildGameObjectEntry(child));
                }

                return entry;
        }

        void LogComponent(const ComponentEntry& component, int depth)
        {
                il2cppi_log_write(Indent(depth) + "Component: " + component.typeName);
                for (const FieldEntry& field : component.fields)
                {
                        std::string line = Indent(depth + 1);
                        if (!field.modifiers.empty())
                        {
                                line += field.modifiers + ' ';
                        }
                        line += field.typeName + ' ' + field.name + " = " + field.value;
                        il2cppi_log_write(std::move(line));
                }
        }

        void LogGameObjectRecursive(const GameObjectEntry& entry, int depth)
        {
                il2cppi_log_write(Indent(depth) + "GameObject: " + entry.name);
                for (const ComponentEntry& component : entry.components)
                {
                        LogComponent(component, depth + 1);
                }

                for (const GameObjectEntry& child : entry.children)
                {
                        LogGameObjectRecursive(child, depth + 1);
                }
        }
}

Snapshot CaptureActiveScene()
{
        Snapshot snapshot;

        Il2CppDomain* domain = il2cpp_domain_get();
        if (!domain)
        {
                il2cppi_log_write("[SceneExplorer] Failed to resolve IL2CPP domain.");
                return snapshot;
        }

        Il2CppThread* thread = il2cpp_thread_attach(domain);
        if (!thread)
        {
                il2cppi_log_write("[SceneExplorer] Failed to attach IL2CPP thread.");
                return snapshot;
        }

        auto detachThread = std::unique_ptr<Il2CppThread, void(*)(Il2CppThread*)>(thread, [](Il2CppThread* th) {
                if (th)
                {
                        il2cpp_thread_detach(th);
                }
        });

        app::Scene scene = app::SceneManager_GetActiveScene(nullptr);
        if (app::String* name = app::Scene_get_name(&scene, nullptr))
        {
                snapshot.sceneName = il2cppi_to_string(name);
        }
        else
        {
                snapshot.sceneName = "<unnamed scene>";
        }

        const auto now = std::chrono::system_clock::now();
        const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm timeInfo{};
        localtime_s(&timeInfo, &nowTime);
        std::ostringstream timeStream;
        timeStream << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
        snapshot.timestamp = timeStream.str();

        if (app::GameObject__Array* roots = app::Scene_GetRootGameObjects(&scene, nullptr))
        {
                for (il2cpp_array_size_t i = 0; i < roots->max_length; ++i)
                {
                        app::GameObject* root = roots->vector[i];
                        if (!SafePtr::IsValid(root))
                        {
                                continue;
                        }

                        app::Transform* transform = app::GameObject_get_transform(root, nullptr);
                        if (!transform)
                        {
                                continue;
                        }

                        snapshot.roots.emplace_back(BuildGameObjectEntry(transform));
                }
        }

        snapshot.valid = true;
        return snapshot;
}

void LogSnapshot(const Snapshot& snapshot)
{
        if (!snapshot.valid)
        {
                il2cppi_log_write("[SceneExplorer] No valid snapshot to log.");
                return;
        }

        il2cppi_log_write("[SceneExplorer] Scene Snapshot - " + snapshot.sceneName + " (" + snapshot.timestamp + ")");
        for (const GameObjectEntry& root : snapshot.roots)
        {
                LogGameObjectRecursive(root, 1);
        }
}

} // namespace SceneExplorer
