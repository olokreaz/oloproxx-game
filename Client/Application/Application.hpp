#pragma once
#include <memory>
#include <thread>
#include <concurrencpp/concurrencpp.h>
#include <spdlog/spdlog.h>
#include <steam/steam_api.h>

#include "Socket/Client.hpp"
#include "Socket/Server.hpp"

#define WHILE while (true)
#define WHILE_QUIT while ( !m_bQuit )

enum class ESocketType {
	CLIENT
	, SERVER
	, ALL
	, NONE
};

class CApplication {
	bool *                            m_bQuit       = { nullptr };
	std::shared_ptr< spdlog::logger > m_logger      = { spdlog::get( "Global" ) };
	std::shared_ptr< spdlog::logger > m_steamlogger = { spdlog::get( "Steam" ) };

	ESocketType m_eSocketType = { ESocketType::NONE };

public:
	void SteamInit( );

	void init(
		ESocketType eSocketType = ESocketType::NONE
	);

	concurrencpp::result< void > run( );

	void shoutdown( );

	operator bool( ) const;

	explicit CApplication( bool *b_quit );

	struct socket_t {
		std::shared_ptr< CServer > m_server;
		std::shared_ptr< CClient > m_client;
	};

private:
	socket_t m_socket;
};
