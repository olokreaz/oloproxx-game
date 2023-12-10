module;
#include <filesystem>

#include <oloprox/config>

#include <windows.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.hpp>

#include <stb_image.h>

export module systems.windows;
import systems.files;

namespace systems::windows
{
	class CWindow
	{
		GLFWwindow *m_pWindow;
		HWND        m_hwnd;

	public:
		CWindow( int width, int height, const char *title )
		{
			glfwInit( );
			glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
			m_pWindow = glfwCreateWindow( width, height, title, nullptr, nullptr );
		}

		void setResolution( int width, int height ) { glfwSetWindowSize( m_pWindow, width, height ); }

		void setAspectRatio( int numerator, int denominator ) { glfwSetWindowAspectRatio( m_pWindow, numerator, denominator ); }

		void minimizeWindow( ) { glfwIconifyWindow( m_pWindow ); }

		void closeWindow( ) { glfwSetWindowShouldClose( m_pWindow, GLFW_TRUE ); }

		void setIcon( std::string iconPath )
		{
			GLFWimage images[ 1 ];
			images[ 0 ] = loadIcon( iconPath );
			glfwSetWindowIcon( m_pWindow, 1, images );
		}

		GLFWwindow* getWindow( ) const { return m_pWindow; }

		void* getNativeDescriptor( ) const
		{
			#if defined(GLFW_EXPOSE_NATIVE_WIN32)
			return glfwGetWin32Window( m_pWindow );
			#elif defined(GLFW_EXPOSE_NATIVE_X11)
            return glfwGetX11Window(m_pWindow);
			#elif defined(GLFW_EXPOSE_NATIVE_COCOA)
            return glfwGetCocoaWindow(m_pWindow);
			#else
            return nullptr;
			#endif
		}

		bool shouldClose( ) const { return glfwWindowShouldClose( m_pWindow ); }

		~CWindow( )
		{
			glfwDestroyWindow( m_pWindow );
			glfwTerminate( );
		}

	private:
		static GLFWimage loadIcon( const std::wstring PathResource )
		{
			GLFWimage image;

			files::CResource data( PathResource );

			image . pixels = stbi_load_from_memory(
								&data . data( )[ 0 ],
								data . data( ) . size( ),
								&image . width,
								&image . height,
								nullptr,
								STBI_rgb_alpha
								);
			return image;
		}

		static GLFWimage loadIcon( const std::filesystem::path path )
		{
			GLFWimage image;

			image . pixels = stbi_load(
						path . string( ) . c_str( ),
						&image . width,
						&image . height,
						nullptr,
						STBI_rgb_alpha
						);
			return image;
		}
	};
}
