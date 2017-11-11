#ifndef __UPDATE_LOBBY_PLAYERS_NOTIF__
#define __UPDATE_LOBBY_PLAYERS_NOTIF__

#include <iostream>
#include "../SDLNotifications/SDLNotification.h"
#include <string>
#include <vector>

class UpdateLobbyPlayersGUINotification : public SDLNotification {
private:
	std::vector<std::string> _playerNames;
public:
	UpdateLobbyPlayersGUINotification(std::vector<std::string>& playerNames);
	~UpdateLobbyPlayersGUINotification();
	void Execute();
};

#endif