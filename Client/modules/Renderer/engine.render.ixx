module;
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/vec2.hpp>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

export module engine.render;
import :deteils;
import :types;
import system;

namespace engine::render {
	export class CEngineRender {
	public:
		CEngineRender( ) : m_config { } {
			if ( !glfwInit( ) ) {
				const char *desc;
				glfwGetError( &desc );
				spdlog::critical( "Error glfwInit(): {}", *desc );
			}

			Init( );
			vulkanInit( );
		}

		~CEngineRender( ) { cleanup( ); }

		void loop( ) // update()
		{
			glfwPollEvents( );
		}

		const types::RenderConfig* GetConfig( ) const { return &m_config; }
		bool                       ShouldClose( ) const { return glfwWindowShouldClose( m_config . pWindow ); }

	protected:
		void Init( ) {
			glfwInitHint( GLFW_CLIENT_API, GLFW_NO_API );
			glfwInitHint( GLFW_RESIZABLE, GLFW_FALSE );

			int x, y;
			glfwGetMonitorPhysicalSize( glfwGetPrimaryMonitor( ), &x, &y );

			m_config . size = { x, y };

			m_config . pWindow = glfwCreateWindow(
								m_config . size . x
								, m_config . size . y
								, "oloprox-game"
								, nullptr
								, nullptr
								);

			m_config . hWindow = glfwGetWin32Window( m_config . pWindow );

			if ( m_config . pWindow == nullptr || m_config . hWindow == 0 ) {
				spdlog::critical( "Error to create Window: {}" );
				glfwTerminate( );
				sys::Console::Show( );
				throw;
			}
		}

		void cleanup( ) { glfwTerminate( ); }

		void vulkanInit( ) { m_config . Instance = deteils::createVkInstance( ); }

		void vulkanCleanup( ) { m_config . Instance . destroy( ); }

	private:
		types::RenderConfig m_config;
	};
}
