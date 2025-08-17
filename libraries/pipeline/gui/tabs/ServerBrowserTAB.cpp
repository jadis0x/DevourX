#include "pch-il2cpp.h"

#include "pipeline/gui/tabs/ServerBrowserTAB.h"
#include <devour/devourbase.h>

#include <imgui/imgui.h>
#include <helpers.h>
#include <iostream>
#include <resolver/il2cpp_resolver.h>
#include <pipeline/settings.h>

std::vector<ServerEntry> g_ServerList;

app::ServerBrowser* browser = nullptr;
app::ServerConnectToken* g_connectToken = nullptr;

void ServerBrowserTAB::Render()
{
	if (ImGui::BeginTabItem("Server Browser")) {

		float btnWidth = 200.f;
		float btnHeight = 45.f;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - btnWidth);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.05f, 0.05f, 1.0f));         // Base
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));     // Hover
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.05f, 0.05f, 1.0f));
		if (ImGui::Button("Stuck on Loading? Fix It", ImVec2(btnWidth, btnHeight))) {
			if (browser) {
				auto* _menu = browser->fields.menuController;
				if (_menu) {
					app::Menu_SetLoading(_menu, false, nullptr);
					app::BoltNetwork_LoadScene(convert_to_system_string("Devour"), nullptr);
				}
			}
		}

		ImGui::PopStyleColor(3);


		// search box
		static ImGuiTextFilter filter;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
		ImGui::PushItemWidth(-1);
		filter.Draw(" Search servers...", 320);
		ImGui::PopItemWidth();

		ImGui::Spacing();
		ImGui::Separator();

		ImGui::Spacing();

		bool foundAny = false;
		for (size_t i = 0; i < g_ServerList.size(); ++i)
		{
			const auto& entry = g_ServerList[i];
			if (!filter.PassFilter(entry.name.c_str()))
				continue;

			foundAny = true;
			ImGui::PushID(static_cast<int>(i));

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(18, 18, 20, 255));
			ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(30, 30, 40, 150)); 

			ImGui::BeginChild("card", ImVec2(-1, 145), true,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			// Server name
			ImGui::AlignTextToFramePadding();
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
			ImGui::TextColored(ImVec4(1, 1, 1, 0.95f), entry.name.empty() ? "*<unknown>" : entry.name.c_str());
			ImGui::TextDisabled("Session ID:");
			ImGui::SameLine();
			ImGui::TextWrapped(entry.udpSessionId.c_str());

			ImGui::PopFont();

			float btnWidth = 120;
			float btnHeight = 35;
			ImGui::Spacing(); 

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.47f, 0.84f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.55f, 0.95f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.40f, 0.75f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); 

			if (ImGui::Button("Join", ImVec2(btnWidth, btnHeight))) {


				if (!entry.udpSessionId.empty() && browser && browser->fields.menuController) {
					SafePtr::safe_call([&]() {

						app::CSteamID lobby{};
						lobby.m_SteamID = entry.steamLobbyId;

						Il2CppDomain* domain = il2cpp_domain_get();

						const Il2CppAssembly* asmCS = il2cpp_domain_assembly_open(domain, "Assembly-CSharp.dll");
						const Il2CppImage* img = il2cpp_assembly_get_image(asmCS);

						Il2CppClass* klass = il2cpp_class_from_name(img,
							"Horror", "ServerConnectToken");

						Il2CppObject* obj = il2cpp_object_new(klass);
						app::ServerConnectToken* connect_token = reinterpret_cast<app::ServerConnectToken*>(obj);

						app::String* prefabRef = app::PlayerPrefs_GetString(convert_to_system_string("SurvivalPrefabId"), convert_to_system_string("Molly"), nullptr);

						connect_token->fields.isFromReconnect = true;
						connect_token->fields.isFromSteamInvite = false;
						connect_token->fields.packetVersion = (*app::RoomProtocolToken__TypeInfo)->static_fields->PACKET_VERSION;
						connect_token->fields.survivalPrefabIdPreference = prefabRef;

						app::ServerConnectToken__ctor(connect_token, nullptr);
						
						// DO_APP_FUNC(0x00B8F8F0, Byte__Array *, ProtocolTokenUtils_ToByteArray, (IProtocolToken * token, MethodInfo * method));

						if (auto* convertToToken = reinterpret_cast<app::IProtocolToken*>(connect_token))
						{
							app::Byte__Array* __byte = app::ProtocolTokenUtils_ToByteArray(convertToToken, nullptr);
							if (__byte)
							{
								auto* menu = browser->fields.menuController;
								
								menu->fields.steamLobbyID.m_SteamID = entry.steamLobbyId;
								menu->fields.boltSessionID = convert_to_system_string(entry.udpSessionId.c_str());

								app::Menu_StartServerJoin(browser->fields.menuController, nullptr);

								auto* __socket = app::BoltNetwork_get_UdpSocket(nullptr);

								if (__socket)
								{
									app::UdpSocket_Connect(__socket, entry.session, __byte, nullptr);
								}

							}
						}

						});
				}
			}

			ImGui::PopStyleColor(4);
			ImGui::EndChild();
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar(2);
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::PopID();
		}

		if (!foundAny) {
			ImGui::Spacing();

			ImGui::TextDisabled("No servers to show. Click 'Join Game' in the main menu to populate this list.");
			ImGui::Spacing();
		}




		ImGui::EndTabItem();
	}
}

void ServerBrowserTAB::CollectServers(app::Map_2_System_Guid_UdpSession_* node)
{
	if (!node) return;
	CollectServers(node->fields._left);

	auto session = node->fields._value; // app::UdpSession*
	if (session)
	{
		ServerEntry serverEntry{};

		auto photonSession = reinterpret_cast<app::PhotonSession*>(session);
		auto token = reinterpret_cast<app::PhotonRoomProperties*>(app::BoltUtils_GetProtocolToken(session, nullptr));

		if (!photonSession || !token || !token->klass) return;
		
		auto props = app::PhotonSession_get_Properties(photonSession, nullptr);
		if (!props || !props->klass || !props->fields._._entries) return;

		serverEntry.session = session;
		serverEntry.photonSession = photonSession;

		auto roomToken = reinterpret_cast<app::RoomProtocolToken*>(token);
		if (roomToken)
		{
			serverEntry.roomToken = roomToken;
			serverEntry.steamLobbyId = roomToken->fields.steamLobbyId;
		}


		for (int i = 0; i < props->fields._._entries->max_length; ++i)
		{
			auto& entry = props->fields._._entries->vector[i];
			if (entry.hashCode < 0 || !entry.key || !entry.key->klass)
				continue;
			auto keyStr = il2cppi_to_string(reinterpret_cast<Il2CppString*>(entry.key));
			auto il2cppObj = reinterpret_cast<Il2CppObject*>(entry.value);

			// Room name (RoomProtocolToken)
			if (keyStr == "RoomProtocolToken") {
				auto byteArrObj = reinterpret_cast<Il2CppArray*>(entry.value);
				if (byteArrObj && byteArrObj->max_length > 8) {
					uint8_t* data = reinterpret_cast<uint8_t*>(byteArrObj + 1);
					int offset = 3;

					std::string roomName;
					int nameStart = offset;

					for (; offset < byteArrObj->max_length; ++offset) {
						if (data[offset] == 0)
							break; 
						roomName += (char)data[offset]; // her byte'ı ekle (UTF-8 )
					}
					if (roomName.empty() && data[nameStart] == 0) offset = nameStart + 1;
					serverEntry.name = roomName;

					//ParseRoomProtocolToken(data, byteArrObj->max_length);
				}
			}

			// UdpSessionId
			if (keyStr == "UdpSessionId" && il2cppObj && il2cppObj->klass &&
				std::string(il2cppObj->klass->name) == "String") {
				serverEntry.udpSessionId = il2cppi_to_string(reinterpret_cast<Il2CppString*>(entry.value));
			}


		}

		if (!serverEntry.name.empty() && !serverEntry.udpSessionId.empty())
			g_ServerList.push_back(serverEntry);
	}

	CollectServers(node->fields._right);
}

void ServerBrowserTAB::ParseRoomProtocolToken(const uint8_t* data, size_t len)
{
	if(!data || len < 5) return;

	std::stringstream log;
	log << "[TOKEN PARSE] Length: " << len << " bytes\n";

	// 0–2: Header / unknown
	uint8_t header1 = data[0];
	uint8_t nameLenLow = data[1];
	uint8_t nameLenHigh = data[2];
	uint16_t nameLength = nameLenLow | (nameLenHigh << 8);

	log << "Header Byte: 0x" << std::hex << (int)header1 << "\n";
	log << "Name Length: " << std::dec << nameLength << "\n";

	if(3 + nameLength > len) {
		log << "[!] Room name exceeds token size. Abort.\n";
		std::cout << log.str();
		return;
	}

	std::string roomName;
	for(int i = 0; i < nameLength; ++i) {
		uint8_t ch = data[3 + i];
		if(ch >= 0x20 && ch <= 0x7E) {
			roomName += static_cast<char>(ch);
		}
		else {
			char buf[8];
			snprintf(buf, sizeof(buf), "\\x%02X", ch);
			roomName += buf;
		}
	}
	log << "Room Name: " << roomName << "\n";

	size_t offset = 3 + nameLength;
	if(len >= offset + 8) {
		uint64_t steamLobbyId = 0;
		memcpy(&steamLobbyId, data + offset, 8);

		log << "Possible Steam Lobby ID (next 8 bytes): 0x" << std::hex << steamLobbyId;
		if(steamLobbyId == 0)
			log << " [NULL or not set]";
		log << "\n";

		offset += 8;
	}
	else {
		log << "[!] Not enough bytes to read SteamLobbyId\n";
	}

	log << "\nRemaining Raw Data:\n";
	for(size_t i = offset; i < len; ++i) {
		log << std::setw(4) << std::setfill(' ') << std::right << i << ": ";
		log << std::setw(2) << std::setfill('0') << std::hex << (int)data[i] << "\n";
	}

	std::cout << log.str();
}

