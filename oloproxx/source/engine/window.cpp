#include "oloproxx/engine/window.hpp"

#include <boost/nowide/convert.hpp>


#include <GLFW/glfw3native.h>
#include <glog/logging.h>
#include <glog/stl_logging.h>


void Window::__error_glfw( std::wstring_view werror )
{
	const char *pError;
	glfwGetError( &pError );

	const auto error = std::string( pError );
	glfwTerminate( );

	MessageBox( nullptr, werror . data( ), L"oloproxx", MB_ICONERROR | MB_OK );
	LOG( FATAL ) << "GLFW Error: " << error;
}

void Window::__init_glfw( )
{
	LOG( INFO ) << "Initializing GLFW";

	if ( glfwInit( ) == GLFW_FALSE ) __error_glfw( );

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

	if ( !glfwVulkanSupported( ) ) __error_glfw( L"Vulkan is not supported. Try updating your graphics card drivers." );

	m_pWindow = glfwCreateWindow( m_size . x, m_size . y, m_title . data( ), nullptr, nullptr );
	if ( m_pWindow == nullptr ) __error_glfw( );

	m_hWindow = glfwGetWin32Window( m_pWindow );
	if ( m_hWindow == nullptr ) __error_glfw( );

	DLOG( INFO ) << "GLFW Initialized";
}

void Window::__cleanup_glfw( )
{
	LOG( INFO ) << "Cleaning up GLFW";
	glfwDestroyWindow( m_pWindow );
	glfwTerminate( );
}

void Window::__error_vulkan( std::wstring werror )
{
	__cleanup_glfw( );
	MessageBox( nullptr, werror . c_str( ), L"oloproxx", MB_ICONERROR | MB_OK );

	std::string error;
	error . resize( werror . size( ) );
	boost::nowide::widen( werror . data( ), werror . size( ), error . data( ) );

	LOG( FATAL ) << "Vulkan Error: " << error;
}

void Window::__init_vulkan( )
{
	{
		glm::u32          count { };
		const char **     pExtensions = glfwGetRequiredInstanceExtensions( &count );
		const std::vector requiredInstanceExt( pExtensions, pExtensions + count );
		// requiredInstanceExt . push_back( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME );
		// requiredInstanceExt . push_back( VK_KHR_DISPLAY_EXTENSION_NAME );
		// requiredInstanceExt . push_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );

		std::vector < const char * > requiredInstanceLayers;
		#ifdef _DEBUG
		requiredInstanceLayers . push_back( "VK_LAYER_KHRONOS_validation" );
		#endif
		requiredInstanceLayers . push_back( "VK_LAYER_VALVE_steam_overlay" );
		requiredInstanceLayers . push_back( "VK_LAYER_VALVE_steam_fossilize" );


		DLOG( INFO ) << "Required Instance Extensions: " << requiredInstanceExt;

		vk::ApplicationInfo appInfo;
		appInfo . setPApplicationName( "oloproxx" );
		appInfo . setApplicationVersion( OLOPROXX_VERSION );
		appInfo . setPEngineName( "oloproxx-engine" );
		appInfo . setEngineVersion( OLOPROXX_VERSION );
		appInfo . setApiVersion( VK_API_VERSION_1_3 );

		vk::InstanceCreateInfo createInfo { };
		createInfo . setPApplicationInfo( &appInfo );

		createInfo . setEnabledExtensionCount( requiredInstanceExt . size( ) );
		createInfo . setPpEnabledExtensionNames( requiredInstanceExt . data( ) );

		createInfo . setEnabledLayerCount( requiredInstanceLayers . size( ) );
		createInfo . setPpEnabledLayerNames( requiredInstanceLayers . data( ) );

		if ( ( m_vkInstance = vk::createInstance( createInfo ) ) == VK_NULL_HANDLE ) __error_vulkan( L"Failed to create instance" );

	}
}

void Window::__cleanup_vulkan( )
{
	LOG( INFO ) << "Cleaning up Vulkan";

	/*
	 *
	 */

	m_vkInstance . destroy( );
}


void Window::initialize( )
{
	LOG( INFO ) << "Initializing window";
	__init_glfw( );
	__init_vulkan( );
}

void Window::cleanup( )
{
	LOG( INFO ) << "Cleaning up window";
	__cleanup_vulkan( );
	__cleanup_glfw( );
}

void Window::set_window_mode( app::config::engine::EWindowMode mode )
{
	m_window_mode = mode;
	switch ( mode )
	{
		case app::config::engine::EWindowMode::kBorderless : borderless( );
			break;
		case app::config::engine::EWindowMode::kFullscreen : fullscreen( );
			break;
		case app::config::engine::EWindowMode::kWindowed : windowed( );
			break;
	}
}

void Window::fullscreen( ) { throw std::logic_error( "Not implemented" ); }
void Window::windowed( ) { throw std::logic_error( "Not implemented" ); }
void Window::borderless( ) { throw std::logic_error( "Not implemented" ); }

void Window::update( ) { glfwPollEvents( ); }
