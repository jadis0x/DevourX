#pragma once

#include "settings/settings.hpp"



namespace ESP {
	// TEMP FIX #60
	inline app::GameObject__Array* ents_azazel = NULL;
	inline app::Object_1__Array* ents_item;
	inline app::Object_1__Array* ents_key;
	inline app::Object_1__Array* ents_goat;
	inline app::Object_1__Array* ents_demon;

	inline std::string name_demon = "N/A";

	inline static int time_refresh = 500;
	inline static int time_counter = 0;

	app::Object_1__Array* RefreshEntList(app::Object_1__Array* ent,const char* className, const char* classNamespace = "");

	void RunPlayersESP();
	void RunGoatsESP();
	void RunItemsESP();
	void RunKeyESP();
	void RunDemonESP();
	void RunAzazelESP();
}
