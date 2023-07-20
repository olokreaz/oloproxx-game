module;
#include <boost/timer/timer.hpp>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

export module Application;
import engine.render;
import engine.core;
import system;
import types;

using namespace fmt::literals;
using namespace std::chrono_literals;

namespace app {
	export class CApplication {
	public:
		CApplication( glm::ivec2 &&vec = { 1980, 1080 } ) : m_render { { .size = vec, .flags = types::EWindowFlag::None } } { }

		void run( ) {
			while ( !Quit( ) ) {
				boost::timer::cpu_timer t;
				t.start(  );
				m_engine . loop( );
				m_render . loop( );
				t.stop(  );
				spdlog::info( "Time: {}", t.format( ) );
			}
		}

	protected:
		bool Quit( ) const { return m_render . ShouldClose( ); }

	private:
		engine::render::CEngineRender m_render;
		engine::CEngine               m_engine;
	};
}
