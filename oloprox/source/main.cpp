#include <oloproxx/config>

#include <stacktrace>
#include <string>
#include <vector>

#include <fmt/std.h>
#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <wil/resource.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>

import systems;
import types;
import utils;

import engine.config;
import engine.types;

class VulkanContext
{
public:
	HWND hwnd;

	vk::Instance instance;

	vk::PhysicalDevice physic_device;
	vk::Device         logic_device;

	vk::SurfaceKHR surface;
};

class WindowConfig : public engine::config::IConfig
{
public:
	std::vector< engine::type::CMonitor > monitors;

	PROPERTY( position, glm::i32vec2 )
	PROPERTY( size, glm::i32vec2 )

	constexpr std::string_view title = "oloproxx";
};

REGISTER_CONFIG( WindowConfig )

class Window
{
public:
	enum class EMode
	{
		Windowed , // оконный
		Fullscreen , // полноэкранный (default)
		Borderless , // оконный без рамки
	};

private:
	VulkanContext                   m_vk;
	std::shared_ptr< WindowConfig > config_ = engine::config::ConfigRegistry::instance( ) -> at< WindowConfig >( );

	GLFWwindow *     m_pWindow { nullptr };
	wil::unique_hwnd m_hwnd;

	EMode m_eScreenMode { EMode::Fullscreen };

public:
	Window( ) noexcept
	{
		( void ) glfwInit( );

		std::span< GLFWmonitor * > monitors;
		{
			int           count   = 0;
			GLFWmonitor **monitor = glfwGetMonitors( nullptr );
			monitors              = std::span { monitor, ( uint64 ) count };
		}

		for ( const auto &x : monitors ) config_ -> monitors . push_back( x );

		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
		glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
		glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );
	}

	EMode mode( ) const { return m_eScreenMode; }

	EMode fullscreen( )
	{
		glfwSetWindowAttrib( m_pWindow, GLFW_DECORATED, GLFW_TRUE );
		glfwSetWindowMonitor( m_pWindow, nullptr, 0, 0, 800, 600, 0 );
	}
};

int main( int argc, char **argv )
{
	try
	{
		systems::console::Console::initialize( argv, argc );
		systems::console::Console::setLogLevel( spdlog::level::trace );
		systems::console::Console::setConsoleTitle( "oloproxx" );
		set_default_logger( utils::create_logger( "application" ) );

		Window window;
	} catch ( const std::exception &ex )
	{
		O_EXCEPTION_HANDLER( main, ex )
		return -1;
	}

	return 0;
}
