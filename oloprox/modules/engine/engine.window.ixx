module;
#include <oloproxx/config>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <wil/resource.h>

#include <libconfig.hh>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

export module engine.window;
import engine.config;
import engine.types;


namespace engine::window
{
	class VulkanContext
	{
	public:
		HWND hwnd;

		vk::Instance instance;

		vk::PhysicalDevice physic_device;
		vk::Device         logic_device;

		vk::SurfaceKHR surface;
	};

	export class Window
	{
	public:
		enum class EMode
		{
			Windowed , // оконный
			Fullscreen , // полноэкранный (default)
			Borderless , // оконный без рамки
		};

	private:
		VulkanContext       m_vk;
		libconfig::Setting &config_ = config::ConfigRegistry::instance( )[ "Window" ];

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
				GLFWmonitor **monitor = glfwGetMonitors( &count );
				monitors              = std::span { monitor, static_cast< uint64 >( count ) };
			}

				auto & config_monitors = config_[ "monitors" ] = config_.TypeArray;
			for ( const auto &x : monitors )
			{
				auto &  monitor = config_monitors.add( libconfig::Setting::TypeGroup );
				type::CMonitor m( x );
				monitor["name"] = m.Name.data();

			}


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
}

