#pragma once
#include <memory>
#include <thread>
#include <concurrencpp/concurrencpp.h>
#include <spdlog/spdlog.h>
#include <steam/steam_api.h>

#include <queue>

#include "Socket/Client.hpp"
#include "Socket/Server.hpp"

#include <Application/Socket/details.hpp>

#define WHILE while (true)
#define WHILE_QUIT while ( !*m_pbQuit )
enum class ESocketType {
	CLIENT
	, SERVER
	, ALL
	, NONE
};

class CApplication {
	bool *                            m_pbQuit      = { nullptr };
	std::shared_ptr< spdlog::logger > m_logger      = { spdlog::get( "Global" ) };
	std::shared_ptr< spdlog::logger > m_steamlogger = { spdlog::get( "Steam" ) };

	ESocketType m_eSocketType = { ESocketType::NONE };

	std::thread *             m_pThreadLocalInput = { nullptr };
	std::mutex                m_mutexLocalInput;
	std::queue< std::string > QueueLocalInput;

public:
	void SteamInit( );

	void init(
		ESocketType eSocketType = ESocketType::NONE
	);

	void initSocket( )
	{
		SteamNetworkingUtils( ) -> SetDebugOutputFunction(
								k_ESteamNetworkingSocketsDebugOutputType_Msg
								, details::Networking::NetDebugOutput
								);
	}

	result< void > run( );
	void           shoutdown( );

	auto getAllThread( )
	{
		return std::make_tuple( m_pThreadLocalInput, m_socket . m_client -> getThread( ), m_socket . m_server -> getThread( ) );
	}

	operator bool( ) const;
	explicit CApplication( bool *b_quit );

public:
	struct socket_t {
		std::shared_ptr< CServer > m_server;
		std::shared_ptr< CClient > m_client;
	};

private:
	socket_t m_socket;
};
