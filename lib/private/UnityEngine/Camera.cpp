#include "pch-il2cpp.h"

#include "UnityEngine/Camera.h"

app::Camera* Camera::GetMainCamera()
{
	return app::Camera_get_main(nullptr) ? app::Camera_get_main(nullptr) : nullptr;
}
