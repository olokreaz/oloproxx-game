#include "oloproxx/engine/window.hpp"

#include <GLFW/glfw3native.h>
#include <boost/nowide/convert.hpp>

#include <GLFW/glfw3native.h>
#include <glog/logging.h>
#include <glog/stl_logging.h>

#include <vulkan/vulkan_hpp_macros.hpp>

import utils;

namespace deteils
{
	VkResult CreateDebugUtilsMessengerEXT(
			VkInstance                instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator,
			VkDebugUtilsMessengerEXT *pDebugMessenger
			)
	{
		static const auto func = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );
		if ( func != nullptr ) return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator )
	{
		static const auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );
		if ( func != nullptr ) func( instance, debugMessenger, pAllocator );
	}
}

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

void Window::__CreateInstance( )
{
	glm::u32     count { };
	const char **pExtensions = glfwGetRequiredInstanceExtensions( &count );
	std::vector  requiredInstanceExt( pExtensions, pExtensions + count );
	requiredInstanceExt . push_back( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME );
	requiredInstanceExt . push_back( VK_KHR_DISPLAY_EXTENSION_NAME );
	requiredInstanceExt . push_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );

	const std::vector requiredInstanceLayers {
				#ifndef RELEASED
				"VK_LAYER_KHRONOS_validation",
				#endif
				"VK_LAYER_VALVE_steam_overlay",
				"VK_LAYER_VALVE_steam_fossilize",
			};

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

	m_vkInstance = vk::createInstance( createInfo );

}

void Window::__SelectPhysicalDevice( )
{
	const auto enumeratePhysicalDevices = m_vkInstance . enumeratePhysicalDevices( );
	if ( enumeratePhysicalDevices . empty( ) ) __error_vulkan( L"No physical devices found" );

	for ( const auto &phDevice : enumeratePhysicalDevices )
	{
		const auto properties = phDevice . getProperties( );
		if ( properties . deviceType == vk::PhysicalDeviceType::eDiscreteGpu )
		{
			m_vkPhysicalDevice = phDevice;
			return;
		}
	}
}

void Window::__CreateDevice( )
{
	vk::DeviceCreateInfo info;

	const type::vulkan::QueueFamilyIndices    indices = m_vkQueueFamilyIndices = this -> __findQueueFamilies( );
	std::vector < vk::DeviceQueueCreateInfo > queueCreateInfos;

	const std::set uniqueQueueFamilies = { *indices . graphicsFamily, *indices . presentFamily };

	constexpr float queuePriority = 1.0f;

	for ( const uint32_t queueFamily : uniqueQueueFamilies )
	{
		vk::DeviceQueueCreateInfo queueInfo;
		queueInfo . setQueueFamilyIndex( queueFamily );
		queueInfo . setQueueCount( 1 );
		queueInfo . setQueuePriorities( queuePriority );
		queueCreateInfos . push_back( queueInfo );
	}

	info . setQueueCreateInfoCount( queueCreateInfos . size( ) );
	info . setQueueCreateInfos( queueCreateInfos );

	std::vector deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, };

	info . setEnabledExtensionCount( deviceExtensions . size( ) );
	info . setPEnabledExtensionNames( deviceExtensions );

	const char *deviceLayers[ ] = { "VK_LAYER_KHRONOS_validation" };

	info . setPEnabledLayerNames( deviceLayers );
	info . setEnabledLayerCount( 1 );

	m_vkDevice = m_vkPhysicalDevice . createDevice( info );

	m_vkGraphicsQueue = m_vkDevice . getQueue( *m_vkQueueFamilyIndices . graphicsFamily, 0 );
	m_vkPresentQueue  = m_vkDevice . getQueue( *m_vkQueueFamilyIndices . presentFamily, 0 );

}

void Window::__CreateSurface( )
{
	vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo . setHinstance( this -> m_hInstance = GetModuleHandle( nullptr ) );
	surfaceCreateInfo . setHwnd( this -> m_hWindow );
	this -> m_vkSurface = m_vkInstance . createWin32SurfaceKHR( surfaceCreateInfo );

	// VkSurfaceKHR   srf;
	// const VkResult glfwResult = glfwCreateWindowSurface( *m_vkInstance, m_pWindow, nullptr, &srf );
	// if ( glfwResult != VK_SUCCESS ) throw std::runtime_error( "failed to create window surface!" );
	// m_vkSurface = srf;
}

void Window::__CreateSwapChain( )
{
	type::vulkan::SwapChainSupportDetails swapChainSupport = __querySwapChainSupport( );

	const vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat( swapChainSupport . formats );
	const vk::PresentModeKHR   presentMode   = chooseSwapPresentMode( swapChainSupport . presentModes );
	const vk::Extent2D         extent        = __chooseSwapExtent( swapChainSupport . capabilities );

	uint32_t imageCount = swapChainSupport . capabilities . minImageCount + 1;
	if ( swapChainSupport . capabilities . maxImageCount > 0 && imageCount > swapChainSupport . capabilities . maxImageCount )
		imageCount = swapChainSupport . capabilities
					      . maxImageCount;

	vk::SwapchainCreateInfoKHR createInfo;
	createInfo . surface = m_vkSurface;

	createInfo . setMinImageCount( imageCount );
	createInfo . setImageFormat( surfaceFormat . format );
	createInfo . setImageColorSpace( surfaceFormat . colorSpace );
	createInfo . setImageExtent( extent );
	createInfo . setImageArrayLayers( 1 );
	createInfo . setImageUsage( vk::ImageUsageFlagBits::eColorAttachment );

	const type::vulkan::QueueFamilyIndices &indices               = m_vkQueueFamilyIndices;
	const uint32_t                          queueFamilyIndices[ ] = { *indices . graphicsFamily, *indices . presentFamily };

	if ( indices . graphicsFamily != indices . presentFamily )
	{
		createInfo . imageSharingMode      = vk::SharingMode::eConcurrent;
		createInfo . queueFamilyIndexCount = 2;
		createInfo . pQueueFamilyIndices   = queueFamilyIndices;
	} else createInfo . imageSharingMode = vk::SharingMode::eExclusive;

	createInfo . preTransform   = swapChainSupport . capabilities . currentTransform;
	createInfo . compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo . presentMode    = presentMode;
	createInfo . clipped        = VK_TRUE;

	createInfo . oldSwapchain = m_vkOldSwapchain;

	createInfo . imageArrayLayers = 1;

	// if ( ( m_vkSwapchain = m_vkDevice . createSwapchainKHR( createInfo ) ) ) throw std::runtime_error( "failed to create swap chain!" );

	vkCreateSwapchainKHR( m_vkDevice, ( VkSwapchainCreateInfoKHR * ) &createInfo, nullptr, ( VkSwapchainKHR * ) &m_vkSwapchain );

	m_vkSwapchainImages = m_vkDevice . getSwapchainImagesKHR( m_vkSwapchain );

	m_vkSwapChainImageFormat = surfaceFormat . format;
	m_vkSwapchainExtent      = extent;
}

void Window::__UpdateSurface( ) { }

VkExtent2D Window::__chooseSwapExtent( const VkSurfaceCapabilitiesKHR &capabilities )
{
	if ( capabilities . currentExtent . width != std::numeric_limits < uint32_t >::max( ) ) return capabilities . currentExtent;

	glfwGetFramebufferSize( m_pWindow, &m_size . x, &m_size . y );

	VkExtent2D actualExtent = {
				static_cast < uint32_t >( m_size . x ),
				static_cast < uint32_t >( m_size . y )
			};

	actualExtent . width  = std::clamp( actualExtent . width, capabilities . minImageExtent . width, capabilities . maxImageExtent . width );
	actualExtent . height = std::clamp( actualExtent . height, capabilities . minImageExtent . height, capabilities . maxImageExtent . height );

	return actualExtent;
}

type::vulkan::QueueFamilyIndices Window::__findQueueFamilies( )
{
	type::vulkan::QueueFamilyIndices indices;

	const auto queueFamilies = m_vkPhysicalDevice . getQueueFamilyProperties( );

	int i = 0;
	for ( const auto &queueFamily : queueFamilies )
	{
		if ( queueFamily . queueFlags & vk::QueueFlagBits::eGraphics ) indices . graphicsFamily = i;
		VkBool32   presentSupport = false;
		vk::Result x              = m_vkPhysicalDevice . getSurfaceSupportKHR( i, m_vkSurface, &presentSupport );
		if ( presentSupport ) indices . presentFamily = i;
		if ( indices . isComplete( ) ) break;
		i++;
	}

	return indices;
}

void Window::__init_vulkan( )
{
	LOG( INFO ) << "Initializing Vulkan";

	__CreateInstance( );
	if ( !m_vkInstance ) __error_vulkan( L"Failed to create instance" );

	__SelectPhysicalDevice( );
	if ( !m_vkPhysicalDevice ) __error_vulkan( L"Failed to select physical device" );

	__CreateSurface( );
	if ( !m_vkSurface ) __error_vulkan( L"Failed to create surface" );

	__CreateDevice( );
	if ( !m_vkDevice ) __error_vulkan( L"Failed to create device" );

	__CreateSwapChain( );


	DLOG( INFO ) << "Vulkan Initialized";
}

void Window::__cleanup_vulkan( )
{
	LOG( INFO ) << "Cleaning up Vulkan";

	m_vkDevice . destroySwapchainKHR( m_vkSwapchain );
	m_vkInstance . destroySurfaceKHR( m_vkSurface );
	m_vkDevice . destroy( );
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

bool Window::__checkValidationLayerSupport( )
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

	std::vector < VkLayerProperties > availableLayers( layerCount );
	vkEnumerateInstanceLayerProperties( &layerCount, availableLayers . data( ) );

	const char *layers[ ] = { "VK_LAYER_KHRONOS_validation" };
	for ( const auto &layerName : layers )
	{
		bool layerFound = false;

		for ( const auto &layerProperties : availableLayers )
			if ( strcmp( layerName, layerProperties . layerName ) == 0 )
			{
				layerFound = true;
				break;
			}
		if ( !layerFound ) return false;
	}
	return true;
}

void Window::fullscreen( ) { glfwSetWindowMonitor( m_pWindow, glfwGetPrimaryMonitor( ), 0, 0, m_size . x, m_size . y, GLFW_DONT_CARE ); }

void Window::windowed( )
{
	glfwSetWindowAttrib( m_pWindow, GLFW_DECORATED, GLFW_FALSE );
	glfwSetWindowMonitor( m_pWindow, nullptr, 0, 0, m_size . x, m_size . y, GLFW_DONT_CARE );
}

void Window::borderless( )
{
	glfwSetWindowAttrib( m_pWindow, GLFW_DECORATED, GLFW_TRUE );
	glfwSetWindowMonitor( m_pWindow, nullptr, 0, 0, m_size . x, m_size . y, GLFW_DONT_CARE );
}

void Window::update( ) { glfwPollEvents( ); }
