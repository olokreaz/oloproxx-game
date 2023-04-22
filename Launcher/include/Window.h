#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32

#include <commandline.h>
#include <GLFW/glfw3native.h>
#include <boost/filesystem.hpp>
#include <sago/platform_folders.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <yaml-cpp/yaml.h>

class CWindow {
public:
	CWindow( );
	~CWindow( );
	void Init( );
	void Update( );
	void Destroy( );
};
