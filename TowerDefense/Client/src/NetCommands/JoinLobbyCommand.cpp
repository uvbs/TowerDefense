#include "NetCommands/JoinLobbyCommand.h"
#include "../../Protocolo.h"

JoinLobbyCommand::JoinLobbyCommand(uint lobbyGUID) : _lobbyGUID(lobbyGUID)
{
	
}

JoinLobbyCommand::~JoinLobbyCommand(){
}



void JoinLobbyCommand::SendCommand(SocketWrapper& sock){
	uint8_t opcode = LOBBY_OPCODE; 
	sock.Send((char*) &opcode, 1);
	uint8_t instruction = JOIN_LOBBY; 
	sock.Send((char*)&instruction, 1);
	sock.Send((char*)&_lobbyGUID, 4);
}