#include "../include/NotificationReciever.h"
#include "../GTKmm/GTKmmRunner.h"
#include "../include/GTKNotifications/LogInSuccessGtkNotification.h"
#include "../include/GTKNotifications/LogInFailedGtkNotification.h"
#include "../include/GTKNotifications/GameStartedGTKNotification.h"
#include "../View/Common/SpriteNamesConfig.h"
#include "../include/Exceptions/UnknownOpcodeException.h"
#include "../include/NetCommands/PlayerLoadedGameCommand.h"
#include "../include/GTKNotifications/SpellIsFreeGTKNotification.h"
#include "../include/GTKNotifications/PlayerIsReadyGTKNotification.h"
#include "../include/GTKNotifications/LobbyIsGoneGTKNotification.h"
#include "../include/GTKNotifications/ServerConnectionLostGTKNotification.h"

NotificationReciever::NotificationReciever(SocketWrapper& socket, ClientLobbyManager& lobbyManager,
                                           GTKmmRunner& runner, CommandDispatcher& dispatcher)
: _sock(socket), _lobbyManager(lobbyManager),  _runner(runner),
  _dispatcher(dispatcher), _stop(false), _towerCoordToId(), _localTowerId() {

}

NotificationReciever::~NotificationReciever(){
	if (_thread.joinable())
		_thread.join();
}

void NotificationReciever::Run(){
	_thread = std::thread(&NotificationReciever::RecieveNotifications, this);
}



void NotificationReciever::_SwitchOnOpcode(uint8_t opcode){
    std::unique_lock<std::mutex> lock(_stopMutex);
    if (_stop)
        return;
    switch (opcode){
        case CREATE_LOBBY:
            std::cout << "CREATE_LOBBY::\n" << std::flush;
            _lobbyManager.HandleNewLobbyNotification();
            break;
        case JOIN_LOBBY:
            std::cout << "JOIN_LOBBY::\n" << std::flush;
            _lobbyManager.HandleLobbyJoin();
            break;
        case PLAYER_LEFT_LOBBY:
            std::cout << "PLAYER_LEFT_LOBBY::\n" << std::flush;
            _lobbyManager.HandlePlayerLeftLobby();
            break;
        case PLAYER_JOINED_LOBBY:
            std::cout << "PLAYER_JOINED_LOBBY::\n" << std::flush;
            _lobbyManager.HandlePlayerJoinedLobby();
            break;
        case PLAYER_JOIN:
            std::cout << "PLAYER_JOIN::\n" << std::flush;
            _lobbyManager.HandlePlayerJoin();
            break;
        case PLAYER_LEAVE:
            std::cout << "PLAYER_LEAVE::\n" << std::flush;
            _lobbyManager.HandlePlayerLeave();
            break;
        case LOG_IN_SUCCESS:
        {
            std::cout << "LOG_IN_SUCCESS::\n" << std::flush;
            _lobbyManager.HandleLoginSuccess();
            break;
        }
        case PLAYER_IS_READY:
        {
            _lobbyManager.HandlePlayerReady();
            break;
        }
        case PICK_SPELL:
            std::cout << "PICK_SPELL::\n" << std::flush;
            _lobbyManager.HandlePickedSpell();
            break;
        case SPELL_IS_FREE:
        {
            std::cout << "SPELL_IS_FREE::\n" << std::flush;
            SPELL_TYPE type = (SPELL_TYPE) _sock.RecieveByte();
            _runner.gtkNotifications.Queue(new SpellIsFreeGTKNotification(type));
            g_idle_add(GTKmmRunner::notification_check, &_runner);
            break;
        }
        case LOBBY_GONE:
        {
            uint32_t lobbyGuid = _sock.RecieveInt32();
            _runner.gtkNotifications.Queue(new LobbyIsGoneGTKNotification(lobbyGuid));
            g_idle_add(GTKmmRunner::notification_check, &_runner);
            break;
        }
        case PICK_MAP:
            std::cout << "PICK_MAP::\n" << std::flush;
            _lobbyManager.HandleMapPicked();
            break;
        case PLAYER_PICKED_SPELL:
            std::cout << "PLAYER_PICKED_SPELL::\n" << std::flush;
            _lobbyManager.HandleOtherPlayerPickedSpell();
            break;
        case LOG_IN_FAILED:
            std::cout << "LOG_IN_FAILED::\n" << std::flush;
            _runner.gtkNotifications.Queue(new LogInFailedGtkNotification());
            g_idle_add(GTKmmRunner::notification_check, &_runner);
            break;
        case GAME_STARTED:
        {
            std::cout << "GAME_STARTED::\n" << std::flush;
            uint8_t superficie = _sock.RecieveByte();
            uint32_t width = _sock.RecieveInt32();
            uint32_t height = _sock.RecieveInt32();

            _runner.lobbyManager->fireHUDEnabled = _sock.RecieveByte();
            _runner.lobbyManager->waterHUDEnabled = _sock.RecieveByte();
            _runner.lobbyManager->airHUDEnabled = _sock.RecieveByte();
            _runner.lobbyManager->groundHUDEnabled = _sock.RecieveByte();

            _runner.gtkNotifications.Queue(new GameStartedGTKNotification(superficie, width, height));
            g_idle_add(GTKmmRunner::notification_check, &_runner);
            break;
        }

        case GAME_OPCODE:
            std::cout << "GAME_OPCODE::\n" << std::flush;
            _HandleGameOpcode();
            break;
        case GAME_MODEL_STARTED_RUNNING:
            std::cout << "GAME_MODEL_STARTED_RUNNING::\n" << std::flush;
            _dispatcher.Enable();
            break;
        case LOAD_MAP:
        {
            std::cout << "LOAD_MAP::\n" << std::flush;
            uint8_t op = _sock.RecieveByte();
            uint32_t x = _sock.RecieveInt32();
            uint32_t y = _sock.RecieveInt32();
            if (op == PATH_TILE)
                model_view->createPathTile(x, y);
            if (op == STRUCTURE_TILE)
                model_view->createStructureTile(x, y);
            if (op == SPAWN_TILE)
                model_view->createPortalEntrada(x, y);
            if (op == FINISH_TILE)
                model_view->createPortalSalida(x, y);
            break;
        }

        case MAP_FINISHED_LOADING:
        {
            std::cout << "MAP_FINISHED_LOADING::\n" << std::flush;
            std::lock_guard<std::mutex> lock(model_view->mapLoadedMutex);
            model_view->mapLoaded = true;
            model_view->mapLoadedCondVariable.notify_one();
            _dispatcher.QueueCommand(new PlayerLoadedGameCommand());
            _dispatcher.Disable();
            model_view->addAnnouncement("Welcome to Tower Defense!");
        }
            break;
        case IN_GAME_CHAT_MESSAGE:
        {
            std::cout << "IN_GAME_CHAT_MESSAGE::\n" << std::flush;
            uint32_t pguid = _sock.RecieveInt32();
            std::string message = _sock.RecieveString();

            if (pguid == _lobbyManager.myGuid){
                chat_view->MessageFrom(message, _lobbyManager.myName);
            } else {
                std::string playerName = _lobbyManager.GetPlayerName(pguid);
                chat_view->MessageFrom(message, playerName);
            }
            break;
        }
        default:
            throw UnknownOpcodeException();
    }
}

void NotificationReciever::RecieveNotifications(){
    try
    {
        while (!_Stop()){

            {
                uint8_t byte = _sock.RecieveByte();
                _SwitchOnOpcode(byte);
            }

        }
    }catch(const std::exception& e)
    {
        if (!_runner.OK){
            _runner.gtkNotifications.Queue(new ServerConnectionLostGTKNotification());
            g_idle_add(GTKmmRunner::notification_check, &_runner);
        } else {
            model_view->addAnnouncement("Connection Lost!");
        }

        std::cerr << e.what() << std::endl;
    }
}

void NotificationReciever::_HandleGameOpcode(){
	switch (_sock.RecieveByte()) {
		case TOWER_PLACED:
			std::cout << "TOWER_PLACED::\n" << std::flush;
			_HandleTowerPlaced();
			break;
		case UNIT_POSITION_UPDATE:
			std::cout << "UNIT_POSITION_UPDATE::\n" << std::flush;
			_HandleUnitPositionUpdate();
			break;
        case UNIT_CREATED:
			std::cout << "UNIT_CREATED::\n" << std::flush;
			_HandleUnitCreated();
            break;
        case PROJECTILE_FIRED:
			std::cout << "PROJECTILE_FIRED::\n" << std::flush;
			_HandleProjectileFired();
            break;
		case GAME_OVER:
            std::cout << "GAME_OVER::\n" << std::flush;
            model_view->gameOver();
            _stop = true;
            break;
        case GAME_WON:
            std::cout << "GAME_WON::\n" << std::flush;
            model_view->win();
            _stop = true;
            break;
        case UNIT_DIED:
        {
            std::cout << "UNIT_DIED::\n" << std::flush;
            uint32_t unitid = _sock.RecieveInt32();
            model_view->killUnit(unitid);
            break;
        }

		case CLIENT_CAST_SPELL:
        {
            std::cout << "CLIENT_CAST_SPELL::\n" << std::flush;
            _HandleSpellCasted();
            break;
        }
		case HORDE_STARTED:
			std::cout << "HORDE_STARTED::\n" << std::flush;
			_HandleHordeStarted();
			break;
		case HORDE_ENDED:
			std::cout << "HORDE_ENDED::\n" << std::flush;
			_HandleHordeEnded();
			break;
		case TOWER_GAINED_EXP:
			std::cout << "TOWER_GAINED_EXP::\n" << std::flush;
			_HandleTowerGainedExperience();
			break;
        case TOWER_UPGRADE:
            std::cout << "TOWER_UPGRADE::\n" << std::flush;
            _HandleTowerUpgrade();
            break;
        case UNIT_SPEED_UPDATE:
            std::cout << "UNIT_SPEED_UPDATE::\n" << std::flush;
            _HandleUnitSpeedUpdate();
            break;
        case UNIT_FROZEN:
            std::cout << "UNIT_FROZEN::\n" << std::flush;
            _HandleUnitFrozen();
            break;
    }
}

void NotificationReciever::_HandleTowerGainedExperience() {
	uint32_t x = _sock.RecieveInt32();
	uint32_t y = _sock.RecieveInt32();
	uint32_t exp = _sock.RecieveInt32();

	uint towerId = _towerCoordToId[std::pair<uint, uint>(x, y)];
    TowerView* tower= model_view->getTower(towerId);
    tower->setExp(exp);
    hud_view->setLastTowerId(towerId);
}

void NotificationReciever::_HandleTowerUpgrade() {
    uint32_t x = _sock.RecieveInt32();
    uint32_t y = _sock.RecieveInt32();
    uint32_t damage = _sock.RecieveInt32();
    uint32_t range = _sock.RecieveInt32();
    uint32_t projectile_ms_over_tile = _sock.RecieveInt32();
    uint32_t level = _sock.RecieveInt32();

	uint towerId = _towerCoordToId[std::pair<uint, uint>(x, y)];


    uint32_t exp_required_for_damage_upgrade = _sock.RecieveInt32();
    uint32_t exp_required_for_range_upgrade = _sock.RecieveInt32();

    TowerView* tower= model_view->getTower(towerId);
    tower->setDamage(damage);
    tower->setRange(range);
    tower->setFrequency(projectile_ms_over_tile);
    tower->setLevel(level);
    tower->setUpgradeExperienceDamage(exp_required_for_damage_upgrade);
    tower->setUpgradeExperienceRange(exp_required_for_range_upgrade);

	uint8_t type = _sock.RecieveByte();
    switch (type){
        case SPELL_TYPE_FIRE:
        {
            uint32_t collateral_damage = _sock.RecieveInt32();
            uint32_t collateral_range = _sock.RecieveInt32();
            tower->setCollateralDamage(collateral_damage);
            tower->setCollateralRange(collateral_range);
            uint32_t exp_required_for_collateral_range_upgrade = _sock.RecieveInt32();
            tower->setUpgradeExperienceCollateralRange(exp_required_for_collateral_range_upgrade);
            break;
        }
        case SPELL_TYPE_WATER:
        {
            uint32_t slow_seconds = _sock.RecieveInt32();
            uint32_t slow_percent = _sock.RecieveInt32();
            tower->setFreezePercent(slow_percent);
            tower->setFreezeDuration(slow_seconds);
            uint32_t exp_required_for_slow_upgrade = _sock.RecieveInt32();
            tower->setUpgradeExperienceSlow(exp_required_for_slow_upgrade);
            break;
        }
        case SPELL_TYPE_AIR:
        {
            uint32_t nonFlyingDamage = _sock.RecieveInt32();
            tower->setDamage(nonFlyingDamage);
            tower->setFlyDamage(damage);
            break;
        }
        case SPELL_TYPE_GROUND:
            break;
    }
}

void NotificationReciever::_HandleUnitSpeedUpdate() {
    uint unitId = _sock.RecieveInt32();
    uint step_delay_ms = _sock.RecieveInt32();
    model_view->setUnitSpeed(unitId, step_delay_ms);
}

void NotificationReciever::_HandleUnitFrozen() {
    uint unitId = _sock.RecieveInt32();
    uint freeze_time_ms = _sock.RecieveInt32();
    model_view->freezeUnit(unitId, freeze_time_ms);
}



void NotificationReciever::_HandleHordeEnded() {
	uint hordeId = _sock.RecieveInt32();
	std::string s = "Horde " +  std::to_string(hordeId) + " won!";
	model_view->addAnnouncement(s);
}
void NotificationReciever::_HandleHordeStarted() {
	uint hordeId = _sock.RecieveInt32();
	std::string s = "Horde " +  std::to_string(hordeId) + " started!";
	model_view->addAnnouncement(s);
}

void NotificationReciever::_HandleSpellCasted(){
	uint8_t spell = _sock.RecieveByte();
	uint32_t x = _sock.RecieveInt32();
	uint32_t y = _sock.RecieveInt32();
    uint32_t duration_ms = _sock.RecieveInt32();
    uint32_t cooldown_ms = _sock.RecieveInt32();
	switch(spell){
		case SPELL_TERRAFORMING:
            hud_view->setCooldown(CMD_TERRAFORMING, cooldown_ms);
			model_view->createStructureTile(x, y);
			break;
		case SPELL_METEORITO:
            hud_view->setCooldown(CMD_METEORITO, cooldown_ms);
			model_view->createSpell(METEORITO, x, y, duration_ms);
			break;
		case SPELL_VENTISCA:
            hud_view->setCooldown(CMD_VENTISCA, cooldown_ms);
			model_view->createSpell(VENTISCA, x, y, duration_ms);
			break;
		case SPELL_CONGELACION:
            hud_view->setCooldown(CMD_CONGELACION, cooldown_ms);
			model_view->createSpell(CONGELACION, x, y, duration_ms);
			break;
		case SPELL_TORNADO:
            hud_view->setCooldown(CMD_TORNADO, cooldown_ms);
			model_view->createSpell(TORNADO, x, y, duration_ms);
			break;
		case SPELL_RAYO:
            hud_view->setCooldown(CMD_RAYO, cooldown_ms);
			model_view->createSpell(RAYO, x, y, duration_ms);
			break;
		case SPELL_FIREWALL:
            hud_view->setCooldown(CMD_FIREWALL, cooldown_ms);
			model_view->createSpell(FIREWALL, x, y, duration_ms);
			break;
		case SPELL_GRIETA:
            hud_view->setCooldown(CMD_GRIETA, cooldown_ms);
			model_view->createSpell(GRIETA, x, y, duration_ms);
			break;
		case SPELL_PING:
			model_view->createSpell(PING, x, y, duration_ms);
			break;
	}
}


void NotificationReciever::_HandleProjectileFired(){
	uint32_t x = _sock.RecieveInt32();
	uint32_t y = _sock.RecieveInt32();
	uint32_t tox = _sock.RecieveInt32();
	uint32_t toy = _sock.RecieveInt32();
	uint32_t delay_ms = _sock.RecieveInt32();
	uint8_t spelltype = _sock.RecieveByte();
	switch(spelltype){
		case SPELL_TYPE_WATER:
			model_view->createShot(DISPARO_AGUA, x, y, tox, toy, delay_ms);
			break;
		case SPELL_TYPE_GROUND:
			model_view->createShot(DISPARO_TIERRA, x, y, tox, toy, delay_ms);
			break;
		case SPELL_TYPE_FIRE:
			model_view->createShot(DISPARO_FUEGO, x, y, tox, toy, delay_ms);
			break;
		case SPELL_TYPE_AIR:
			model_view->createShot(DISPARO_AIRE, x, y, tox, toy, delay_ms);
			break;
	}
}
void NotificationReciever::_HandleTowerPlaced(){
	uint32_t x = _sock.RecieveInt32();
	uint32_t y = _sock.RecieveInt32();
    uint32_t cooldown_ms = _sock.RecieveInt32() * 1000;
	uint8_t type = _sock.RecieveByte();
	_towerCoordToId[std::pair<uint, uint>(x, y)] = _localTowerId;
    switch (type) {
        case SPELL_TYPE_GROUND:
            hud_view->setCooldown(CMD_EARTH_TOWER, cooldown_ms);
            model_view->createTower(_localTowerId++, TORRE_TIERRA , x, y);
            break;
        case SPELL_TYPE_FIRE:
            hud_view->setCooldown(CMD_FIRE_TOWER, cooldown_ms);
            model_view->createTower(_localTowerId++, TORRE_FUEGO , x, y);
            break;
        case SPELL_TYPE_WATER:
            hud_view->setCooldown(CMD_WATER_TOWER, cooldown_ms);
            model_view->createTower(_localTowerId++, TORRE_AGUA , x, y);
            break;
        case SPELL_TYPE_AIR:
            hud_view->setCooldown(CMD_AIR_TOWER, cooldown_ms);
            model_view->createTower(_localTowerId++, TORRE_AIRE, x, y);
            break;
    }
}
void NotificationReciever::_HandleUnitPositionUpdate(){
    uint32_t unitID = _sock.RecieveInt32();
    uint32_t x = _sock.RecieveInt32();
    uint32_t y = _sock.RecieveInt32();
    uint32_t tox = _sock.RecieveInt32();
    uint32_t toy = _sock.RecieveInt32();

	if (tox == 0xFFFFFFFF || toy == 0xFFFFFFFF)
		return;

	model_view->moveUnit(unitID, x, y, tox, toy);
}
void NotificationReciever::_HandleUnitCreated(){
    uint32_t unitID = _sock.RecieveInt32();
    uint32_t x = _sock.RecieveInt32();
    uint32_t y = _sock.RecieveInt32();
    uint32_t tox = _sock.RecieveInt32();
    uint32_t toy = _sock.RecieveInt32();
    uint32_t delay_ms = _sock.RecieveInt32();
    uint8_t unittype = _sock.RecieveByte();
    UNIT_TYPE type = (UNIT_TYPE) unittype;
    switch(type){
        case UNIT_TYPE_Abmonible:
            model_view->createUnit(unitID, ABOMINABLE, x, y, tox, toy, delay_ms);
            break;
        case UNIT_TYPE_NoMuerto:
            model_view->createUnit(unitID, NOMUERTO, x, y, tox, toy, delay_ms);
            break;
        case UNIT_TYPE_HalconSangriento:
            model_view->createUnit(unitID, HALCONSANGRIENTO, x, y, tox, toy, delay_ms);
            break;
        case UNIT_TYPE_HombreCabra:
            model_view->createUnit(unitID, HOMBRECABRA, x, y, tox, toy, delay_ms);
            break;
        case UNIT_TYPE_DemonioVerde:
            model_view->createUnit(unitID, DEMONIOVERDE, x, y, tox, toy, delay_ms);
            break;
        case UNIT_TYPE_Espectro:
            model_view->createUnit(unitID, ESPECTRO, x, y, tox, toy, delay_ms);
            break;
    }
}


bool NotificationReciever::_Stop(){
	std::lock_guard<std::mutex> lock(_stopMutex);
	return _stop;
}

bool NotificationReciever::Running(){
	std::lock_guard<std::mutex> lock(_stopMutex);
	return !_stop;
}


void NotificationReciever::Stop(){
    std::cout << "TRYING TO STOP\n" << std::flush;
	std::lock_guard<std::mutex> lock(_stopMutex);
	_stop = true;
    std::cout << "STOPPED\n" << std::flush;

}