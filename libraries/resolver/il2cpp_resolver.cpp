#include "pch-il2cpp.h"

#include "il2cpp_resolver.h"

namespace app {
	Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
		return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}

	Vector3 operator*(const Vector3& vec, float scalar) {
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
	}

	Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}
}


#pragma region Il2cppData
Il2cppData::Il2cppData()
{
	_domain = il2cpp_domain_get();
}

Il2cppData::~Il2cppData()
{
	_domain = nullptr;
	_image = nullptr;
}

void Il2cppData::SetImage(const char* imageName)
{
	if (_domain != nullptr) {
		if (const Il2CppAssembly* assembly = il2cpp_domain_assembly_open(_domain, imageName))
		{
			_image = assembly->image;
		}
	}
}

const Il2CppImage* Il2cppData::GetImage() const
{
	return _image;
}

void Il2cppData::FindClass(const char* _namespace, const char* _className)
{
	if (_object) return;
	if (!_image) return;

	Il2CppClass* target_class = il2cpp_class_from_name(_image, _namespace, _className);
	if (!target_class) return;

	const Il2CppType* target_type = il2cpp_class_get_type(target_class);
	if (!target_type) return;

	Il2CppObject* result = il2cpp_type_get_object(target_type);
	if (!result || !result->klass) return;

	_object = result;
}

Il2CppObject* Il2cppData::Get()
{
	return _object;
}

Il2CppDomain* Il2cppData::GetDomain() const
{
	return _domain;
}

#pragma endregion Il2cppData


#pragma region UGameObject
void UGameObject::Reset() {
	gameObject = nullptr;
	transform = nullptr;
}

app::GameObject* UGameObject::Find(const char* name) {
	if (!IsValid()) {
		app::String* sys_str = convert_to_system_string(name);
		gameObject = app::GameObject_Find(sys_str, nullptr);
		transform = nullptr;
	}
	return gameObject;
}

app::GameObject* UGameObject::FindWithTag(const char* tag) {
	if (!IsValid()) {
		app::String* sys_str = convert_to_system_string(tag);
		gameObject = app::GameObject_FindWithTag(sys_str, nullptr);
		transform = nullptr;
	}
	return gameObject;
}

app::GameObject__Array* UGameObject::FindAllWithTag(const char* tag)
{
	app::String* sys_str = convert_to_system_string(tag);
	return app::GameObject_FindGameObjectsWithTag(sys_str, nullptr);
}

app::GameObject* UGameObject::Get() {
	return IsValid() ? gameObject : nullptr;
}

app::Transform* UGameObject::GetTransform() {
	if (!IsValid())
		return nullptr;

	if (!transform || !transform->klass)
		transform = app::GameObject_get_transform(gameObject, nullptr);

	return transform;
}

bool UGameObject::IsValid() {
	if (!gameObject || !gameObject->klass)
		return false;

	return app::Object_1_op_Implicit(reinterpret_cast<app::Object_1*>(gameObject), nullptr);
}

app::String* UGameObject::GetName()
{
	if (!IsValid()) return nullptr;

	return nullptr;
}
#pragma endregion UGameObject

#pragma region UCamera
app::Camera* UCamera::FindMain()
{
	if (!IsValid()) {
		camera = app::Camera_get_main(nullptr);
		transform = nullptr;
	}
	return camera;
}

app::Camera* UCamera::Get()
{
	return IsValid() ? camera : nullptr;
}

bool UCamera::IsValid()
{
	if (!camera || !camera->klass)
		return false;

	return app::Object_1_op_Implicit(reinterpret_cast<app::Object_1*>(camera), nullptr);
}

app::Vector3 UCamera::GetPosition()
{
	if (!IsValid()) return { 0,0,0 };

	if (!transform || !transform->klass)
		transform = app::Component_get_transform(reinterpret_cast<app::Component*>(camera), nullptr);

	return app::Transform_get_position(transform, nullptr);
}

app::Vector3 UCamera::GetForward()
{
	if (!IsValid()) return { 0, 0, 1 };
	if (!transform || !transform->klass)
		transform = app::Component_get_transform(reinterpret_cast<app::Component*>(camera), nullptr);

	return app::Transform_get_forward(transform, nullptr);
}

app::Vector3 UCamera::GetRight()
{
	if (!IsValid()) return { 1, 0, 0 };
	if (!transform || !transform->klass)
		transform = app::Component_get_transform(reinterpret_cast<app::Component*>(camera), nullptr);

	return app::Transform_get_right(transform, nullptr);
}

app::Vector3 UCamera::GetUp()
{
	if (!IsValid()) return { 0, 1, 0 };
	if (!transform || !transform->klass)
		transform = app::Component_get_transform(reinterpret_cast<app::Component*>(camera), nullptr);

	return app::Transform_get_up(transform, nullptr);
}

app::Vector3 UCamera::WorldToScreen(const app::Vector3& world, bool* success)
{
	app::Vector3 screen = { 0,0,0 };

	if (!FindMain()) {
		if (success) *success = false;
		return screen;
	}

	screen = app::Camera_WorldToScreenPoint_1(camera, world, nullptr);
	if (success) *success = (screen.z > 0.f);
	return screen;
}

float UCamera::DistanceTo(const app::Vector3& target)
{
	app::Vector3 camPos = this->GetPosition();
	float dx = camPos.x - target.x;
	float dy = camPos.y - target.y;
	float dz = camPos.z - target.z;
	return sqrtf(dx * dx + dy * dy + dz * dz);
}

void UCamera::Reset()
{
	camera = nullptr;
	transform = nullptr;
}
#pragma endregion UCamera

#pragma region UComponent
app::Component* UComponent::GetComponentByName(const char* componentName)
{
	if (!IsValid()) return nullptr;

	if (component && SafePtr::IsValid(reinterpret_cast<app::Object_1*>(component)))
		return component;

	app::String* sys_str = convert_to_system_string(componentName);
	if (app::GameObject_GetComponentByName == nullptr) return nullptr;

	component = app::GameObject_GetComponentByName(gameObject, sys_str, nullptr);
	return component;
}

void UComponent::Attach(app::GameObject* obj)
{
	this->gameObject = obj;
	this->transform = nullptr;
	this->component = nullptr;
}

app::Component* UComponent::GetCurrent()
{
	if (!IsValid()) return nullptr;

	return component;
}
#pragma endregion UComponent

Il2CppDomain* Il2CppInvoker::AttachDomainOnce()
{
	static thread_local bool il2cpp_attached = false;
	Il2CppDomain* domain = il2cpp_domain_get();

	if (!domain) {
		std::cout << "[ERROR] il2cpp_domain_get null!\n";
		return nullptr;
	}

	if (!il2cpp_attached) {
		if (const Il2CppThread* thread = il2cpp_thread_attach(domain); !thread) {
			std::cout << "[ERROR] il2cpp_thread_attach failed!\n";
			return nullptr;
		}
		std::cout << "[INFO] Attached to domain.\n";
		il2cpp_attached = true;
	}

	return domain;
}

const Il2CppImage* Il2CppInvoker::LoadImageFromAssembly(const char* dllName)
{
	const Il2CppDomain* domain = AttachDomainOnce();
	if (!domain) return nullptr;

	size_t size = 0;
	const Il2CppAssembly** assemblies = il2cpp_domain_get_assemblies(domain, &size);

	for (size_t i = 0; i < size; ++i) {
		const Il2CppImage* img = il2cpp_assembly_get_image(assemblies[i]);
		if (!img) continue;

		if (const char* name = il2cpp_image_get_name(img); strcmp(name, dllName) == 0)
			return img;
	}

	std::cout << "[ERROR] DLL not found: " << dllName << "\n";
	return nullptr;
}

Il2CppClass* Il2CppInvoker::FindClass(const Il2CppImage* image, const char* namespaze, const char* className)
{
	if (!image) return nullptr;

	Il2CppClass* klass = il2cpp_class_from_name(image, namespaze, className);
	if (!klass)
		std::cout << "[ERROR] class not found: " << className << "\n";
	return klass;
}

const MethodInfo* Il2CppInvoker::FindMethod(Il2CppClass* klass, const char* methodName, int paramCount)
{
	if (!klass) return nullptr;

	const MethodInfo* method = il2cpp_class_get_method_from_name(klass, methodName, paramCount);
	if (!method)
		std::cout << "[ERROR] Metot bulunamadi: " << methodName << "\n";
	return method;
}

Il2CppObject* Il2CppInvoker::InvokeStatic(const MethodInfo* method, void** args)
{
	if (!method) {
		return nullptr;
	}

	Il2CppException* exception = nullptr;
	Il2CppObject* result = il2cpp_runtime_invoke(method, nullptr, args, &exception);

	if (exception) {
		char msg[512];
		il2cpp_format_exception(exception, msg, sizeof(msg));
		std::cout << "[EXCEPTION] " << msg << "\n";
		return nullptr;
	}

	return result;
}

void Il2CppInvoker::DumpMethods(Il2CppClass* klass, const char* className)
{
	if (!klass) {
		return;
	}

	void* iter = nullptr;
	const MethodInfo* method = nullptr;
	int index = 0;

	std::cout << "\n=== Dump => " << className << " ===\n";
	while ((method = il2cpp_class_get_methods(klass, &iter)))
	{
		const Il2CppType* retType = method->return_type;
		Il2CppClass* retClass = il2cpp_type_get_class_or_element_class(retType);
		const char* retName = retClass ? il2cpp_class_get_name(retClass) : "void";

		std::cout << "[" << index++ << "] " << retName << " " << method->name << "(";

		for (int i = 0; i < method->parameters_count; ++i) {
			const Il2CppType* param = method->parameters[i];
			Il2CppClass* paramClass = il2cpp_type_get_class_or_element_class(param);
			std::cout << (paramClass ? il2cpp_class_get_name(paramClass) : "unknown");

			if (i < method->parameters_count - 1)
				std::cout << ", ";
		}

		std::cout << ") [ParamCount: " << method->parameters_count << "]\n";
	}
	std::cout << "=== Dump End ===\n";
}