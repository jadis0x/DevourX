#pragma once
#include <vector>
#include <string>

struct ServerEntry {
	std::string name;
	std::string udpSessionId;
	app::UdpSession* session;
	app::PhotonSession* photonSession;
	app::RoomProtocolToken* roomToken;
	uint64_t steamLobbyId;
};

extern std::vector<struct ServerEntry> g_ServerList;
extern app::ServerBrowser* browser;

extern app::ServerConnectToken* g_connectToken;

namespace ServerBrowserTAB {
	void Render();
	void CollectServers(app::Map_2_System_Guid_UdpSession_* node);

	void ParseRoomProtocolToken(const uint8_t* data, size_t len);
}