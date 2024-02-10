#pragma once
#include <oloproxx/config>

#include <string_view>

#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

#include <oloproxx/protobuf/config.engine.pb.h>
#include <vulkan/vulkan.hpp>


class Window
{
	glm::ivec2       m_size { 1920, 1080 };
	std::string_view m_title;

	GLFWwindow *m_pWindow;
	HWND        m_hWindow;

	app::config::engine::EWindowMode m_window_mode;

	[[ noreturn ]] void __error_glfw( std::wstring_view werror = L"Check the log for more information" );

	[[noreturn]] void __error_vulkan( std::wstring werror = L"Check the log for more information" );

	vk::Instance       m_vkInstance;
	vk::PhysicalDevice m_vkPhysicalDevice;
	vk::Device         m_vkDevice;

	vk::SurfaceKHR m_vkSurface;


	void __init_glfw( );

	void __cleanup_glfw( );

	void __init_vulkan( );

	void __cleanup_vulkan( );

public:
	Window( glm::ivec2 size, std::string_view title ) noexcept :
		m_size( size ),
		m_title( title ),
		m_pWindow { nullptr },
		m_hWindow { nullptr } { }


	~Window( ) { cleanup( ); }

	void update( );

	void initialize( );

	void cleanup( );


	void fullscreen( );

	void windowed( );

	void borderless( );


	void set_window_mode( app::config::engine::EWindowMode mode );

	app::config::engine::EWindowMode window_mode( ) const { return m_window_mode; }

	GLFWwindow *get_window( ) const { return m_pWindow; }
	HWND        get_hwnd( ) const { return m_hWindow; }

	auto ShouldClose( ) const { return glfwWindowShouldClose( m_pWindow ); }
};
