#ifndef __ENVIORMENT_UNIT__
#define __ENVIORMENT_UNIT__


class Map;
class PathTile;

class EnviormentUnit{
private:
	unsigned long long  _lastTimeStamp_ms;
	unsigned long long _lastSlowBeginTimeStamp_ms;
	unsigned long long _lastFreezeTimeStamp_ms;
	bool _alive;
	uint _id;
	uint _stepDelay;
	int _healthPoints;

	PathTile* _position;
	PathTile* _lastPosition;

	Map* _map;

	bool _isSlowed;
	bool _isFrozen;
	uint _lastFreezeDuration_sec;
	uint _lastSlowDuration_sec;
	uint _activePercentSlow;

	PathTile* _GetNextTile();
	uint _GetActualStepDelay();
	bool _CanStep();
public:
	EnviormentUnit(uint id, uint stepDelay_ms, int healthPoints);
	virtual ~EnviormentUnit();
	void Step();
	unsigned int GetId();
	PathTile* GetPosition();
	void SetPosition(PathTile* pos, Map* map);
	void GetHit(uint hitPoints);
	virtual void PrintDebug() = 0;
	virtual bool Flies() = 0;
	bool IsAlive();
	uint GetHP();
	void PushBack();
	void Slow(uint slowSeconds, uint percentSlow);
	bool IsSlowed();
	void Kill();
	void Freeze(uint seconds);
};

#endif
