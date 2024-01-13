#include <any>
#include <iostream>
#include <stacktrace>
#include <string>
#include <vector>

#include <fmt/std.h>
#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <oloprox/config>

import systems;
import types;

class VulkanContext
{
public:
	HWND hwnd;

	vk::Instance instance;

	vk::PhysicalDevice physic_device;
	vk::Device         logic_device;

	vk::SurfaceKHR surface;
};

#include <wil/resource.h>

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
	VulkanContext m_vk;

	GLFWwindow *     m_pWindow { nullptr };
	wil::unique_hwnd m_hwnd;

	EMode m_eMode { EMode::Fullscreen };

public:
	Window( )
	{
		( void ) glfwInit( );

		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
		glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
	}

	auto mode( ) const { return EMode( ); }

	void mode( EMode e ) { m_eMode = e; }
};

int main( int argc, char **argv )
{
	vk::Instance instance;
	try
	{
		systems::console::Console::initialize( argv, argc );
		systems::console::Console::setLogLevel( spdlog::level::trace );
		systems::console::Console::setConsoleTitle( "oloprox" );
		set_default_logger( systems::utils::create_logger( "application" ) );

		Window window;
	} catch ( const std::exception &ex )
	{
		O_EXCEPTION_HANDLER( main, ex )
		return -1;
	}

	return 0;
}
