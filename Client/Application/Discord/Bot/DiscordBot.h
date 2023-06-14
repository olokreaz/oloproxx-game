#pragma once
#include <map>
#include <string>
#include <spdlog/spdlog.h>

#include <boost/filesystem.hpp>
#include <steam/steamtypes.h>
#include <yaml-cpp/yaml.h>

/*
 * Singleton class for Discord bot
 */
class CDiscordBot { // TODO: make discord bot class
	CDiscordBot( )
	{
		YAML::Node config = YAML::LoadFile(
						(
							boost::
							filesystem
							::current_path( ) /
							"config.yaml"
						) . string( )
						);
	}

	CDiscordBot( const std::string &token, const std::string &prefix ) : m_Token { token }
									, m_Prefix { prefix } { }

public:
	void init( ) { }
	~CDiscordBot( ) = default;

private:
	const std::string                            m_Token; // get of Yaml config 
	std::string                                  m_Prefix; // specify prefix for commands
	std::shared_ptr< spdlog::logger >            m_logger    = spdlog::get( "Discord Bot" );
	inline static std::shared_ptr< CDiscordBot > m_pInstance = { nullptr };
};
