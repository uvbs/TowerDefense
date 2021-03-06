#include <ctime>
#include <iostream>

#include "../../../include/GameModel/Towers/GroundTower.h"
#include "../../../include/GameModel/Map/SolidGroundTile.h"
#include "../../../include/GameModel/Map/PathTile.h"
#include "../../../include/GameModel/Commands/CastSpellCommand.h"
#include "../../../include/GameModel/Towers/Tower.h"
#include "../../../include/GameModel/EnviormentUnits/EnviormentUnit.h"
#include "../../../include/GameModel/TowerDefenseGame.h"
#include "../../../include/GameModel/ViewModels/CommandVM.h"
#include "../../../include/GameModel/GameNotifications/SpellCastedGameNotification.h"

CastSpellCommand::CastSpellCommand(CAST_SPELL_TYPE spell, uint x, uint y, uint cooldown_ms, PlayerProxy& player):
 _cooldown_ms(cooldown_ms), _xPos(x), _yPos(y), _unitId(-1), _spell(spell), _player(player) {}

CastSpellCommand::CastSpellCommand(CAST_SPELL_TYPE spell, uint unitId, uint cooldown_ms, PlayerProxy& player):
 _cooldown_ms(cooldown_ms), _xPos(-1), _yPos(-1), _unitId(unitId), _spell(spell), _player(player) {}

CastSpellCommand::~CastSpellCommand(){}

bool CastSpellCommand::Execute(Map* map, TowerDefenseGame* game, ThreadSafeQueue<GameNotification*>& notifications){
	switch(_spell){
		case SPELL_TERRAFORMING: return _CastTerraforming(map, game);
		case SPELL_GRIETA: return _CastGrieta(map, game);
		case SPELL_METEORITO: return _CastMeteorito(map, game);
		case SPELL_FIREWALL: return _CastMuroDeFuego(map, game);
		case SPELL_CONGELACION: return _CastCongelacion(map, game);
		case SPELL_VENTISCA: return _CastVentisca(map, game);
		case SPELL_TORNADO: return _CastTornado(map, game);
		case SPELL_RAYO: return _CastRayos(map, game);
		case SPELL_PING: return _CastPing(map, game);
		default: return false;
	}
}

bool CastSpellCommand::_CastTerraforming(Map* map, TowerDefenseGame* game){
	PathTile* pathtile = map->GetPathTile(_xPos, _yPos);
	SolidGroundTile* tile = map->GetSolidGroundTile(_xPos, _yPos);
	if (tile != nullptr || pathtile != nullptr)
		return false;
	map->PlaceGroundTile(new SolidGroundTile(_xPos,_yPos));
	game->notifications.Queue(new SpellCastedGameNotification(SPELL_TERRAFORMING, _xPos, _yPos,0, _cooldown_ms));
	game->clientCooldownManager->SpellCasted(SPELL_TERRAFORMING);
	return true;
}

bool CastSpellCommand::_CastGrieta(Map* map, TowerDefenseGame* game){
	PathTile* tile = map->GetPathTile(_xPos, _yPos);
	if (tile != nullptr){
		uint32_t time_ms = game->GameCfg->Cfg["spells"]["grieta"]["duration_sec"].as<uint>();
		time_ms *= 1000;
		tile->Crack(time_ms);
		game->notifications.Queue(new SpellCastedGameNotification(SPELL_GRIETA, _xPos, _yPos, time_ms, _cooldown_ms ));
		game->clientCooldownManager->SpellCasted(SPELL_GRIETA);
		return true;
	}
	return false;
}

void MeteoritoKillAfterDelay(){

}

bool CastSpellCommand::_CastMeteorito(Map* map, TowerDefenseGame* game){
	uint collateralDamageRange = game->GameCfg->Cfg["spells"]["meteorito"]["collateral_range"].as<uint>();
	uint collateralDamage = game->GameCfg->Cfg["spells"]["meteorito"]["collateral_damage"].as<uint>();
	uint targetDamage = game->GameCfg->Cfg["spells"]["meteorito"]["damage"].as<uint>();
	PathTile* tile = map->GetPathTile(_xPos, _yPos);
	if (tile != nullptr){
		game->notifications.Queue(new SpellCastedGameNotification(SPELL_METEORITO, _xPos, _yPos, 1500, _cooldown_ms ));
		game->clientCooldownManager->SpellCasted(SPELL_METEORITO);

		std::vector<PathTile*> tiles = map->GetPathTilesInRange(tile, collateralDamageRange);
		tile->HitUnitsAfterDelay(450, targetDamage);

		for (auto it = tiles.begin(); it != tiles.end(); ++it){
			if (*it != tile)
				(*it)->HitUnitsAfterDelay(450, collateralDamage);
		}
		return true;
	}
	return false;
}

bool CastSpellCommand::_CastMuroDeFuego(Map* map, TowerDefenseGame* game){
	uint fireDuration_sec = game->GameCfg->Cfg["spells"]["murodefuego"]["duration_sec"].as<uint>();
	uint fireDamage = game->GameCfg->Cfg["spells"]["murodefuego"]["damage"].as<uint>();
	PathTile* tile = map->GetPathTile(_xPos, _yPos);
	if (tile != nullptr){
		tile->SetOnFire(fireDuration_sec * 1000, fireDamage);
		game->notifications.Queue(new SpellCastedGameNotification(SPELL_FIREWALL, _xPos, _yPos, fireDuration_sec * 1000, _cooldown_ms ));
		game->clientCooldownManager->SpellCasted(SPELL_FIREWALL);
		return true;
	}
	return false;
}

bool CastSpellCommand::_CastCongelacion(Map* map, TowerDefenseGame* game){
	uint freezeDurationSec = game->GameCfg->Cfg["spells"]["congelacion"]["duration_sec"].as<uint>();
	EnviormentUnit* unit = nullptr;
	PathTile* tile = map->GetPathTile(_xPos, _yPos);
	if (tile == nullptr)
		return false;

	std::vector<EnviormentUnit*> unitsInTile = tile->GetUnits();

	if (unitsInTile.size() == 0)
		return false;

	unit = *(unitsInTile.begin());

	if (unit != nullptr){
		unit->Freeze(freezeDurationSec);
		game->notifications.Queue(new SpellCastedGameNotification(SPELL_CONGELACION, _xPos, _yPos, freezeDurationSec * 1000 , _cooldown_ms));
		game->clientCooldownManager->SpellCasted(SPELL_CONGELACION);
		return true;
	}
	return false;
}

bool CastSpellCommand::_CastVentisca(Map* map, TowerDefenseGame* game){
	uint ventiscaDamage = game->GameCfg->Cfg["spells"]["ventisca"]["damage"].as<uint>();
	uint percentSlow = game->GameCfg->Cfg["spells"]["ventisca"]["slow_percent"].as<uint>();
	uint slowDuration_sec = game->GameCfg->Cfg["spells"]["ventisca"]["slow_duration_sec"].as<uint>();
	uint ventiscaDuration_sec = game->GameCfg->Cfg["spells"]["ventisca"]["duration_sec"].as<uint>();
	PathTile* tile = map->GetPathTile(_xPos, _yPos);
	if (tile != nullptr){
		tile->Ventisca(ventiscaDamage, ventiscaDuration_sec, slowDuration_sec, percentSlow);
		game->notifications.Queue(new SpellCastedGameNotification(SPELL_VENTISCA, _xPos, _yPos, ventiscaDuration_sec * 1000, _cooldown_ms ));
		game->clientCooldownManager->SpellCasted(SPELL_VENTISCA);
		return true;
	}
	return false;
}

bool CastSpellCommand::_CastTornado(Map* map, TowerDefenseGame* game){
	uint tornadoMaxDamage = game->GameCfg->Cfg["spells"]["tornado"]["max_damage"].as<uint>();;
	uint tornadoDuration_sec = game->GameCfg->Cfg["spells"]["tornado"]["duration_sec"].as<uint>();;
	PathTile* tile = map->GetPathTile(_xPos, _yPos);
	if (tile != nullptr){
		tile->Tornado(tornadoMaxDamage, tornadoDuration_sec);
		game->notifications.Queue(new SpellCastedGameNotification(SPELL_TORNADO, _xPos, _yPos, tornadoDuration_sec * 1000, _cooldown_ms));
		game->clientCooldownManager->SpellCasted(SPELL_TORNADO);
		return true;
	}
	return false;
}

bool CastSpellCommand::_CastRayos(Map* map, TowerDefenseGame* game){
	uint rayoMaxDamage = game->GameCfg->Cfg["spells"]["rayo"]["max_damage"].as<uint>();
	EnviormentUnit* unit = nullptr;
	PathTile* tile = map->GetPathTile(_xPos, _yPos);
	if (tile == nullptr)
		return false;

	std::vector<EnviormentUnit*> unitsInTile = tile->GetUnits();

	if (unitsInTile.size() == 0)
		return false;

	unit = *(unitsInTile.begin());
	if (unit != nullptr){
		std::srand(std::time(0));
		uint randomDamage = (uint) std::rand() % rayoMaxDamage;
		unit->GetHit(randomDamage);
		game->notifications.Queue(new SpellCastedGameNotification(SPELL_RAYO, _xPos, _yPos, 2000, _cooldown_ms));
		game->clientCooldownManager->SpellCasted(SPELL_RAYO);
		return true;
	}
	return false;
}

bool CastSpellCommand::_CastPing(Map* map, TowerDefenseGame* game){
	if (!map->AreCoordinatesInsideMap(_xPos, _yPos))
		return false;
	game->clientCooldownManager->PingSet(_player);
	game->notifications.Queue(new SpellCastedGameNotification(SPELL_PING, _xPos, _yPos, 2000, _cooldown_ms));
}
