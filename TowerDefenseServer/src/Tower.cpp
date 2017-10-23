#include <iostream>	
#include <vector>

#include "Tower.h"
#include "EnviormentUnit.h"
#include "Map.h"
#include "Helpers.h"

Tower::Tower(uint cooldown_ms, uint range, std::shared_ptr<SolidGroundTile> pos, Map* map) : 
_lastTimeStamp_ms(0), _cooldown_ms(cooldown_ms), _map(map), _position(pos), _range(range) {

}

Tower::~Tower(){

}



bool Tower::_CanFire(){
	if (_lastTimeStamp_ms == 0){
		_lastTimeStamp_ms = Helpers::MillisecondsTimeStamp();
		return true;
	}

	if (Helpers::TimeElapsed(_lastTimeStamp_ms, _cooldown_ms)){
		_lastTimeStamp_ms = Helpers::MillisecondsTimeStamp();
		return true;
	}

	return false;
}


std::shared_ptr<Projectile> Tower::Step(){
	if (_CanFire()){
		std::shared_ptr<EnviormentUnit> targetUnit = _GetTargetUnitInRange();
		if (targetUnit.get() != nullptr){
			return _Fire(targetUnit);
		}
		else
			return std::shared_ptr<Projectile>(nullptr);
	}

	return std::shared_ptr<Projectile>(nullptr);
}


std::shared_ptr<EnviormentUnit> Tower::_GetTargetUnitInRange(){
	std::vector<std::shared_ptr<EnviormentUnit>> units = _map->GetUnitsInRadius(_range, _position);
	if (units.size() == 0)
		return std::shared_ptr<EnviormentUnit>();

	//De todos los que tengo en rango agarro el primero que encuentro.
	return *units.begin();
}



std::shared_ptr<Projectile> Tower::_Fire(std::shared_ptr<EnviormentUnit> target){
	std::cout << "TOWER FIRE!!!\n" << std::flush;
	_lastTimeStamp_ms = Helpers::MillisecondsTimeStamp();
	return _BuildProjectile(target.get()->GetPosition());
}
