#include "Window.h"


void window::Window::__update_config( )
{

	std::span < GLFWmonitor * > monitors;
	{
		int           count   = 0;
		GLFWmonitor **monitor = glfwGetMonitors( &count );
		monitors              = std::span { monitor, static_cast < std::size_t >( count ) };
	}

	int count = 0;


}

void window::Window::__chage_window_mode( EWindowType e )
{
	switch ( e )
	{
		case EWindowType::kFullscreen :
		{
			m_pConfig -> set_type( EWindowType::kFullscreen );
			glfwSetWindowAttrib( m_pWindow, GLFW_DECORATED, GLFW_FALSE );
			glfwSetWindowAttrib( m_pWindow, GLFW_RESIZABLE, GLFW_FALSE );
			break;
		}
		case EWindowType::kWindowed :
		{
			m_pConfig -> set_type( EWindowType::kWindowed );
			glfwSetWindowAttrib( m_pWindow, GLFW_DECORATED, GLFW_TRUE );
			glfwSetWindowAttrib( m_pWindow, GLFW_RESIZABLE, GLFW_TRUE );
			break;
		}
		case EWindowType::kBorderless :
		{
			m_pConfig -> set_type( EWindowType::kBorderless );
			glfwSetWindowAttrib( m_pWindow, GLFW_DECORATED, GLFW_FALSE );
			glfwSetWindowAttrib( m_pWindow, GLFW_RESIZABLE, GLFW_TRUE );
			break;
		}
		default : ;
	}
	spdlog::info( "window mode changed to {}", fmt::underlying( e ) );
}

window::Window::Window( ) noexcept
{
	( void ) glfwInit( );

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

	__update_config( );

}

window::Window::~Window( )
{
	glfwDestroyWindow( m_pWindow );
	glfwTerminate( );
}

window::EWindowType window::Window::mode( ) const { return m_eScreenMode; }

window::EWindowType window::Window::fullscreen( )
{
	m_pConfig -> set_type( EWindowType::kFullscreen );
	glfwSetWindowAttrib( m_pWindow, GLFW_DECORATED, GLFW_TRUE );
	glfwSetWindowMonitor( m_pWindow, nullptr, 0, 0, 800, 600, -1 );
	return m_eScreenMode = EWindowType::kFullscreen;
}

window::EWindowType window::Window::windowed( )
{
	m_pConfig -> set_type( EWindowType::kWindowed );
	glfwSetWindowAttrib( m_pWindow, GLFW_DECORATED, GLFW_TRUE );
	glfwSetWindowMonitor( m_pWindow, nullptr, 0, 0, 800, 600, -1 );
	return m_eScreenMode = EWindowType::kWindowed;
}

window::EWindowType window::Window::borderless( )
{
	m_pConfig -> set_type( EWindowType::kBorderless );
	glfwSetWindowAttrib( m_pWindow, GLFW_DECORATED, GLFW_FALSE );
	glfwSetWindowMonitor( m_pWindow, nullptr, 0, 0, 800, 600, -1 );
	return m_eScreenMode = EWindowType::kBorderless;
}
