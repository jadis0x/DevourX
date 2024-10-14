#pragma once

namespace GameObject {
	app::Component* GetComponentByName(app::GameObject* go, const char* type);
	app::Component__Array* LogComponents(app::GameObject* go);
};