module;

#include <ada.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

export module core.handler.url;
import core.Itype;

namespace core::handlers
{
	export class CHandlerUrl final : interface::IHandler
	{
		friend interface::HandlersRegistry;

	public:
		void handle( ) override { }

		void handle( std::string_view sUrl )
		{
			auto url = ada::parse< ada::url_aggregator >( sUrl );
			if ( !url ) throw std::invalid_argument( fmt::format( "\"sUrl\": {} must be url", sUrl ) );
			spdlog::info( "url: {}", url -> to_string( ) );
		}
	};
}
