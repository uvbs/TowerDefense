#ifndef __LOGGED_IN_NOTIFICATION__
#define __LOGGED_IN_NOTIFICATION__

#include <stdint.h>
#include <vector>
#include "../Notifications/Notification.h"
#include "../PlayerProxy.h"

class LoggedInNotification : public Notification 
{

private:
	PlayerProxy& _player;
	std::vector<Lobby*> _lobbies;
	std::vector<std::tuple<uint32_t, uint32_t>> _lobbies2playersGUIDS;
public:

	LoggedInNotification(PlayerProxy& player,
						 std::vector<Lobby*>& lobbies,
						 std::vector<std::tuple<uint32_t, uint32_t>>& lobbies2playersGUIDS);
	~LoggedInNotification();

	void Notify();
	void SetPlayersToNotify(std::vector<PlayerProxy*>* players);
	
};

#endif