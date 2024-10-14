#pragma once

#include "wrapper.h"
#include <string>

class Object {
public:
	static std::string ToString(app::Object* object);
	static std::string ToString(app::Object_1* object);
	static std::string GetObjectName(app::Object_1* obj);
	static app::GameObject__Array* FindGameObjectsWithTag(const char* tag);
	static void FindObjectFromInstanceID(int32_t instanceID);
	static bool IsNull(app::Object_1* obj);

	static app::Object_1__Array* FindObjectsOfType(const char* className, const char* classNamespace = "", const char* assemblyName = "Assembly-CSharp.dll");

	template<typename T>
	static inline T* FindObjectOfType(const char* className, const char* classNamespace = "", const char* assemblyName = "Assembly-CSharp.dll");

	template<typename T>
	static inline bool Enabled(T* behaviour);

	template<typename T>
	static inline void Enabled(T* behaviour, bool value);
};

template<typename T>
inline T* Object::FindObjectOfType(const char* className, const char* classNamespace, const char* assemblyName)
{
	Wrapper obj(assemblyName);

	Il2CppObject* object = obj.find_class(classNamespace, className).get_class();

	if (!object || !app::Object_1_FindObjectOfType) return nullptr;

	app::Object_1* obj_1 = app::Object_1_FindObjectOfType(reinterpret_cast<app::Type*>(object), nullptr);

	if (!obj_1 || IsNull(obj_1)) return nullptr;

	return reinterpret_cast<T*>(obj_1);
}

template<typename T>
inline bool Object::Enabled(T* behaviour)
{
	return app::Behaviour_get_enabled((app::Behaviour*)behaviour, nullptr);
}

template<typename T>
inline void Object::Enabled(T* behaviour, bool value)
{
	app::Behaviour_set_enabled((app::Behaviour*)behaviour, value, nullptr);
}
