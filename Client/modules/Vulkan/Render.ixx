module;
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <ostream>
#include <sstream>
#include <stacktrace>
#include <vector>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <vulkan/vulkan.h>

export module Vulkan;
import Vulkan.bootstrap;

template< > struct fmt::formatter< std::stacktrace > {
	constexpr auto parse( format_parse_context &ctx ) { return ctx . end( ); }

	auto format( const std::stacktrace &p, format_context &ctx )
	{
		std::stringstream ss;
		ss << p;
		return format_to( ctx . out( ), "{}", ss . str( ) );
	}
};

namespace Vulkan
{
	export class VulkanRenderer {
	protected:

	public:
		// Инициализация Vulkan
		void initialize( )
		{
			if ( !glfwInit( ) ) throw fmt::system_error( errno, "glfwInit failed" );

			// GLFW необходимо инициализировать Vulkan
			glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
			glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

			// Создание окна размером 800x600
			m_pWindow = glfwCreateWindow( 800, 600, "oLoprox", nullptr, nullptr );

			if ( !m_pWindow ) {
				glfwTerminate( );
				throw fmt::system_error( errno, "glfwCreateWindow failed" );
			}

			m_hWindow = glfwGetWin32Window( m_pWindow );

			this -> initVulkan( );
		}

		void run( const bool *isQuit = nullptr )
		{
			while ( !glfwWindowShouldClose( m_pWindow ) && ( isQuit == nullptr || !( *isQuit ) ) ) glfwPollEvents( );
		}

		GLFWwindow* getGlfWwindow( ) const { return m_pWindow; }
		HWND        getHwndWindow( ) const { return m_hWindow; } // для библиотеки gainput
		VkInstance  getVkInstance( ) const { return m_vkInstance; }

		~VulkanRenderer( ) { this -> cleanup( ); }

	private:
		void initVulkan( )
		{
			// for ( auto &extension: deteils::getAvailableExtensions( ) ) m_extensions . push_back( extension );
			// for ( auto &layer: deteils::getAvailableLayers( ) ) m_layers . push_back( layer );
			try {
				m_vkInstance       = deteils::createInstance( m_extensions, m_layers );
				m_vkPhysicalDevice = deteils::choosePhysicalDevice( m_vkInstance );
				m_vkDevice         = deteils::createLogicalDevice( m_vkPhysicalDevice );
				m_vkGraphicsQueue  = deteils::getQueue( m_vkDevice, deteils::findQueueFamilies( m_vkPhysicalDevice ) );
				m_vkSurface        = deteils::CreateSurface( m_vkInstance, m_pWindow );
			} catch ( const std::exception &e ) {
				const std::stacktrace st = std::stacktrace::current( );
				fmt::print( "Vulkan initialization failed: {}\n stacktrace: {}", e . what( ), st );
			}catch ( ... ) { fmt::print( "Unknown failure during Vulkan command execution!\n" ); }
		}

		// Закрытие GLFW и очистка Vulkan
		void cleanup( )
		{
			this -> cleanupVulkan( );
			glfwDestroyWindow( m_pWindow );
			glfwTerminate( );
		}

		void cleanupVulkan( )
		{
			vkDestroySurfaceKHR( m_vkInstance, m_vkSurface, nullptr );
			vkDestroyDevice( m_vkDevice, nullptr );
			vkDestroyInstance( m_vkInstance, nullptr );
		}

		const std::vector< const char * > m_extensions = { deteils::getConstExtensions( ) };
		const std::vector< const char * > m_layers     = { deteils::getConstLayers( ) };

		HWND        m_hWindow { nullptr };
		GLFWwindow *m_pWindow { nullptr };

		VkInstance       m_vkInstance { VK_NULL_HANDLE };
		VkDevice         m_vkDevice { VK_NULL_HANDLE };
		VkPhysicalDevice m_vkPhysicalDevice { VK_NULL_HANDLE };
		VkSurfaceKHR     m_vkSurface { VK_NULL_HANDLE };
		VkQueue          m_vkGraphicsQueue { VK_NULL_HANDLE };
	};
}
