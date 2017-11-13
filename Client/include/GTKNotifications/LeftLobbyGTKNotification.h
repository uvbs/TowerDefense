#ifndef __LEFT_LOBBY_GUI_NOTIF__
#define __LEFT_LOBBY_GUI_NOTIF__

#include <iostream>
#include <vector>
#include "../GTKNotifications/GTKNotification.h"
#include "../Lobbies/Lobby.h"
class GTKRunner;

class LeftLobbyGTKNotification : public GTKNotification {
public:
	LeftLobbyGTKNotification();
	~LeftLobbyGTKNotification();
	void Execute(GTKRunner& runner);
};

#endif