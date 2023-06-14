#pragma once
#include <fstream>
#include <memory>
#include <string>
#include <sleepy_discord/sleepy_discord.h>
#include <spdlog/spdlog.h>

#include <boost/filesystem.hpp>
#include <steam/steamtypes.h>
#include <yaml-cpp/yaml.h>

/*
 * Singleton class for Discord bot
 */

namespace SD = SleepyDiscord;

struct Token_t {
	const std::string Token;
};

class CDiscordBot : public SD::DiscordClient { // TODO: make discord bot class
	static void ConfigInit( )
	{
		YAML::Node config = YAML::LoadFile(
						(
							boost::
							filesystem
							::current_path( ) /
							"config.yaml"
						) . string( )
						)[ "discord" ];
	}

	CDiscordBot( const std::string &token, char numOfThreads ) : WebsocketppDiscordClient { token, numOfThreads } {}

public:
	static std::shared_ptr< CDiscordBot > instance( )
	{
		if ( !m_pInstance ) {
			m_pInstance = std::make_shared< CDiscordBot >( m_Token . Token, SD::DEFAULT_THREADS );
			m_pInstance -> ConfigInit( );
		}
		return m_pInstance;
	}

	void set_Prefix( std::string_view prefix )
	{
		m_Prefix            = prefix;
		m_config[ "token" ] = m_Prefix;
	}

	~CDiscordBot( ) override
	{
		std::ofstream fs( (
					boost::
					filesystem
					::current_path( ) /
					"config.yaml"
				) . string( )
				);
		if ( !fs . is_open( ) ) throw std::runtime_error( "Can Create File" );
		fs << m_config;
		fs . close( );
	}

private:
	static YAML::Node                               m_config;
	static Token_t                                  m_Token; // get of Yaml config 
	static std::string                              m_Prefix; // specify prefix for commands
	inline static std::shared_ptr< spdlog::logger > m_logger    = { spdlog::get( "Discord Bot" ) };
	inline static std::shared_ptr< CDiscordBot >    m_pInstance = { nullptr };
};
