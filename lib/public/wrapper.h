#pragma once

struct Il2CppObject;
struct Il2CppImage;

class Wrapper {
public:
	Wrapper(const char* imageName);
	Wrapper& find_class(const char* classNamespace, const char* className);
	void get_fields(const char* classNamespace, const char* className);
	void method_info(const char* classNamespace, const char* className);
	void classList();
	Il2CppObject* get_class();
	const char* GetImageName();
	const Il2CppImage* get_image();
private:
	const Il2CppImage* GetImage(const char* image);
	const Il2CppImage* image;
	Il2CppObject* obj;
};
