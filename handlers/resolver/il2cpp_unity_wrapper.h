#pragma once

#include <cstdint>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include "il2cpp-appdata.h"

namespace il2cppx {
    namespace detail {

        inline const Il2CppImage* find_image(const char* assembly_name) {
            auto* domain = il2cpp_domain_get();
            if (!domain) {
                throw std::runtime_error("il2cpp_domain_get returned null");
            }

            size_t assembly_count = 0;
            const Il2CppAssembly* const* assemblies = il2cpp_domain_get_assemblies(domain, &assembly_count);
            if (!assemblies) {
                throw std::runtime_error("Failed to enumerate IL2CPP assemblies");
            }

            for (size_t i = 0; i < assembly_count; ++i) {
                auto* image = il2cpp_assembly_get_image(assemblies[i]);
                if (!image) {
                    continue;
                }
                const char* image_name = il2cpp_image_get_name(image);
                if (image_name && std::strcmp(image_name, assembly_name) == 0) {
                    return image;
                }
            }

            return nullptr;
        }

        inline const Il2CppImage* get_image_cached(const char* assembly_name) {
            static std::mutex mutex;
            static std::unordered_map<std::string, const Il2CppImage*> cache;

            {
                std::lock_guard<std::mutex> lock(mutex);
                auto it = cache.find(assembly_name);
                if (it != cache.end()) {
                    return it->second;
                }
            }

            const Il2CppImage* image = find_image(assembly_name);

            std::lock_guard<std::mutex> lock(mutex);
            cache.emplace(assembly_name, image);
            return image;
        }

        inline Il2CppClass* get_class(const char* assembly_name, const char* namesp, const char* name) {
            const Il2CppImage* image = get_image_cached(assembly_name);
            if (!image) {
                throw std::runtime_error("Failed to locate IL2CPP image: " + std::string(assembly_name));
            }

            Il2CppClass* klass = il2cpp_class_from_name(image, namesp, name);
            if (!klass) {
                throw std::runtime_error("Failed to locate IL2CPP class: " + std::string(namesp) + "." + name);
            }

            return klass;
        }

        inline const MethodInfo* get_method(Il2CppClass* klass, const char* name, int arg_count) {
            const MethodInfo* method = il2cpp_class_get_method_from_name(klass, name, arg_count);
            if (!method) {
                throw std::runtime_error("Failed to locate IL2CPP method: " + std::string(name));
            }
            return method;
        }

        inline std::string to_utf8(Il2CppString* str) {
            if (!str) {
                return {};
            }

            const Il2CppChar* chars = il2cpp_string_chars(str);
            const int length = il2cpp_string_length(str);
            std::string result;
            result.reserve(static_cast<size_t>(length));

            for (int i = 0; i < length; ++i) {
                uint32_t codepoint = chars[i];

                if (codepoint >= 0xD800 && codepoint <= 0xDBFF && i + 1 < length) {
                    uint32_t low = chars[i + 1];
                    if (low >= 0xDC00 && low <= 0xDFFF) {
                        codepoint = 0x10000 + (((codepoint - 0xD800) << 10) | (low - 0xDC00));
                        ++i;
                    }
                }
                else if (codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
                    continue;
                }

                if (codepoint <= 0x7F) {
                    result.push_back(static_cast<char>(codepoint));
                }
                else if (codepoint <= 0x7FF) {
                    result.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
                    result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                }
                else if (codepoint <= 0xFFFF) {
                    result.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
                    result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                }
                else {
                    result.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
                    result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                }
            }

            return result;
        }

        inline Il2CppObject* runtime_invoke(const MethodInfo* method, void* instance, void** args) {
            Il2CppException* exception = nullptr;
            Il2CppObject* result = il2cpp_runtime_invoke(method, instance, args, &exception);
            if (exception) {
                throw std::runtime_error("Managed exception thrown while invoking method");
            }
            return result;
        }

        inline Il2CppString* make_string(const std::string& value) {
            return il2cpp_string_new_len(value.data(), static_cast<uint32_t>(value.size()));
        }

        inline Il2CppReflectionType* make_reflection_type(Il2CppClass* klass) {
            const Il2CppType* type = il2cpp_class_get_type(klass);
            if (!type) {
                throw std::runtime_error("Failed to obtain Il2CppType");
            }
            Il2CppObject* obj = il2cpp_type_get_object(type);
            if (!obj) {
                throw std::runtime_error("il2cpp_type_get_object returned null");
            }
            return reinterpret_cast<Il2CppReflectionType*>(obj);
        }

        template <typename T>
        inline T unbox_value(Il2CppObject* value) {
            if (!value) {
                return T{};
            }
            void* data = il2cpp_object_unbox(value);
            if (!data) {
                T result{};
                std::memcpy(&result, value, sizeof(T));
                return result;
            }
            T result{};
            std::memcpy(&result, data, sizeof(T));
            return result;
        }

        inline bool unbox_bool(Il2CppObject* value) {
            return unbox_value<int32_t>(value) != 0;
        }

        inline int32_t unbox_int(Il2CppObject* value) {
            return unbox_value<int32_t>(value);
        }

        inline float unbox_float(Il2CppObject* value) {
            return unbox_value<float>(value);
        }

        inline Il2CppClass* unity_object_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Object");
            return klass;
        }

        inline Il2CppClass* component_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Component");
            return klass;
        }

        inline Il2CppClass* transform_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Transform");
            return klass;
        }

        inline Il2CppClass* game_object_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject");
            return klass;
        }

        inline Il2CppClass* behaviour_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Behaviour");
            return klass;
        }

        inline Il2CppClass* renderer_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Renderer");
            return klass;
        }

        inline Il2CppClass* mesh_renderer_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "MeshRenderer");
            return klass;
        }

        inline Il2CppClass* light_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Light");
            return klass;
        }

        inline Il2CppClass* camera_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Camera");
            return klass;
        }

        inline Il2CppClass* rigidbody_class() {
            static Il2CppClass* klass = get_class("UnityEngine.PhysicsModule.dll", "UnityEngine", "Rigidbody");
            return klass;
        }

        inline Il2CppClass* collider_class() {
            static Il2CppClass* klass = get_class("UnityEngine.PhysicsModule.dll", "UnityEngine", "Collider");
            return klass;
        }

        inline Il2CppClass* animator_class() {
            static Il2CppClass* klass = get_class("UnityEngine.AnimationModule.dll", "UnityEngine", "Animator");
            return klass;
        }

        inline Il2CppClass* audio_source_class() {
            static Il2CppClass* klass = get_class("UnityEngine.AudioModule.dll", "UnityEngine", "AudioSource");
            return klass;
        }

        inline Il2CppClass* animator_state_info_class() {
            static Il2CppClass* klass = get_class("UnityEngine.AnimationModule.dll", "UnityEngine", "AnimatorStateInfo");
            return klass;
        }

        inline Il2CppClass* rect_transform_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "RectTransform");
            return klass;
        }

        inline Il2CppClass* vector2_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Vector2");
            return klass;
        }

        inline Il2CppClass* canvas_class() {
            static Il2CppClass* klass = get_class("UnityEngine.UI.dll", "UnityEngine", "Canvas");
            return klass;
        }

        inline Il2CppClass* character_controller_class() {
            static Il2CppClass* klass = get_class("UnityEngine.PhysicsModule.dll", "UnityEngine", "CharacterController");
            return klass;
        }

        inline Il2CppClass* nav_mesh_agent_class() {
            static Il2CppClass* klass = get_class("UnityEngine.AIModule.dll", "UnityEngine.AI", "NavMeshAgent");
            return klass;
        }

        inline Il2CppClass* material_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Material");
            return klass;
        }

        inline Il2CppClass* color_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Color");
            return klass;
        }

        inline Il2CppClass* vector3_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Vector3");
            return klass;
        }

        inline Il2CppClass* quaternion_class() {
            static Il2CppClass* klass = get_class("UnityEngine.CoreModule.dll", "UnityEngine", "Quaternion");
            return klass;
        }

    } // namespace detail

    namespace math {

        struct Vector2 {
            float x = 0.f;
            float y = 0.f;
        };

        struct Vector3 {
            float x = 0.f;
            float y = 0.f;
            float z = 0.f;
        };

        struct Quaternion {
            float x = 0.f;
            float y = 0.f;
            float z = 0.f;
            float w = 1.f;
        };

        struct Color {
            float r = 0.f;
            float g = 0.f;
            float b = 0.f;
            float a = 1.f;
        };

        inline Vector2 to_vector2(Il2CppObject* object) {
            return detail::unbox_value<Vector2>(object);
        }

        inline Vector3 to_vector3(Il2CppObject* object) {
            return detail::unbox_value<Vector3>(object);
        }

        inline Quaternion to_quaternion(Il2CppObject* object) {
            return detail::unbox_value<Quaternion>(object);
        }

        inline Color to_color(Il2CppObject* object) {
            return detail::unbox_value<Color>(object);
        }

        inline Il2CppObject* box(const Vector2& value) {
            return il2cpp_value_box(detail::vector2_class(), const_cast<Vector2*>(&value));
        }

        inline Il2CppObject* box(const Vector3& value) {
            return il2cpp_value_box(detail::vector3_class(), const_cast<Vector3*>(&value));
        }

        inline Il2CppObject* box(const Quaternion& value) {
            return il2cpp_value_box(detail::quaternion_class(), const_cast<Quaternion*>(&value));
        }

        inline Il2CppObject* box(const Color& value) {
            return il2cpp_value_box(detail::color_class(), const_cast<Color*>(&value));
        }

    } // namespace math

    class GcHandle {
    public:
        GcHandle() = default;

        explicit GcHandle(Il2CppObject* object, bool pinned = false) {
            reset(object, pinned);
        }

        ~GcHandle() {
            reset();
        }

        GcHandle(const GcHandle&) = delete;
        GcHandle& operator=(const GcHandle&) = delete;

        GcHandle(GcHandle&& other) noexcept { move_from(std::move(other)); }

        GcHandle& operator=(GcHandle&& other) noexcept {
            if (this != &other) {
                reset();
                move_from(std::move(other));
            }
            return *this;
        }

        void reset(Il2CppObject* object = nullptr, bool pinned = false) {
            if (handle_ != 0) {
                il2cpp_gchandle_free(handle_);
                handle_ = 0;
            }
            if (object) {
                handle_ = il2cpp_gchandle_new(object, pinned ? 1 : 0);
            }
        }

        Il2CppObject* get() const {
            return handle_ ? il2cpp_gchandle_get_target(handle_) : nullptr;
        }

        explicit operator bool() const { return handle_ != 0; }

    private:
        void move_from(GcHandle&& other) noexcept {
            handle_ = other.handle_;
            other.handle_ = 0;
        }

        uint32_t handle_ = 0;
    };

    class Object {
    public:
        Object() = default;
        explicit Object(Il2CppObject* instance) : handle_(instance) {}

        Il2CppObject* get() const { return handle_.get(); }
        Il2CppClass* klass() const { return get() ? il2cpp_object_get_class(get()) : nullptr; }
        bool valid() const { return static_cast<bool>(handle_); }
        explicit operator bool() const { return valid(); }

        std::string name() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::unity_object_class(), "get_name", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::to_utf8(reinterpret_cast<Il2CppString*>(result));
        }

    protected:
        GcHandle handle_;
    };

    class GameObject;
    class Transform;

    class Component : public Object {
    public:
        Component() = default;
        explicit Component(Il2CppObject* instance) : Object(instance) {}

        GameObject game_object() const;
        Transform transform() const;

        Component get_component(Il2CppClass* klass) const;
        Component get_component_in_children(Il2CppClass* klass, bool include_inactive = false) const;
        Component get_component_in_parent(Il2CppClass* klass, bool include_inactive = false) const;

        template <typename Wrapper>
        Wrapper as() const {
            return Wrapper(get());
        }
    };

    class Transform : public Component {
    public:
        Transform() = default;
        explicit Transform(Il2CppObject* instance) : Component(instance) {}

        Transform find(const std::string& path) const {
            if (!valid()) {
                return Transform{};
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "Find", 1);
            Il2CppString* name_str = detail::make_string(path);
            void* args[1] = { name_str };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return Transform(result);
        }

        math::Vector3 position() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "get_position", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector3(result);
        }

        void set_position(const math::Vector3& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "set_position", 1);
            math::Vector3 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector3 local_position() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "get_localPosition", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector3(result);
        }

        void set_local_position(const math::Vector3& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "set_localPosition", 1);
            math::Vector3 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        math::Quaternion rotation() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "get_rotation", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_quaternion(result);
        }

        void set_rotation(const math::Quaternion& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "set_rotation", 1);
            math::Quaternion copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector3 local_scale() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "get_localScale", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector3(result);
        }

        void set_local_scale(const math::Vector3& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "set_localScale", 1);
            math::Vector3 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        Transform parent() const {
            if (!valid()) {
                return Transform{};
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "get_parent", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return Transform(result);
        }

        void set_parent(const Transform& value, bool world_position_stays = true) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "SetParent", 2);
            Il2CppObject* parent_obj = value.get();
            int32_t wps = world_position_stays ? 1 : 0;
            void* args[2] = { parent_obj, &wps };
            detail::runtime_invoke(method, get(), args);
        }

        int32_t child_count() const {
            if (!valid()) {
                return 0;
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "get_childCount", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_int(result);
        }

        Transform get_child(int32_t index) const {
            if (!valid()) {
                return Transform{};
            }
            static const MethodInfo* method = detail::get_method(detail::transform_class(), "GetChild", 1);
            void* args[1] = { &index };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return Transform(result);
        }
    };

    class RectTransform : public Transform {
    public:
        RectTransform() = default;
        explicit RectTransform(Il2CppObject* instance) : Transform(instance) {}

        math::Vector2 anchored_position() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "get_anchoredPosition", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector2(result);
        }

        void set_anchored_position(const math::Vector2& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "set_anchoredPosition", 1);
            math::Vector2 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector2 size_delta() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "get_sizeDelta", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector2(result);
        }

        void set_size_delta(const math::Vector2& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "set_sizeDelta", 1);
            math::Vector2 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector2 anchor_min() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "get_anchorMin", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector2(result);
        }

        void set_anchor_min(const math::Vector2& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "set_anchorMin", 1);
            math::Vector2 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector2 anchor_max() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "get_anchorMax", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector2(result);
        }

        void set_anchor_max(const math::Vector2& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "set_anchorMax", 1);
            math::Vector2 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector2 pivot() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "get_pivot", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector2(result);
        }

        void set_pivot(const math::Vector2& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rect_transform_class(), "set_pivot", 1);
            math::Vector2 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }
    };

    class GameObject : public Object {
    public:
        GameObject() = default;
        explicit GameObject(Il2CppObject* instance) : Object(instance) {}

        static GameObject find(const std::string& name) {
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "Find", 1);
            Il2CppString* name_str = detail::make_string(name);
            void* args[1] = { name_str };
            Il2CppObject* result = detail::runtime_invoke(method, nullptr, args);
            return GameObject(result);
        }

        static GameObject find_with_tag(const std::string& name) {
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "FindWithTag", 1);
            Il2CppString* name_str = detail::make_string(name);
            void* args[1] = { name_str };
            Il2CppObject* result = detail::runtime_invoke(method, nullptr, args);
            return GameObject(result);
        }

        Transform transform() const {
            if (!valid()) {
                return Transform{};
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "get_transform", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return Transform(result);
        }

        Component add_component(Il2CppClass* klass) const {
            if (!valid()) {
                return Component{};
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "AddComponent", 1);
            Il2CppReflectionType* reflection_type = detail::make_reflection_type(klass);
            void* args[1] = { reflection_type };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return Component(result);
        }

        Component get_component(Il2CppClass* klass) const {
            if (!valid()) {
                return Component{};
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "GetComponent", 1);
            Il2CppReflectionType* reflection_type = detail::make_reflection_type(klass);
            void* args[1] = { reflection_type };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return Component(result);
        }

        Component get_component_in_children(Il2CppClass* klass, bool include_inactive = false) const {
            if (!valid()) {
                return Component{};
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "GetComponentInChildren", 2);
            Il2CppReflectionType* reflection_type = detail::make_reflection_type(klass);
            int32_t include = include_inactive ? 1 : 0;
            void* args[2] = { reflection_type, &include };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return Component(result);
        }

        Component get_component_in_parent(Il2CppClass* klass, bool include_inactive = false) const {
            if (!valid()) {
                return Component{};
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "GetComponentInParent", 2);
            Il2CppReflectionType* reflection_type = detail::make_reflection_type(klass);
            int32_t include = include_inactive ? 1 : 0;
            void* args[2] = { reflection_type, &include };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return Component(result);
        }

        void set_active(bool value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "SetActive", 1);
            int32_t active = value ? 1 : 0;
            void* args[1] = { &active };
            detail::runtime_invoke(method, get(), args);
        }

        bool active_self() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "get_activeSelf", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        bool active_in_hierarchy() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "get_activeInHierarchy", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        int32_t layer() const {
            if (!valid()) {
                return 0;
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "get_layer", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_int(result);
        }

        void set_layer(int32_t value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "set_layer", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        std::string tag() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "get_tag", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::to_utf8(reinterpret_cast<Il2CppString*>(result));
        }

        void set_tag(const std::string& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::game_object_class(), "set_tag", 1);
            Il2CppString* tag_str = detail::make_string(value);
            void* args[1] = { tag_str };
            detail::runtime_invoke(method, get(), args);
        }
    };

    inline GameObject Component::game_object() const {
        if (!valid()) {
            return GameObject{};
        }
        static const MethodInfo* method = detail::get_method(detail::component_class(), "get_gameObject", 0);
        Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
        return GameObject(result);
    }

    inline Transform Component::transform() const {
        if (!valid()) {
            return Transform{};
        }
        static const MethodInfo* method = detail::get_method(detail::component_class(), "get_transform", 0);
        Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
        return Transform(result);
    }

    inline Component Component::get_component(Il2CppClass* klass) const {
        if (!valid()) {
            return Component{};
        }
        static const MethodInfo* method = detail::get_method(detail::component_class(), "GetComponent", 1);
        Il2CppReflectionType* reflection_type = detail::make_reflection_type(klass);
        void* args[1] = { reflection_type };
        Il2CppObject* result = detail::runtime_invoke(method, get(), args);
        return Component(result);
    }

    inline Component Component::get_component_in_children(Il2CppClass* klass, bool include_inactive) const {
        if (!valid()) {
            return Component{};
        }
        static const MethodInfo* method = detail::get_method(detail::component_class(), "GetComponentInChildren", 2);
        Il2CppReflectionType* reflection_type = detail::make_reflection_type(klass);
        int32_t include = include_inactive ? 1 : 0;
        void* args[2] = { reflection_type, &include };
        Il2CppObject* result = detail::runtime_invoke(method, get(), args);
        return Component(result);
    }

    inline Component Component::get_component_in_parent(Il2CppClass* klass, bool include_inactive) const {
        if (!valid()) {
            return Component{};
        }
        static const MethodInfo* method = detail::get_method(detail::component_class(), "GetComponentInParent", 2);
        Il2CppReflectionType* reflection_type = detail::make_reflection_type(klass);
        int32_t include = include_inactive ? 1 : 0;
        void* args[2] = { reflection_type, &include };
        Il2CppObject* result = detail::runtime_invoke(method, get(), args);
        return Component(result);
    }

    class Behaviour : public Component {
    public:
        Behaviour() = default;
        explicit Behaviour(Il2CppObject* instance) : Component(instance) {}

        bool enabled() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::behaviour_class(), "get_enabled", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        void set_enabled(bool value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::behaviour_class(), "set_enabled", 1);
            int32_t enabled_value = value ? 1 : 0;
            void* args[1] = { &enabled_value };
            detail::runtime_invoke(method, get(), args);
        }
    };

    class Material : public Object {
    public:
        Material() = default;
        explicit Material(Il2CppObject* instance) : Object(instance) {}

        math::Color color() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::material_class(), "get_color", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_color(result);
        }

        void set_color(const math::Color& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::material_class(), "set_color", 1);
            math::Color copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        void set_float(const std::string& name, float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::material_class(), "SetFloat", 2);
            Il2CppString* param = detail::make_string(name);
            void* args[2] = { param, &value };
            detail::runtime_invoke(method, get(), args);
        }

        float get_float(const std::string& name) const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::material_class(), "GetFloat", 1);
            Il2CppString* param = detail::make_string(name);
            void* args[1] = { param };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return detail::unbox_float(result);
        }

        void enable_keyword(const std::string& keyword) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::material_class(), "EnableKeyword", 1);
            Il2CppString* key = detail::make_string(keyword);
            void* args[1] = { key };
            detail::runtime_invoke(method, get(), args);
        }

        void disable_keyword(const std::string& keyword) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::material_class(), "DisableKeyword", 1);
            Il2CppString* key = detail::make_string(keyword);
            void* args[1] = { key };
            detail::runtime_invoke(method, get(), args);
        }
    };

    class Renderer : public Component {
    public:
        Renderer() = default;
        explicit Renderer(Il2CppObject* instance) : Component(instance) {}

        bool enabled() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::renderer_class(), "get_enabled", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        void set_enabled(bool value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::renderer_class(), "set_enabled", 1);
            int32_t enabled_value = value ? 1 : 0;
            void* args[1] = { &enabled_value };
            detail::runtime_invoke(method, get(), args);
        }

        Material material() const {
            if (!valid()) {
                return Material{};
            }
            static const MethodInfo* method = detail::get_method(detail::renderer_class(), "get_material", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return Material(result);
        }

        void set_material(const Material& material_instance) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::renderer_class(), "set_material", 1);
            Il2CppObject* material_object = material_instance.get();
            void* args[1] = { material_object };
            detail::runtime_invoke(method, get(), args);
        }

        int32_t sorting_order() const {
            if (!valid()) {
                return 0;
            }
            static const MethodInfo* method = detail::get_method(detail::renderer_class(), "get_sortingOrder", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_int(result);
        }

        void set_sorting_order(int32_t value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::renderer_class(), "set_sortingOrder", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }
    };

    class MeshRenderer : public Renderer {
    public:
        MeshRenderer() = default;
        explicit MeshRenderer(Il2CppObject* instance) : Renderer(instance) {}
    };

    class Light : public Behaviour {
    public:
        Light() = default;
        explicit Light(Il2CppObject* instance) : Behaviour(instance) {}

        float intensity() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::light_class(), "get_intensity", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_intensity(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::light_class(), "set_intensity", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        math::Color color() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::light_class(), "get_color", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_color(result);
        }

        void set_color(const math::Color& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::light_class(), "set_color", 1);
            math::Color copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        float range() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::light_class(), "get_range", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_range(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::light_class(), "set_range", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }
    };

    class Camera : public Behaviour {
    public:
        Camera() = default;
        explicit Camera(Il2CppObject* instance) : Behaviour(instance) {}

        static Camera main() {
            static const MethodInfo* method = detail::get_method(detail::camera_class(), "get_main", 0);
            Il2CppObject* result = detail::runtime_invoke(method, nullptr, nullptr);
            return Camera(result);
        }

        float field_of_view() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::camera_class(), "get_fieldOfView", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_field_of_view(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::camera_class(), "set_fieldOfView", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        float near_clip_plane() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::camera_class(), "get_nearClipPlane", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_near_clip_plane(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::camera_class(), "set_nearClipPlane", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        float far_clip_plane() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::camera_class(), "get_farClipPlane", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_far_clip_plane(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::camera_class(), "set_farClipPlane", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector3 world_to_screen_point(const math::Vector3& world) const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::camera_class(), "WorldToScreenPoint", 1);
            math::Vector3 copy = world;
            void* args[1] = { &copy };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return math::to_vector3(result);
        }

        math::Vector3 screen_to_world_point(const math::Vector3& screen) const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::camera_class(), "ScreenToWorldPoint", 1);
            math::Vector3 copy = screen;
            void* args[1] = { &copy };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return math::to_vector3(result);
        }
    };

    class Rigidbody : public Component {
    public:
        Rigidbody() = default;
        explicit Rigidbody(Il2CppObject* instance) : Component(instance) {}

        math::Vector3 velocity() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "get_velocity", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector3(result);
        }

        void set_velocity(const math::Vector3& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "set_velocity", 1);
            math::Vector3 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        void add_force(const math::Vector3& force, int32_t force_mode = 0) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "AddForce", 2);
            math::Vector3 copy = force;
            void* args[2] = { &copy, &force_mode };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector3 angular_velocity() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "get_angularVelocity", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector3(result);
        }

        void set_angular_velocity(const math::Vector3& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "set_angularVelocity", 1);
            math::Vector3 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        float mass() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "get_mass", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_mass(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "set_mass", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        float drag() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "get_drag", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_drag(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "set_drag", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        bool use_gravity() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "get_useGravity", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        void set_use_gravity(bool value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::rigidbody_class(), "set_useGravity", 1);
            int32_t enabled_value = value ? 1 : 0;
            void* args[1] = { &enabled_value };
            detail::runtime_invoke(method, get(), args);
        }
    };

    class Collider : public Component {
    public:
        Collider() = default;
        explicit Collider(Il2CppObject* instance) : Component(instance) {}

        bool enabled() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::collider_class(), "get_enabled", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        void set_enabled(bool value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::collider_class(), "set_enabled", 1);
            int32_t enabled_value = value ? 1 : 0;
            void* args[1] = { &enabled_value };
            detail::runtime_invoke(method, get(), args);
        }

        bool is_trigger() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::collider_class(), "get_isTrigger", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        void set_is_trigger(bool value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::collider_class(), "set_isTrigger", 1);
            int32_t trigger_value = value ? 1 : 0;
            void* args[1] = { &trigger_value };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector3 closest_point(const math::Vector3& position) const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::collider_class(), "ClosestPoint", 1);
            math::Vector3 copy = position;
            void* args[1] = { &copy };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return math::to_vector3(result);
        }
    };

    class CharacterController : public Collider {
    public:
        CharacterController() = default;
        explicit CharacterController(Il2CppObject* instance) : Collider(instance) {}

        math::Vector3 velocity() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::character_controller_class(), "get_velocity", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector3(result);
        }

        bool is_grounded() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::character_controller_class(), "get_isGrounded", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        float height() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::character_controller_class(), "get_height", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_height(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::character_controller_class(), "set_height", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        float radius() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::character_controller_class(), "get_radius", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_radius(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::character_controller_class(), "set_radius", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        math::Vector3 center() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::character_controller_class(), "get_center", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector3(result);
        }

        void set_center(const math::Vector3& value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::character_controller_class(), "set_center", 1);
            math::Vector3 copy = value;
            void* args[1] = { &copy };
            detail::runtime_invoke(method, get(), args);
        }

        int32_t move(const math::Vector3& motion) const {
            if (!valid()) {
                return 0;
            }
            static const MethodInfo* method = detail::get_method(detail::character_controller_class(), "Move", 1);
            math::Vector3 copy = motion;
            void* args[1] = { &copy };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return detail::unbox_int(result);
        }
    };

    class NavMeshAgent : public Behaviour {
    public:
        NavMeshAgent() = default;
        explicit NavMeshAgent(Il2CppObject* instance) : Behaviour(instance) {}

        math::Vector3 destination() const {
            if (!valid()) {
                return {};
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "get_destination", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return math::to_vector3(result);
        }

        bool set_destination(const math::Vector3& value) const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "SetDestination", 1);
            math::Vector3 copy = value;
            void* args[1] = { &copy };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return detail::unbox_bool(result);
        }

        float remaining_distance() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "get_remainingDistance", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        float stopping_distance() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "get_stoppingDistance", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_stopping_distance(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "set_stoppingDistance", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        bool has_path() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "get_hasPath", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        bool is_stopped() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "get_isStopped", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }

        void set_is_stopped(bool value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "set_isStopped", 1);
            int32_t stopped = value ? 1 : 0;
            void* args[1] = { &stopped };
            detail::runtime_invoke(method, get(), args);
        }

        float speed() const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "get_speed", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_speed(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::nav_mesh_agent_class(), "set_speed", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }
    };

    class Canvas : public Behaviour {
    public:
        Canvas() = default;
        explicit Canvas(Il2CppObject* instance) : Behaviour(instance) {}

        int32_t render_mode() const {
            if (!valid()) {
                return 0;
            }
            static const MethodInfo* method = detail::get_method(detail::canvas_class(), "get_renderMode", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_int(result);
        }

        void set_render_mode(int32_t value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::canvas_class(), "set_renderMode", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        float scale_factor() const {
            if (!valid()) {
                return 1.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::canvas_class(), "get_scaleFactor", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_float(result);
        }

        void set_scale_factor(float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::canvas_class(), "set_scaleFactor", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }

        int32_t sorting_order() const {
            if (!valid()) {
                return 0;
            }
            static const MethodInfo* method = detail::get_method(detail::canvas_class(), "get_sortingOrder", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_int(result);
        }

        void set_sorting_order(int32_t value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::canvas_class(), "set_sortingOrder", 1);
            void* args[1] = { &value };
            detail::runtime_invoke(method, get(), args);
        }
    };

    class Animator : public Behaviour {
    public:
        Animator() = default;
        explicit Animator(Il2CppObject* instance) : Behaviour(instance) {}

        void play(const std::string& state_name) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "Play", 1);
            Il2CppString* state = detail::make_string(state_name);
            void* args[1] = { state };
            detail::runtime_invoke(method, get(), args);
        }

        void play(const std::string& state_name, int32_t layer, float normalized_time = -1.0f) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "Play", 3);
            Il2CppString* state = detail::make_string(state_name);
            void* args[3] = { state, &layer, &normalized_time };
            detail::runtime_invoke(method, get(), args);
        }

        void set_float(const std::string& name, float value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "SetFloat", 2);
            Il2CppString* param = detail::make_string(name);
            void* args[2] = { param, &value };
            detail::runtime_invoke(method, get(), args);
        }

        float get_float(const std::string& name) const {
            if (!valid()) {
                return 0.0f;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "GetFloat", 1);
            Il2CppString* param = detail::make_string(name);
            void* args[1] = { param };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return detail::unbox_float(result);
        }

        void set_bool(const std::string& name, bool value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "SetBool", 2);
            Il2CppString* param = detail::make_string(name);
            int32_t bool_value = value ? 1 : 0;
            void* args[2] = { param, &bool_value };
            detail::runtime_invoke(method, get(), args);
        }

        bool get_bool(const std::string& name) const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "GetBool", 1);
            Il2CppString* param = detail::make_string(name);
            void* args[1] = { param };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return detail::unbox_bool(result);
        }

        void set_integer(const std::string& name, int32_t value) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "SetInteger", 2);
            Il2CppString* param = detail::make_string(name);
            void* args[2] = { param, &value };
            detail::runtime_invoke(method, get(), args);
        }

        int32_t get_integer(const std::string& name) const {
            if (!valid()) {
                return 0;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "GetInteger", 1);
            Il2CppString* param = detail::make_string(name);
            void* args[1] = { param };
            Il2CppObject* result = detail::runtime_invoke(method, get(), args);
            return detail::unbox_int(result);
        }

        void set_trigger(const std::string& name) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "SetTrigger", 1);
            Il2CppString* param = detail::make_string(name);
            void* args[1] = { param };
            detail::runtime_invoke(method, get(), args);
        }

        void reset_trigger(const std::string& name) const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::animator_class(), "ResetTrigger", 1);
            Il2CppString* param = detail::make_string(name);
            void* args[1] = { param };
            detail::runtime_invoke(method, get(), args);
        }
    };

    class AudioSource : public Behaviour {
    public:
        AudioSource() = default;
        explicit AudioSource(Il2CppObject* instance) : Behaviour(instance) {}

        void play() const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::audio_source_class(), "Play", 0);
            detail::runtime_invoke(method, get(), nullptr);
        }

        void stop() const {
            if (!valid()) {
                return;
            }
            static const MethodInfo* method = detail::get_method(detail::audio_source_class(), "Stop", 0);
            detail::runtime_invoke(method, get(), nullptr);
        }

        bool is_playing() const {
            if (!valid()) {
                return false;
            }
            static const MethodInfo* method = detail::get_method(detail::audio_source_class(), "get_isPlaying", 0);
            Il2CppObject* result = detail::runtime_invoke(method, get(), nullptr);
            return detail::unbox_bool(result);
        }
    };

} // namespace il2cppx