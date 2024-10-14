#include "pch-il2cpp.h"

#include "Wrapper.h"

#include <iostream>
#include <string.h>

Wrapper::Wrapper(const char* imageName) : image(GetImage(imageName)), obj(nullptr) {}

const Il2CppImage* Wrapper::GetImage(const char* imageName)
{
    if (image)
        return image;

    const Il2CppDomain* domain = il2cpp_domain_get();
    size_t size;
    const Il2CppAssembly** assemblies = il2cpp_domain_get_assemblies(domain, &size);

    for (size_t i = 0; i < size; ++i) {
        const Il2CppAssembly* assembly = assemblies[i];
        if (assembly) {
            const char* assemblyName = il2cpp_image_get_name(assembly->image);
            if (std::string(assemblyName) == imageName) {
                return assembly->image;
            }
        }
    }
    return nullptr;
}

Wrapper& Wrapper::find_class(const char* classNamespace, const char* className)
{
    Il2CppClass* _class = il2cpp_class_from_name(image, classNamespace, className);
    if (_class) {
        const Il2CppType* _type = il2cpp_class_get_type(_class);
        if (_type) {
            obj = il2cpp_type_get_object(_type);
        }
    }

    return *this;
}

void Wrapper::get_fields(const char* classNamespace, const char* className)
{

}

void Wrapper::method_info(const char* classNamespace, const char* className)
{

}


void Wrapper::classList()
{
    size_t classCount = il2cpp_image_get_class_count(image);

    std::cout << "{\n";

    for (size_t i = 0; i < classCount; ++i) {
        const Il2CppClass* _klass = il2cpp_image_get_class(image, i);

        if (_klass) {
            char* _name = const_cast<char*>(il2cpp_class_get_name(const_cast<Il2CppClass*>(_klass)));
            char* _namespace = const_cast<char*>(il2cpp_class_get_namespace(const_cast<Il2CppClass*>(_klass)));

            std::cout << " [\n";
            std::cout << "\tName: " << _name << "\n";
            std::cout << "\tNamespace: " << _namespace << "\n";

            std::cout << " ],\n";
        }
    }

    std::cout << "\n}\n";
}

Il2CppObject* Wrapper::get_class()
{
    return obj;
}

const char* Wrapper::GetImageName()
{
    if (image)
        return il2cpp_image_get_name(image);

    return nullptr;
}

const Il2CppImage* Wrapper::get_image()
{
    if (image)
        return image;

    return nullptr;
}
