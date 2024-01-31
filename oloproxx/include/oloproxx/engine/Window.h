#pragma once
#include <oloproxx/config>


#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <wil/resource.h>

#include <glm/glm.hpp>
#include <glm/packing.hpp>
#include <glm/gtc/packing.hpp>

#include <spdlog/spdlog.h>

#include <oloproxx/protobuf/config.window.pb.h>


import utils;
import systems.console;

using namespace std::chrono_literals;

namespace window
{
	using app::config::window::EWindowType;


	class VulkanContext
	{
	public:
		HWND hwnd;

		vk::Instance instance;

		vk::PhysicalDevice physic_device;
		vk::Device         logic_device;

		vk::SurfaceKHR surface;
	};


	class Window
	{
		VulkanContext m_vk;

		GLFWwindow *     m_pWindow { nullptr };
		wil::unique_hwnd m_hwnd;

		std::unique_ptr < app::config::window::WindowConfig > m_pConfig { nullptr };

		EWindowType m_eScreenMode { EWindowType::kWindowed };

		void __update_config( );

		void __chage_window_mode( EWindowType e );

	public:
		Window( ) noexcept;

		~Window( );

		EWindowType mode( ) const;

		EWindowType fullscreen( );

		EWindowType windowed( );

		EWindowType borderless( );
	};
}
