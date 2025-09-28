#pragma once
#include <vector>
#include <string>

extern std::vector<struct ServerEntry> g_ServerList;

struct ServerEntry {
	std::string name;
	std::string udpSessionId;
	app::UdpSession* session;
	app::PhotonSession* photonSession;
	app::RoomProtocolToken* roomToken;
	uint64_t steamLobbyId;
};

namespace ServerBrowserTAB {
	void Render();
	void CollectServers(app::Map_2_System_Guid_UdpSession_* node);
}

