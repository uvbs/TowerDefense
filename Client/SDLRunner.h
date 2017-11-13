#ifndef TOWERDEFENSE_SDLRUNNER_H
#define TOWERDEFENSE_SDLRUNNER_H

#include <thread>
#include "View/Common/Window.h"
#include "View/Common/Renderer.h"
#include "View/Common/TextureLoader.h"
#include "View/Model/MapView.h"
#include "View/Common/SpriteNamesConfig.h"
#include "View/Model/PortalEntradaView.h"
#include "View/Model/PortalSalidaView.h"
#include "View/Model/TowerView.h"
#include "View/Model/UnitView.h"
#include "include/NotificationReciever.h"
#include "include/NetCommands/CommandDispatcher.h"
#include "include/ClientSocket.h"
#include "include/SDLNotifications/SDLNotification.h"
#include "include/NonBlockingThreadSafeQueue.h"

#define TITLE "Tower Defense"

#define WINDOWWIDTH 640
#define WINDOWHEIGHT 480

#define MAPSIZE 7

class SDLRunner {
private:
    CommandDispatcher* _dispatcher;
    NotificationReciever* _reciever;
    ClientLobbyManager* _lobbyManager;
    ClientSocket* _sock;
public:
    SDLRunner();
    ~SDLRunner();
    void Run(CommandDispatcher* dispatcher, NotificationReciever* reciever, ClientLobbyManager* lobbyManager, ClientSocket* sock);
};


#endif