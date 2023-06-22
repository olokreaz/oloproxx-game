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



#include "types/Command.hpp"

#define WHILE while (true)
#define WHILE_QUIT while ( !*m_pbQuit )
enum class ESocketType {
	NONE
	, CLIENT
	, SERVER
};

class CApplication {
	bool *                            m_pbQuit      = { nullptr };
	std::shared_ptr< spdlog::logger > m_logger      = { spdlog::get( "Global" ) };
	std::shared_ptr< spdlog::logger > m_steamlogger = { spdlog::get( "Steam" ) };

	ESocketType m_eSocketType = { ESocketType::NONE };

	std::thread *             m_pThreadLocalInput = { nullptr };
	std::mutex                m_mutexLocalInput;
	std::queue< std::string > QueueLocalInput;

protected:
	void chooseSocketType( );
	void SteamInit( );

	void CommandHandlerInit( ) { }

	void SocketInit( )
	{
		SteamNetworkingUtils( ) -> SetDebugOutputFunction(
								k_ESteamNetworkingSocketsDebugOutputType_Msg
								, details::Networking::NetDebugOutput
								);
	}

	void shoutdown( );

	void runLocalInputCallback( );

public:
	void init( ESocketType eSocketType = ESocketType::NONE );

	result< void > run( );

	operator bool( ) const;
	explicit CApplication( bool *b_quit );

public:
	struct socket_t {
		std::shared_ptr< CServer > m_server;
		std::shared_ptr< CClient > m_client;
	};

private:
	socket_t                       m_socket;
};
