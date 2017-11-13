#ifndef ___PROTOCOLO___
#define ___PROTOCOLO___

#define CREATE_LOBBY 11
#define JOIN_LOBBY 13
#define LEAVE_LOBBY 16
#define PLAYER_LEFT_LOBBY 14
#define PLAYER_JOINED_LOBBY 15
#define PLAYER_JOIN 17
#define PLAYER_LEAVE 18
#define PLAYER_IS_READY 19

#define LOG_IN	20
#define LOG_IN_FAILED 21
#define LOG_IN_SUCCESS 22

#define NEW_TOWER 30
#define WATER_TOWER 31
#define FIRE_TOWER 32
#define EARTH_TOWER 33
#define AIR_TOWER 34

#define PLAYER_PICKED_SPELL 40
#define PLAYER_UNPICKED_SPELL 41
#define PICK_SPELL 42
#define UNPICK_SPELL 43

#ifndef __SPELLS_ENUM__
#define __SPELLS_ENUM__
enum SPELL_TYPE {
    SPELL_TYPE_FIRE = 50,
    SPELL_TYPE_AIR = 51,
    SPELL_TYPE_GROUND = 52,
    SPELL_TYPE_WATER = 53
} ;
#endif


#ifndef __UNITS_ENUM__
#define __UNITS_ENUM__
enum UNIT_TYPE {
    UNIT_TYPE_Abmonible = 54,
    UNIT_TYPE_DemonioVerde = 55,
    UNIT_TYPE_Espectro = 56,
    UNIT_TYPE_HalconSangriento = 57,
    UNIT_TYPE_HombreCabra = 58,
    UNIT_TYPE_NoMuerto = 59
} ;
#endif



#define GAME_STARTED 100
#define GAME_OPCODE 101
#define TOWER_PLACED 102
#define UNIT_POSITION_UPDATE 103
#define UNIT_CREATED 104


#endif