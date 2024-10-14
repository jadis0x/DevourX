#include "pch-il2cpp.h"

#include "UnityEngine/Object.h"
#include <helpers.h>

std::string Object::ToString(app::Object* object)
{
	std::string type = il2cppi_to_string(app::Object_ToString(object, NULL));
	if (type == "System.String") {
		return il2cppi_to_string((app::String*)object);
	}
	return type;
}

std::string Object::ToString(app::Object_1* object)
{
	std::string type = il2cppi_to_string(app::Object_1_ToString(object, NULL));
	if (type == "System.String") {
		return il2cppi_to_string((app::String*)object);
	}
	return type;
}

std::string Object::GetObjectName(app::Object_1* obj)
{
	return ToString(obj);
}

app::GameObject__Array* Object::FindGameObjectsWithTag(const char* tag)
{
	app::GameObject__Array* go_array_result = app::GameObject_FindGameObjectsWithTag(ConvertToSystemString(tag), nullptr);

	return go_array_result ? go_array_result : nullptr;
}

void Object::FindObjectFromInstanceID(int32_t instanceID)
{
}

bool Object::IsNull(app::Object_1* obj)
{
	if (obj == nullptr)
		return true;
	
	return !app::Object_1_op_Implicit(obj, nullptr);
}

app::Object_1__Array* Object::FindObjectsOfType(const char* className, const char* classNamespace, const char* assemblyName)
{
	// Wrapper objeyi bir kez oluþtur ve tekrar kullan
	Wrapper obj(assemblyName);

	// find_class'in baþarýsýz olma olasýlýðýna karþý önceden kontrol ekle
	auto classWrapper = obj.find_class(classNamespace, className);

	// Il2CppObject'yi al ve geçerli olup olmadýðýný kontrol et
	Il2CppObject* object = classWrapper.get_class();
	if (!object) {
		return nullptr;
	}

	// FindObjectsOfType fonksiyonunun geçerli olup olmadýðýný kontrol et
	if (!app::Object_1_FindObjectsOfType) {
		return nullptr;
	}

	// FindObjectsOfType çaðýr ve sonuçlarý kontrol et
	app::Object_1__Array* obj_1 = app::Object_1_FindObjectsOfType(reinterpret_cast<app::Type*>(object), nullptr);
	return obj_1 ? obj_1 : nullptr;
}
