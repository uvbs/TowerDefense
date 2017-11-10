#include <iostream>
#include <fstream>

#include "../../include/GameModel/GameConfiguration.h"
#include <yaml-cpp/yaml.h>

GameConfiguration::GameConfiguration(std::string& cfgpath) : Cfg(std::move(YAML::LoadFile(cfgpath)))
{
		//std::cout << "ctor: " << Cfg["towers"]["ground"]["range"] << "\n" << std::flush;

}

GameConfiguration::~GameConfiguration()
{

}