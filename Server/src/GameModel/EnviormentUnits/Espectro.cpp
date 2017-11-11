#include <iostream>

#include "../../../include/GameModel/EnviormentUnits/Espectro.h"
#include "../../../include/GameModel/Map/PathTile.h"
#include "../../../include/GameModel/ViewModels/UnitVM.h"

Espectro::Espectro(uint id, uint stepDelay, uint healthPoints, ThreadSafeQueue<GameNotification*>& notifications)
 : EnviormentUnit(id, stepDelay, healthPoints, notifications) {}

Espectro::~Espectro()
{
	std::cout << "Espectro" << GetId() << " IS DEAD!!!\n";
}

void Espectro::PrintDebug(){
	PathTile* pos = GetPosition();
	if (pos == nullptr)
		std::cout << "Espectro" << GetId() << " outside map\n";
	else
		std::cout << "Espectro" << GetId() <<  "@(" << pos->GetXPos() << ", " << pos->GetYPos() << ")\n";
}

bool Espectro::Flies(){
	return true;
}

UnitVM Espectro::GetViewModel(){
	UnitVM vm;
	vm.unitType = uEspectro;
	vm.healthPoints = _healthPoints;
	vm.xPos = _position->GetXPos();
	vm.yPos = _position->GetYPos();
	if (_position->next != nullptr){
		vm.nextXpos = _position->next->GetXPos();
		vm.nextYpos = _position->next->GetYPos();
	} else{
		vm.nextYpos = -1;
		vm.nextXpos = -1;
	}
	vm.stepDelay_ms = _GetActualStepDelay();
	vm.id = _id;
	return vm;
}