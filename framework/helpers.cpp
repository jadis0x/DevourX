// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// Helper functions

#include "pch-il2cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <codecvt>
#include "helpers.h"
#include <cstdio>
#include <filesystem>

// Helper function to get the module base address
uintptr_t il2cppi_get_base_address() {
	return (uintptr_t)GetModuleHandleW(L"GameAssembly.dll");
}

static std::wstring GetLogFilePath()
{
	wchar_t modulePath[MAX_PATH] = { 0 };
	if (GetModuleFileNameW(nullptr, modulePath, MAX_PATH) == 0)
		return L"DevourX.log";

	std::filesystem::path path(modulePath);
	return (path.parent_path() / L"DevourX.log").wstring();
}

// Helper function to append text to a file
void il2cppi_log_write(std::string text) {
	std::wstring path = GetLogFilePath();

	HANDLE file = CreateFileW(
		path.c_str(),
		FILE_APPEND_DATA,
		FILE_SHARE_READ,
		nullptr,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (file == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		wchar_t buf[256];
		swprintf_s(buf, L"Could not open log file\nError: %lu", err);
		MessageBoxW(nullptr, buf, L"Error", 0);
		return;
	}

	DWORD written = 0;
	WriteFile(file, text.c_str(), static_cast<DWORD>(text.length()), &written, nullptr);

	static constexpr char kNewLine[] = "\r\n";
	WriteFile(file, kNewLine, sizeof(kNewLine) - 1, &written, nullptr);

	CloseHandle(file);
}

// Helper function to open a new console window and redirect stdout there
void il2cppi_new_console() {
	AllocConsole();

	FILE* stream = nullptr;
	freopen_s(&stream, "CONOUT$", "w", stdout);
}

#if _MSC_VER >= 1920
// Helper function to convert Il2CppString to std::string
std::string il2cppi_to_string(Il2CppString* str) {
	if (!str)
		return std::string{};

	const auto length = str->length;
	if (length <= 0)
		return std::string{};

	const auto* begin = reinterpret_cast<const char16_t*>(str->chars);
	const auto* end = begin + static_cast<size_t>(length);

	return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(begin, end);
}

// Helper function to convert System.String to std::string
std::string il2cppi_to_string(app::String* str) {
	return il2cppi_to_string(reinterpret_cast<Il2CppString*>(str));
}
app::String* convert_to_system_string(const char* str)
{
	Il2CppString* il2cpp_str = il2cpp_string_new(str);

	if (!il2cpp_str) return nullptr;

	return reinterpret_cast<app::String*>(il2cpp_str);
}

std::string ToString(app::Object* Object)
{
	std::string type = il2cppi_to_string(app::Object_ToString(Object, NULL));
	if (type == "System.String") {
		return il2cppi_to_string((app::String*)Object);
	}
	return type;
}

app::String* ToString(app::Object_1* Object)
{
	return app::Object_1_GetName(Object, nullptr);
}
#endif