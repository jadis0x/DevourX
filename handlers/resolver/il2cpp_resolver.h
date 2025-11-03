#pragma once

#include "il2cpp-appdata.h"
#include <string>
#include "helpers.h"
#include <iostream>
#include <vector>
#include <Windows.h>

#define ASSEMBLY_CSHARP         "Assembly-CSharp.dll"
#define CORE_MODULE             "UnityEngine.CoreModule.dll"
#define PHYSICS_MODULE          "UnityEngine.PhysicsModule.dll"
#define INPUT_MODULE            "UnityEngine.InputLegacyModule.dll"
#define UI_MODULE               "UnityEngine.UI.dll"
#define UNITY_TEXT              "UnityEngine.TextRenderingModule.dll"
#define UNITY_AUDIO             "UnityEngine.AudioModule.dll"

#define NS_UNITYENGINE "UnityEngine"
#define NS_SYSTEM      "System"
#define NS_EMPTY       ""

namespace SafePtr {
	inline bool IsValid(app::GameObject* obj) {
		return obj && obj->klass && app::Object_1_op_Implicit(reinterpret_cast<app::Object_1*>(obj), nullptr);
	}

	inline bool IsValid(app::Object_1* obj) {
		return obj && obj->klass && app::Object_1_op_Implicit(obj, nullptr);
	}

	template<typename Func>
	inline bool safe_call(Func&& fn)
	{
		__try {
			fn();
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return false;
		}
	}

	inline bool IsValidIl2CppObject(void* ptr) {
		if (!ptr) return false;

		uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
		if (addr < 0x10000 || addr > 0x7FFFFFFFFFFF)
			return false;

		__try {
			void* klass = *(void**)ptr;
			return klass != nullptr;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return false;
		}
	}
}

namespace app {
	Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
	Vector3 operator*(const Vector3& vec, float scalar);
	Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
}

namespace LuridLabMathLibrary {
	namespace Vector3Utils {
		inline float Dot(const app::Vector3& a, const app::Vector3& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		inline float Magnitude(const app::Vector3& v)
		{
			return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		}

		inline app::Vector3 Normalize(const app::Vector3& v)
		{
			float mag = Magnitude(v);
			if (mag == 0.0f)
				return { 0.f, 0.f, 0.f };

			return { v.x / mag, v.y / mag, v.z / mag };
		}

		inline app::Vector3 Sub(const app::Vector3& a, const app::Vector3& b)
		{
			return { a.x - b.x, a.y - b.y, a.z - b.z };
		}

		inline app::Vector3 Add(const app::Vector3& a, const app::Vector3& b)
		{
			return { a.x + b.x, a.y + b.y, a.z + b.z };
		}

		inline app::Vector3 Multiply(const app::Vector3& v, float scalar)
		{
			return { v.x * scalar, v.y * scalar, v.z * scalar };
		}
	}
}

class Il2CppInvoker
{
public:
	static Il2CppDomain* AttachDomainOnce();
	static const Il2CppImage* LoadImageFromAssembly(const char* dllName);
	static Il2CppClass* FindClass(const Il2CppImage* image, const char* namespaze, const char* className);
	static const MethodInfo* FindMethod(Il2CppClass* klass, const char* methodName, int paramCount);
	static Il2CppObject* InvokeStatic(const MethodInfo* method, void** args);
	static void DumpMethods(Il2CppClass* klass, const char* className);
	static const MethodInfo* FindMethodByParamNames(Il2CppClass* klass, const char* methodName, std::initializer_list<const char*> paramTypeNames);
};

class Il2cppData {
public:
	Il2cppData();
	~Il2cppData();
	Il2CppDomain* GetDomain() const;
	void SetImage(const char* imageName);
	const Il2CppImage* GetImage() const;

	void FindClass(const char* _namespace, const char* _className);

	Il2CppObject* Get();
private:
	Il2CppObject* _object = nullptr;
	Il2CppDomain* _domain = nullptr;
	const Il2CppImage* _image = nullptr;
};

class UGameObject {
public:
	void Reset();

	app::GameObject* Find(const char* name);

	app::GameObject* FindWithTag(const char* tag);
	app::GameObject__Array* FindAllWithTag(const char* tag);

	app::GameObject* Get();

	app::Transform* GetTransform();
	bool IsValid();

	app::String* GetName();
protected:
	app::GameObject* gameObject = nullptr;
	app::Transform* transform = nullptr;
};

class UComponent : public UGameObject {
public:
	app::Component* GetComponentByName(const char* componentName);
	void Attach(app::GameObject* obj);
	app::Component* GetCurrent();
private:
	app::Component* component = nullptr;
};

class UCamera {
public:
	app::Camera* FindMain();
	app::Camera* Get();

	app::Vector3 GetPosition();
	app::Vector3 GetForward();
	app::Vector3 GetRight();
	app::Vector3 GetUp();

	app::Vector3 WorldToScreen(const app::Vector3& world, bool* success = nullptr);

	float DistanceTo(const app::Vector3& target);

	void Reset();
	bool IsValid();

private:
	app::Camera* camera = nullptr;
	app::Transform* transform = nullptr;
};

template<typename T>
class UObject {
public:
	const std::vector<T*>& FindAllObjects(Il2cppData& cppData, bool forceRefresh = false);

	T* FindByName(const char* name, Il2cppData& cppData);

	void Debug();
protected:
	inline std::string ToString(app::Object_1* object) {
		app::String* str = app::Object_1_ToString(object, nullptr);
		return str ? il2cppi_to_string(str) : "null";
	}

	inline void ResetCache();

private:
	T* currentObject = nullptr;
	std::vector<T*> cache;
};

template<typename T>
inline const std::vector<T*>& UObject<T>::FindAllObjects(Il2cppData& cppData, bool forceRefresh)
{
	if (!cache.empty() && !forceRefresh)
		return cache;

	cache.clear();

	Il2CppObject* _obj = cppData.Get();

	if (!_obj) {
		return cache;
	}

	app::Type* type = reinterpret_cast<app::Type*>(_obj);

	if (!type) {
		return cache;
	}

	app::Object_1__Array* objects = app::Object_1_FindObjectsOfType(type, nullptr);

	if (!objects || !objects->vector || objects->max_length <= 0) {
		return cache;
	}

	for (int i = 0; i < objects->max_length; ++i) {
		app::Object_1* raw = objects->vector[i];

		if (!SafePtr::IsValid(raw)) {
			continue;
		}

		T* casted = reinterpret_cast<T*>(raw);

		if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(casted))) {
			continue;
		}

		cache.push_back(casted);
	}

	return cache;
}

template<typename T>
inline T* UObject<T>::FindByName(const char* name, Il2cppData& cppData)
{
	for (T* obj : FindAllObjects(cppData)) {
		if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(obj))) continue;
		app::String* objName = app::Object_1_get_name(reinterpret_cast<app::Object_1*>(obj), nullptr);
		if (!objName) continue;
		if (il2cppi_to_string(objName) == name) {
			currentObject = obj;
			return currentObject;
		}
	}
	return nullptr;
}

template<typename T>
inline void UObject<T>::Debug()
{
	for (auto* obj : cache) {
		if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(obj)))
			continue;

		app::String* name = app::Object_1_get_name(reinterpret_cast<app::Object_1*>(obj), nullptr);
		if (name) {
			std::string cppName = il2cppi_to_string(name);
			std::cout << cppName << std::endl;
		}
	}
}

template<typename T>
inline void UObject<T>::ResetCache()
{
	cache.clear();
	currentObject = nullptr;
}

namespace Func {
	template<typename T>
	inline T* FindObject(const char* dll, const char* namespaze, const char* class_name);


	template<typename T>
	T* FindObject(const char* dll, const char* namespaze, const char* class_name)
	{
		auto* image = Il2CppInvoker::LoadImageFromAssembly(dll);
		auto* klass = Il2CppInvoker::FindClass(image, namespaze, class_name);

		const Il2CppType* _type = il2cpp_class_get_type(klass);

		if (!_type) return nullptr;

		Il2CppObject* _type_object = il2cpp_type_get_object(_type);

		if (!_type_object) return nullptr;

		app::Type* _appType = reinterpret_cast<app::Type*>(_type_object);

		if (!_appType) return nullptr;

		if (app::Object_1_FindObjectOfType_1 == nullptr) return nullptr;

		app::Object_1* result = app::Object_1_FindObjectOfType_1(_appType, false, nullptr);

		if (!SafePtr::IsValid(result)) return nullptr;

		return reinterpret_cast<T*>(result);
	}
}
