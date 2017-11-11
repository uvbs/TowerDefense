#ifndef __CLIENT_LOBBY_MANAGER__
#define __CLIENT_LOBBY_MANAGER__

#include <vector>
#include <string>
#include <mutex>
#include <iostream>

#include "Lobby.h"
#include "../../../Common/SocketWrapper.h"

class GTKRunner;

class ClientLobbyManager{
private:
	SocketWrapper& _sock;
	std::vector<Lobby*> _lobbies;
	Lobby* _joinedLobby;
	GTKRunner& _runner;
public:
	ClientLobbyManager(SocketWrapper& sock, GTKRunner& GtkRunner);
	~ClientLobbyManager();
	void HandleNotification();
	void _HandleNewLobbyNotification();
	void _HandleGetLobbies();
	void _HandleLobbyJoin();
	void _HandlePlayerLeftLobby();
	void _HandlePlayerJoinedLobby();
	void _HandleLeaveLobby();
};

#endif