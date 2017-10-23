#include <iostream>
#include <memory>

#include "Towers/WindProjectile.h"
#include "Towers/Projectile.h"
#include "Map/PathTile.h"
#include "EnviormentUnits/EnviormentUnit.h"

WindProjectile::WindProjectile
(std::shared_ptr<SolidGroundTile> origin, std::shared_ptr<PathTile> target, uint hitpoints) :
 Projectile(origin, target, 50, hitpoints)
{

}

WindProjectile::~WindProjectile(){}



void WindProjectile::_OnImpact(){
	std::vector<std::shared_ptr<EnviormentUnit>> units = _target->GetUnits();
	bool hit = false;
	for (auto it = units.begin(); it != units.end() && !hit; ++it){
		if ((*it).get()->Flies()){
			(*it).get()->GetHit(_hitPoints);
			hit = true;
		}
	}

	if (!hit)
		std::cout << "Projectile missed!!\n" << std::flush;


}