#pragma once
#include <oloproxx/config>

#include <string_view>

#include <glm/glm.hpp>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <oloproxx/protobuf/config.engine.pb.h>
#include <vulkan/vulkan.hpp>

import types.vulkan;

class Window
{
	glm::ivec2       m_size { 1920, 1080 };
	std::string_view m_title;

	GLFWwindow *m_pWindow;
	HWND        m_hWindow;
	HINSTANCE   m_hInstance;

	app::config::engine::EWindowMode m_window_mode;

	[[ noreturn ]] void __error_glfw( std::wstring_view werror = L"Check the log for more information" );
	[[noreturn]] void   __error_vulkan( std::wstring werror = L"Check the log for more information" );

	void __CreateInstance( );
	void __SelectPhysicalDevice( );
	void __CreateDevice( );
	void __CreateSurface( );
	void __CreateSwapChain( );
	void __UpdateSurface( );

	VkExtent2D __chooseSwapExtent( const VkSurfaceCapabilitiesKHR &capabilities );

	type::vulkan::QueueFamilyIndices __findQueueFamilies( );

	vk::DebugUtilsMessengerEXT debugMessenger;

	vk::Format m_vkSwapChainImageFormat;

	vk::Instance       m_vkInstance;
	vk::PhysicalDevice m_vkPhysicalDevice;
	vk::Device         m_vkDevice;

	vk::SurfaceKHR   m_vkSurface;
	vk::SwapchainKHR m_vkSwapchain;
	vk::SwapchainKHR m_vkOldSwapchain;

	type::vulkan::QueueFamilyIndices m_vkQueueFamilyIndices;
	vk::Queue                        m_vkGraphicsQueue;
	vk::Queue                        m_vkPresentQueue;

	std::vector < vk::Image > m_vkSwapchainImages;
	vk::Extent2D              m_vkSwapchainExtent;

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

private:
	bool __checkValidationLayerSupport( );

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat( const std::vector < vk::SurfaceFormatKHR > &availableFormats )
	{
		for ( const auto &availableFormat : availableFormats )
			if ( availableFormat . format == vk::Format::eR8G8B8Srgb && availableFormat . colorSpace ==
				vk::ColorSpaceKHR::eSrgbNonlinear )
				return availableFormat;
		return availableFormats[ 0 ];
	}

	vk::PresentModeKHR chooseSwapPresentMode( const std::vector < vk::PresentModeKHR > &availablePresentModes )
	{
		for ( const auto &availablePresentMode : availablePresentModes ) if ( availablePresentMode == vk::PresentModeKHR::eMailbox ) return availablePresentMode;
		return vk::PresentModeKHR::eFifo;
	}

	type::vulkan::SwapChainSupportDetails __querySwapChainSupport( )
	{
		type::vulkan::SwapChainSupportDetails details;
		details . capabilities = m_vkPhysicalDevice . getSurfaceCapabilitiesKHR( m_vkSurface );
		details . formats      = m_vkPhysicalDevice . getSurfaceFormatsKHR( m_vkSurface );
		details . presentModes = m_vkPhysicalDevice . getSurfacePresentModesKHR( m_vkSurface );
		return details;
	}
};
