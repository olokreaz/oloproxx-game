module;

#include <oloproxx/config>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_shared.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <vulkan/layer/vk_layer_settings.hpp>
#include <vulkan/utility/vk_struct_helper.hpp>

#include <glog/logging.h>

export module vulkan;
import types.vulkan;

type::vulkan::SwapChainSupportDetails querySwapChainSupport( const vk::PhysicalDevice &device, vk::SurfaceKHR &surface );
bool                                  isDeviceSuitable( const vk::PhysicalDevice &device, vk::SurfaceKHR &surface );

export namespace vulkan
{
	vk::Instance createInstance( const vk::InstanceCreateInfo &createInfo )
	{
		vk::Instance instance;
		if ( ( instance = vk::createInstance( createInfo ) ) == nullptr )
			LOG( FATAL );
		return instance;
	}

	vk::SurfaceKHR createSurface( const vk::Instance instance, GLFWwindow *window )
	{
		vk::SurfaceKHR                surface;
		vk::Win32SurfaceCreateInfoKHR info;

		info . hinstance = GetModuleHandle( nullptr );
		info . hwnd      = glfwGetWin32Window( window );

		surface = instance . createWin32SurfaceKHR( info );

		if ( surface == nullptr )
			LOG( FATAL ) << "Failed to create surface";

		return surface;
	}

	vk::PhysicalDevice selectPhysicalDevice( vk::Instance &instance, vk::SurfaceKHR &surface )
	{
		const std::vector < vk::PhysicalDevice > devices = instance . enumeratePhysicalDevices( );
		if ( devices . empty( ) )
			LOG( FATAL ) << "Failed to find GPUs with Vulkan support";

		for ( const auto &device : devices )
		{
			const auto &prop = device . getProperties( );
			const auto &fut  = device . getFeatures( );

			if ( isDeviceSuitable( device, surface ) )
			{
				LOG( INFO ) << "Selected GPU: " << prop . deviceName . data( );
				return device;
			}
		}

		return devices[ 0 ];
	}

	type::vulkan::QueueFamilyIndices findQueueFamilies( const vk::PhysicalDevice &device, vk::SurfaceKHR &surface )
	{
		type::vulkan::QueueFamilyIndices indices;

		const std::vector < vk::QueueFamilyProperties > queueFamilies = device . getQueueFamilyProperties( );

		int i = 0;
		for ( const auto &queueFamily : queueFamilies )
		{
			if ( queueFamily . queueFlags & vk::QueueFlagBits::eGraphics ) indices . graphicsFamily = i;

			VkBool32 presentSupport = false;
			( void ) device . getSurfaceSupportKHR( i, surface, &presentSupport );

			if ( presentSupport ) indices . presentFamily = i;

			if ( indices . isComplete( ) ) break;

			i++;
		}

		return indices;
	}

}

type::vulkan::SwapChainSupportDetails querySwapChainSupport( const vk::PhysicalDevice &device, vk::SurfaceKHR &surface )
{
	type::vulkan::SwapChainSupportDetails details {
				device . getSurfaceCapabilitiesKHR( surface ),
				device . getSurfaceFormatsKHR( surface ),
				device . getSurfacePresentModesKHR( surface )
			};

	return details;
}

bool isDeviceSuitable( const vk::PhysicalDevice &device, vk::SurfaceKHR &surface )
{
	type::vulkan::QueueFamilyIndices indices = vulkan::findQueueFamilies( device, surface );

	bool                                        swapChainAdequate = false;
	const type::vulkan::SwapChainSupportDetails swapChainSupport  = querySwapChainSupport( device, surface );
	swapChainAdequate                                             = !swapChainSupport . formats . empty( ) && !swapChainSupport . presentModes . empty( );


	return indices . isComplete( ) && swapChainAdequate;
}
