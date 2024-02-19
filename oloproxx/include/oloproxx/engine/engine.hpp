#pragma once
#include <oloproxx/config>

#include <oloproxx/protobuf/config.engine.pb.h>

#include <glog/logging.h>
#include <glog/stl_logging.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>

#include <string>
#include <vector>

#include "window.hpp"

using namespace std::chrono_literals;


class Engine
{

	app::config::engine::EngineConfig m_config;

	// window
	GLFWwindow *m_pWindow;
	HWND        m_hWindow;

	// monitor
	GLFWmonitor *m_pMonitor;

	glm::ivec2 m_windowSize { 1920, 1080 };
	glm::ivec2 m_windowResolution { 1920, 1080 };

	Window m_window { m_windowSize, "oloproxx" };

	void __update( glm::f32 dt = 0.0f )
	{
		m_window . update( );
		std::this_thread::sleep_for( 500ms );
	}

	bool Quited( ) { return glfwWindowShouldClose( m_pWindow ) /* && btnQuit */; }

public:
	Engine( )
	{
		m_window . initialize( );
		m_pWindow = m_window . get_window( );
		m_hWindow = m_window . get_hwnd( );
	}

	void run( )
	{
		while ( !Quited( ) )
		{
			std::this_thread::sleep_for( 50ms );
			m_window . update( );
			// DLOG_EVERY_N( INFO, 100 ) << "Rendering";
		}
	}

	~Engine( ) { }
};
