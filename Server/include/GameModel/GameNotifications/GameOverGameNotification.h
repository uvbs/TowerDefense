//
// Created by tino on 13/11/17.
//

#ifndef TOWERDEFENSE_GAMEOVERGAMENOTIFICATION_H
#define TOWERDEFENSE_GAMEOVERGAMENOTIFICATION_H

#include "../../PlayerProxy.h"
#include "GameNotification.h"

class GameOverGameNotification : public GameNotification
{
public:
    GameOverGameNotification(std::vector<PlayerProxy*> playersToNotify);
    ~GameOverGameNotification();
    void Notify();
};


#endif //TOWERDEFENSE_GAMEOVERGAMENOTIFICATION_H