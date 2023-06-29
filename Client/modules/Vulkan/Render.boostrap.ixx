module;
#include <unordered_map>
#include<vector>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <functional>

#include<GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include<fmt/format.h>
#include<fmt/ranges.h>
#include<vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

export module Vulkan.bootstrap;
import types;

namespace Vulkan::deteils
{
	std::unordered_map< std::string, std::vector< const char * > > g_cache;

	export constexpr std::vector< const char * > getConstExtensions( )
	{
		return {
					VK_KHR_SURFACE_EXTENSION_NAME
					, VK_KHR_WIN32_SURFACE_EXTENSION_NAME
					, VK_KHR_SURFACE_EXTENSION_NAME
					, VK_KHR_WIN32_SURFACE_EXTENSION_NAME
					, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME
					, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME

				};;
	}

	export constexpr std::vector< const char * > getConstLayers( )
	{
		return {
					// "VK_LAYER_VALVE_steam_fossilize"
					// , "VK_LAYER_VALVE_steam_overlay"
					#ifdef _DEBUG
					 "VK_LAYER_KHRONOS_validation"
					#endif
				};
	}

	export [[maybe_unused]] std::vector< const char * > getAvailableExtensions( )
	{
		if ( g_cache . contains( "extensions" ) ) return g_cache[ "extensions" ];

		std::vector< const char * > vExtensions = { };
		uint32_t                    extensionCount;
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );
		std::vector< VkExtensionProperties > availableExtensions( extensionCount );
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, availableExtensions . data( ) );

		for ( const auto &extensionProperties: availableExtensions ) vExtensions . push_back( extensionProperties . extensionName );

		// uint32_t     glfwExtensionCount = 0;
		// const char **glfwExtensions;
		// glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );
		//
		// for ( uint32_t i = 0; i < glfwExtensionCount; i++ ) vExtensions . push_back( glfwExtensions[ i ] );

		g_cache[ "extensions" ] = vExtensions;

		fmt::print( "available extensions: {}\n", fmt::join( vExtensions, ",\n" ) );

		return vExtensions;
	}

	export [[maybe_unused]] std::vector< const char * > getAvailableLayers( )
	{
		if ( g_cache . contains( "layers" ) ) return g_cache[ "layers" ];

		std::vector< const char * > vLayers = { };
		uint32_t                    layerCount;
		vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
		std::vector< VkLayerProperties > availableLayers( layerCount );
		vkEnumerateInstanceLayerProperties( &layerCount, availableLayers . data( ) );
		for ( const auto &layerProperties: availableLayers ) vLayers . push_back( layerProperties . layerName );

		g_cache[ "layers" ] = vLayers;

		fmt::print( "available layers: {}\n", fmt::join( vLayers, ",\n" ) );

		return vLayers;
	}

	export VkInstance createInstance( const std::vector< const char * > &vExtensions, const std::vector< const char * > &vLayers )
	{
		VkInstance instance;

		VkApplicationInfo appInfo    = { };
		appInfo . sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo . pApplicationName   = "oloprox";
		appInfo . applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo . pEngineName        = "oloprox engine";
		appInfo . engineVersion      = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo . apiVersion         = VK_API_VERSION_1_3;
		appInfo . pNext              = nullptr;

		VkInstanceCreateInfo createInfo { };
		createInfo . sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo . pApplicationInfo        = &appInfo;
		createInfo . enabledExtensionCount   = vExtensions . size( );
		createInfo . ppEnabledExtensionNames = vExtensions . data( );
		createInfo . enabledLayerCount       = vLayers . size( );
		createInfo . ppEnabledLayerNames     = vLayers . data( );
		createInfo . pNext                   = nullptr;

		const VkResult result = vkCreateInstance( &createInfo, nullptr, &instance );

		if ( result != VK_SUCCESS )
			throw std::runtime_error( fmt::format( "Failed to create Vulkan instance: {}"
								, std::to_underlying( result )
								)
						);

		return instance;
	}

	export VkPhysicalDevice choosePhysicalDevice( VkInstance instance )
	{
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices( instance, &deviceCount, nullptr );

		if ( deviceCount == 0 ) throw std::runtime_error( "Failed to find GPUs with Vulkan support!" );

		std::vector< VkPhysicalDevice > devices( deviceCount );
		vkEnumeratePhysicalDevices( instance, &deviceCount, devices . data( ) );

		for ( const auto &device: devices ) {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties( device, &deviceProperties );

			if ( deviceProperties . deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ) {
				physicalDevice = device;
				break;
			}
		}

		if ( physicalDevice == VK_NULL_HANDLE ) physicalDevice = devices . at( 0 );

		if ( physicalDevice == VK_NULL_HANDLE ) throw std::runtime_error( "Failed to find a suitable GPU!" );

		return physicalDevice;
	}

	export std::vector< VkQueueFamilyProperties > getQueueFamilies( VkPhysicalDevice physicalDevice )
	{
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, nullptr );
		std::vector< VkQueueFamilyProperties > queueFamilies( queueFamilyCount );
		vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, queueFamilies . data( ) );

		return queueFamilies;
	}

	export uint32_t findQueueFamilies( VkPhysicalDevice physicalDevice )
	{
		std::vector< VkQueueFamilyProperties > queueFamilies = getQueueFamilies( physicalDevice );

		int i = 0;
		for ( const auto &queueFamily: queueFamilies ) {
			if ( queueFamily . queueFlags & VK_QUEUE_GRAPHICS_BIT ) return i;
			++i;
		}

		throw std::runtime_error( "Failed to find a suitable queue family!" );
	}

	// Проверка доступности слоев перед использованием
	[[maybe_unused]] void checkAvailableLayers( const std::vector< const char * > &layers )
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

		std::vector< VkLayerProperties > availableLayers( layerCount );
		vkEnumerateInstanceLayerProperties( &layerCount, availableLayers . data( ) );

		for ( const char *layerName: layers ) {
			bool layerFound = false;

			for ( const auto &layerProperties: availableLayers )
				if ( strcmp( layerName, layerProperties . layerName ) == 0 ) {
					layerFound = true;
					break;
				}

			if ( !layerFound ) throw std::runtime_error( "Not all requested layers are available!" );
		}
	}

	[[maybe_unused]] std::vector< const char * > getDeviceExtensions( VkPhysicalDevice device )
	{
		if ( g_cache . contains( "DeviceExt" ) ) return g_cache[ "DeviceExt" ];
		// Получаем количество доступных расширений
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );

		// Запрашиваем информацию о всех доступных расширениях
		fmt::print( "Available extensions:\n" );
		std::vector< VkExtensionProperties > availableExtensions( extensionCount );
		vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions . data( ) );

		for ( auto i: availableExtensions ) fmt::print( "\t{}\n", i . extensionName );

		// Создаем вектор для хранения имен расширений
		std::vector< const char * > deviceExtensions;
		for ( const auto &extension: availableExtensions ) deviceExtensions . push_back( extension . extensionName );
		g_cache[ "DeviceExt" ] = deviceExtensions;

		return deviceExtensions;
	}

	[[maybe_unused]]
	void checkDeviceExtensionsSupport( VkPhysicalDevice device, const std::vector< const char * > &deviceExtensions )
	{
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );
		std::vector< VkExtensionProperties > availableExtensions( extensionCount );
		vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions . data( ) );

		for ( const char *extensionName: deviceExtensions ) {
			bool extensionFound = false;
			for ( const auto &extensionProperties: availableExtensions ) {
				if ( strcmp( extensionName, extensionProperties . extensionName ) == 0 ) {
					extensionFound = true;
					break;
				}
			}

			if ( !extensionFound ) fmt::print( "Extension {} is not supported\n", extensionName );
		}
	}

	export constexpr std::vector< const char * > getConstDeviceExtensions( )
	{
		return {
					VK_KHR_SWAPCHAIN_EXTENSION_NAME
					, VK_KHR_8BIT_STORAGE_EXTENSION_NAME
					, VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME
					// , VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
					// , VK_KHR_RAY_QUERY_EXTENSION_NAME
				};
	}

	export VkDevice createLogicalDevice(
		VkPhysicalDevice physicalDevice
	)
	{
		auto queueFamilies = getQueueFamilies( physicalDevice );

		int graphicsFamily = findQueueFamilies( physicalDevice );
		if ( graphicsFamily == -1 ) throw std::runtime_error( "Failed to find a suitable queue family!" );

		float queuePriority = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfo { };
		queueCreateInfo . sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo . queueFamilyIndex = graphicsFamily;
		queueCreateInfo . queueCount       = 1;
		queueCreateInfo . pQueuePriorities = &queuePriority;

		auto ext = getConstDeviceExtensions( );

		checkDeviceExtensionsSupport( physicalDevice, ext );

		VkDeviceCreateInfo deviceCreateInfo { };
		deviceCreateInfo . sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo . pQueueCreateInfos       = &queueCreateInfo;
		deviceCreateInfo . queueCreateInfoCount    = 1;
		deviceCreateInfo . ppEnabledExtensionNames = ext . data( );
		deviceCreateInfo . enabledExtensionCount   = ext . size( );

		deviceCreateInfo . enabledLayerCount = 0;

		VkDevice device;
		VkResult result = vkCreateDevice( physicalDevice, &deviceCreateInfo, nullptr, &device );
		if ( result != VK_SUCCESS )
			throw fmt::system_error(
						errno
						, "Failed to create logical device: {}"
						, std::to_underlying( result )
						);

		return device;
	}

	export VkQueue getQueue( VkDevice device, uint32 graphicsFamilyIndex )
	{
		VkQueue graphicsQueue;
		// после создания устройства мы можем получить очередь
		vkGetDeviceQueue( device, graphicsFamilyIndex, 0, &graphicsQueue );
		return graphicsQueue;
	}

	export VkSurfaceKHR CreateSurface( VkInstance instance, GLFWwindow *window )
	{
		VkSurfaceKHR surface;

		VkWin32SurfaceCreateInfoKHR createInfo { };
		createInfo . sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo . hwnd      = glfwGetWin32Window( window );
		createInfo . hinstance = GetModuleHandle( nullptr );

		VkResult result = vkCreateWin32SurfaceKHR( instance, &createInfo, nullptr, &surface );

		if ( result != VK_SUCCESS ) throw fmt::system_error( errno, "Failed to create Vulkan surface: {}", std::to_underlying( result ) );

		return surface;
	}
}
