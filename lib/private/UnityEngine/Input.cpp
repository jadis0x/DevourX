#include "pch-il2cpp.h"


#include "UnityEngine/Input.h"
#include <helpers.h>
#include <Windows.h>

float Input::GetAxis(const char* axisName)
{
	return app::Input_1_GetAxis(ConvertToSystemString(axisName), nullptr);
}

int Input::GetKey(KeyCode code)
{
	return static_cast<int>(code);
}

bool Input::GetKeyDown(KeyCode key)
{
	// Convert KeyCode enum value to virtual keycode
	int vkey = GetKey(key);

	// Check key state with GetKeyState
	return (GetKeyState(vkey) & 0x8000) != 0;
}

bool Input::GetKeyDownAsync(KeyCode key)
{
	// Convert KeyCode enum value to virtual keycode
	int vkey = GetKey(key);

	// Check key state with GetAsyncKeyState
	return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}
