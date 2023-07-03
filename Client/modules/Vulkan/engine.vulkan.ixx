#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

export module engine.vulkan;
import types;
import vulkan.bootstrap;
import engine.vulkan.deteils;

constexpr uint32_t WIDTH  = 800;
constexpr uint32_t HEIGHT = 600;

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

export class Renderer {
public:
	void run( )
	{
		initWindow( );
		initVulkan( );
		mainLoop( );
		cleanup( );
	}

private:
	GLFWwindow *window;

	VkInstance               instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR             surface;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice         m_vkDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR               swapChain;
	std::vector< VkImage >       swapChainImages;
	VkFormat                     swapChainImageFormat;
	VkExtent2D                   swapChainExtent;
	std::vector< VkImageView >   swapChainImageViews;
	std::vector< VkFramebuffer > swapChainFramebuffers;

	VkRenderPass     renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline       graphicsPipeline;

	VkCommandPool   commandPool;
	VkCommandBuffer commandBuffer;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence     inFlightFence;

	void initWindow( )
	{
		glfwInit( );

		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
		glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

		window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr );
	}

	void initVulkan( )
	{
		createInstance( );
		createSurface( );
		pickPhysicalDevice( );
		createLogicalDevice( );
		createSwapChain( );
		createImageViews( );
		createRenderPass( );
		createGraphicsPipeline( );
		createFramebuffers( );
		createCommandPool( );
		createCommandBuffer( );
		createSyncObjects( );
	}

	void mainLoop( )
	{
		while ( !glfwWindowShouldClose( window ) ) {
			glfwPollEvents( );
			drawFrame( );
		}

		vkDeviceWaitIdle( m_vkDevice );
	}

	void cleanup( )
	{
		vkDestroySemaphore( m_vkDevice, renderFinishedSemaphore, nullptr );
		vkDestroySemaphore( m_vkDevice, imageAvailableSemaphore, nullptr );
		vkDestroyFence( m_vkDevice, inFlightFence, nullptr );

		vkDestroyCommandPool( m_vkDevice, commandPool, nullptr );

		for ( auto framebuffer: swapChainFramebuffers ) vkDestroyFramebuffer( m_vkDevice, framebuffer, nullptr );

		vkDestroyPipeline( m_vkDevice, graphicsPipeline, nullptr );
		vkDestroyPipelineLayout( m_vkDevice, pipelineLayout, nullptr );
		vkDestroyRenderPass( m_vkDevice, renderPass, nullptr );

		for ( auto imageView: swapChainImageViews ) vkDestroyImageView( m_vkDevice, imageView, nullptr );

		vkDestroySwapchainKHR( m_vkDevice, swapChain, nullptr );
		vkDestroyDevice( m_vkDevice, nullptr );

		vkDestroySurfaceKHR( instance, surface, nullptr );
		vkDestroyInstance( instance, nullptr );

		glfwDestroyWindow( window );

		glfwTerminate( );
	}

	void createInstance( )
	{
		VkApplicationInfo appInfo { };
		appInfo . sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo . pApplicationName   = "oloprox";
		appInfo . applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo . pEngineName        = "oloprox Engine";
		appInfo . engineVersion      = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo . apiVersion         = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo { };
		createInfo . sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo . pApplicationInfo = &appInfo;

		auto extensions                      = Vulkan::bootstrap::getConstExtensions( );
		createInfo . enabledExtensionCount   = extensions . size( );
		createInfo . ppEnabledExtensionNames = extensions . data( );

		auto validationLayers            = Vulkan::bootstrap::getConstLayers( );
		createInfo . enabledLayerCount   = validationLayers . size( );
		createInfo . ppEnabledLayerNames = validationLayers . data( );

		createInfo . pNext = nullptr;

		if ( vkCreateInstance( &createInfo, nullptr, &instance ) != VK_SUCCESS ) throw std::runtime_error( "failed to create instance!" );
	}

	void createSurface( )
	{
		if ( glfwCreateWindowSurface( instance, window, nullptr, &surface ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to create window surface!" );
	}

	void pickPhysicalDevice( )
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices( instance, &deviceCount, nullptr );

		if ( deviceCount == 0 ) throw std::runtime_error( "failed to find GPUs with Vulkan support!" );

		std::vector< VkPhysicalDevice > devices( deviceCount );
		vkEnumeratePhysicalDevices( instance, &deviceCount, devices . data( ) );

		for ( const auto &device: devices )
			if ( isDeviceSuitable( device ) ) {
				physicalDevice = device;
				break;
			}

		if ( physicalDevice == VK_NULL_HANDLE ) throw std::runtime_error( "failed to find a suitable GPU!" );
	}

	void createLogicalDevice( )
	{
		QueueFamilyIndices indices = findQueueFamilies( physicalDevice );

		std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;
		std::set< uint32_t > uniqueQueueFamilies = { indices . graphicsFamily . value( ), indices . presentFamily . value( ) };

		float queuePriority = 1.0f;
		for ( uint32_t queueFamily: uniqueQueueFamilies ) {
			VkDeviceQueueCreateInfo queueCreateInfo { };
			queueCreateInfo . sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo . queueFamilyIndex = queueFamily;
			queueCreateInfo . queueCount       = 1;
			queueCreateInfo . pQueuePriorities = &queuePriority;
			queueCreateInfos . push_back( queueCreateInfo );
		}

		VkPhysicalDeviceFeatures deviceFeatures { };

		VkDeviceCreateInfo createInfo { };
		createInfo . sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo . queueCreateInfoCount = static_cast< uint32_t >( queueCreateInfos . size( ) );
		createInfo . pQueueCreateInfos    = queueCreateInfos . data( );

		createInfo . pEnabledFeatures = &deviceFeatures;

		auto deviceExtensions = Vulkan::bootstrap::getConstDeviceExtensions( );

		createInfo . enabledExtensionCount   = deviceExtensions . size( );
		createInfo . ppEnabledExtensionNames = deviceExtensions . data( );

		createInfo . enabledLayerCount = 0;

		if ( vkCreateDevice( physicalDevice, &createInfo, nullptr, &m_vkDevice ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to create logical device!" );

		vkGetDeviceQueue( m_vkDevice, indices . graphicsFamily . value( ), 0, &graphicsQueue );
		vkGetDeviceQueue( m_vkDevice, indices . presentFamily . value( ), 0, &presentQueue );
	}

	void createSwapChain( )
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport( physicalDevice );

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat( swapChainSupport . formats );
		VkPresentModeKHR   presentMode   = chooseSwapPresentMode( swapChainSupport . presentModes );
		VkExtent2D         extent        = chooseSwapExtent( swapChainSupport . capabilities );

		uint32_t imageCount = swapChainSupport . capabilities . minImageCount + 1;
		if ( swapChainSupport . capabilities . maxImageCount > 0 && imageCount > swapChainSupport . capabilities .
													maxImageCount )
			imageCount = swapChainSupport . capabilities . maxImageCount;

		VkSwapchainCreateInfoKHR createInfo { };
		createInfo . sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo . surface = surface;

		createInfo . minImageCount    = imageCount;
		createInfo . imageFormat      = surfaceFormat . format;
		createInfo . imageColorSpace  = surfaceFormat . colorSpace;
		createInfo . imageExtent      = extent;
		createInfo . imageArrayLayers = 1;
		createInfo . imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices               = findQueueFamilies( physicalDevice );
		uint32_t           queueFamilyIndices[ ] = { indices . graphicsFamily . value( ), indices . presentFamily . value( ) };

		if ( indices . graphicsFamily != indices . presentFamily ) {
			createInfo . imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
			createInfo . queueFamilyIndexCount = 2;
			createInfo . pQueueFamilyIndices   = queueFamilyIndices;
		} else createInfo . imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

		createInfo . preTransform   = swapChainSupport . capabilities . currentTransform;
		createInfo . compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo . presentMode    = presentMode;
		createInfo . clipped        = VK_TRUE;

		createInfo . oldSwapchain = VK_NULL_HANDLE;

		if ( vkCreateSwapchainKHR( m_vkDevice, &createInfo, nullptr, &swapChain ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to create swap chain!" );

		vkGetSwapchainImagesKHR( m_vkDevice, swapChain, &imageCount, nullptr );
		swapChainImages . resize( imageCount );
		vkGetSwapchainImagesKHR( m_vkDevice, swapChain, &imageCount, swapChainImages . data( ) );

		swapChainImageFormat = surfaceFormat . format;
		swapChainExtent      = extent;
	}

	void createImageViews( )
	{
		swapChainImageViews . resize( swapChainImages . size( ) );

		for ( size_t i = 0; i < swapChainImages . size( ); i++ ) {
			VkImageViewCreateInfo createInfo { };
			createInfo . sType                             = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo . image                             = swapChainImages[ i ];
			createInfo . viewType                          = VK_IMAGE_VIEW_TYPE_2D;
			createInfo . format                            = swapChainImageFormat;
			createInfo . components . r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo . components . g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo . components . b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo . components . a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo . subresourceRange . aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo . subresourceRange . baseMipLevel   = 0;
			createInfo . subresourceRange . levelCount     = 1;
			createInfo . subresourceRange . baseArrayLayer = 0;
			createInfo . subresourceRange . layerCount     = 1;

			if ( vkCreateImageView( m_vkDevice, &createInfo, nullptr, &swapChainImageViews[ i ] ) != VK_SUCCESS )
				throw
						std::runtime_error( "failed to create image views!" );
		}
	}

	void createRenderPass( )
	{
		VkAttachmentDescription colorAttachment { };
		colorAttachment . format         = swapChainImageFormat;
		colorAttachment . samples        = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment . loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment . storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment . stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment . stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment . initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment . finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef { };
		colorAttachmentRef . attachment = 0;
		colorAttachmentRef . layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass { };
		subpass . pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass . colorAttachmentCount = 1;
		subpass . pColorAttachments    = &colorAttachmentRef;

		VkSubpassDependency dependency { };
		dependency . srcSubpass    = VK_SUBPASS_EXTERNAL;
		dependency . dstSubpass    = 0;
		dependency . srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency . srcAccessMask = 0;
		dependency . dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency . dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo { };
		renderPassInfo . sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo . attachmentCount = 1;
		renderPassInfo . pAttachments    = &colorAttachment;
		renderPassInfo . subpassCount    = 1;
		renderPassInfo . pSubpasses      = &subpass;
		renderPassInfo . dependencyCount = 1;
		renderPassInfo . pDependencies   = &dependency;

		if ( vkCreateRenderPass( m_vkDevice, &renderPassInfo, nullptr, &renderPass ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to create render pass!" );
	}

	void createGraphicsPipeline( )
	{
		std::vector< uint8 > vertShaderCode = utils::readFile( "shader.vert.spv" );
		std::vector< uint8 > fragShaderCode = utils::readFile( "shader.frag.spv" );

		VkShaderModule vertShaderModule = utils::createShaderModule( vertShaderCode, m_vkDevice);
		VkShaderModule fragShaderModule = utils::createShaderModule( fragShaderCode, m_vkDevice);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo { };
		vertShaderStageInfo . sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo . stage  = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo . module = vertShaderModule;
		vertShaderStageInfo . pName  = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo { };
		fragShaderStageInfo . sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo . stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo . module = fragShaderModule;
		fragShaderStageInfo . pName  = "main";

		VkPipelineShaderStageCreateInfo shaderStages[ ] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo { };
		vertexInputInfo . sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo . vertexBindingDescriptionCount   = 0;
		vertexInputInfo . vertexAttributeDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly { };
		inputAssembly . sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly . topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly . primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState { };
		viewportState . sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState . viewportCount = 1;
		viewportState . scissorCount  = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer { };
		rasterizer . sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer . depthClampEnable        = VK_FALSE;
		rasterizer . rasterizerDiscardEnable = VK_FALSE;
		rasterizer . polygonMode             = VK_POLYGON_MODE_FILL;
		rasterizer . lineWidth               = 1.0f;
		rasterizer . cullMode                = VK_CULL_MODE_BACK_BIT;
		rasterizer . frontFace               = VK_FRONT_FACE_CLOCKWISE;
		rasterizer . depthBiasEnable         = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling { };
		multisampling . sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling . sampleShadingEnable  = VK_FALSE;
		multisampling . rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment { };
		colorBlendAttachment . colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
							VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment . blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending { };
		colorBlending . sType               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending . logicOpEnable       = VK_FALSE;
		colorBlending . logicOp             = VK_LOGIC_OP_COPY;
		colorBlending . attachmentCount     = 1;
		colorBlending . pAttachments        = &colorBlendAttachment;
		colorBlending . blendConstants[ 0 ] = 0.0f;
		colorBlending . blendConstants[ 1 ] = 0.0f;
		colorBlending . blendConstants[ 2 ] = 0.0f;
		colorBlending . blendConstants[ 3 ] = 0.0f;

		std::vector< VkDynamicState > dynamicStates = {
					VK_DYNAMIC_STATE_VIEWPORT
					, VK_DYNAMIC_STATE_SCISSOR
				};
		VkPipelineDynamicStateCreateInfo dynamicState { };
		dynamicState . sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState . dynamicStateCount = static_cast< uint32_t >( dynamicStates . size( ) );
		dynamicState . pDynamicStates    = dynamicStates . data( );

		VkPipelineLayoutCreateInfo pipelineLayoutInfo { };
		pipelineLayoutInfo . sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo . setLayoutCount         = 0;
		pipelineLayoutInfo . pushConstantRangeCount = 0;

		if ( vkCreatePipelineLayout( m_vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to create pipeline layout!" );

		VkGraphicsPipelineCreateInfo pipelineInfo { };
		pipelineInfo . sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo . stageCount          = 2;
		pipelineInfo . pStages             = shaderStages;
		pipelineInfo . pVertexInputState   = &vertexInputInfo;
		pipelineInfo . pInputAssemblyState = &inputAssembly;
		pipelineInfo . pViewportState      = &viewportState;
		pipelineInfo . pRasterizationState = &rasterizer;
		pipelineInfo . pMultisampleState   = &multisampling;
		pipelineInfo . pColorBlendState    = &colorBlending;
		pipelineInfo . pDynamicState       = &dynamicState;
		pipelineInfo . layout              = pipelineLayout;
		pipelineInfo . renderPass          = renderPass;
		pipelineInfo . subpass             = 0;
		pipelineInfo . basePipelineHandle  = VK_NULL_HANDLE;

		if ( vkCreateGraphicsPipelines( m_vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to create graphics pipeline!" );

		vkDestroyShaderModule( m_vkDevice, fragShaderModule, nullptr );
		vkDestroyShaderModule( m_vkDevice, vertShaderModule, nullptr );
	}

	void createFramebuffers( )
	{
		swapChainFramebuffers . resize( swapChainImageViews . size( ) );

		for ( size_t i = 0; i < swapChainImageViews . size( ); i++ ) {
			VkImageView attachments[ ] = {
						swapChainImageViews[ i ]
					};

			VkFramebufferCreateInfo framebufferInfo { };
			framebufferInfo . sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo . renderPass      = renderPass;
			framebufferInfo . attachmentCount = 1;
			framebufferInfo . pAttachments    = attachments;
			framebufferInfo . width           = swapChainExtent . width;
			framebufferInfo . height          = swapChainExtent . height;
			framebufferInfo . layers          = 1;

			if ( vkCreateFramebuffer( m_vkDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[ i ] ) != VK_SUCCESS )
				throw
						std::runtime_error( "failed to create framebuffer!" );
		}
	}

	void createCommandPool( )
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies( physicalDevice );

		VkCommandPoolCreateInfo poolInfo { };
		poolInfo . sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo . flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo . queueFamilyIndex = queueFamilyIndices . graphicsFamily . value( );

		if ( vkCreateCommandPool( m_vkDevice, &poolInfo, nullptr, &commandPool ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to create command pool!" );
	}

	void createCommandBuffer( )
	{
		VkCommandBufferAllocateInfo allocInfo { };
		allocInfo . sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo . commandPool        = commandPool;
		allocInfo . level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo . commandBufferCount = 1;

		if ( vkAllocateCommandBuffers( m_vkDevice, &allocInfo, &commandBuffer ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to allocate command buffers!" );
	}

	void recordCommandBuffer( VkCommandBuffer commandBuffer, uint32_t imageIndex )
	{
		VkCommandBufferBeginInfo beginInfo { };
		beginInfo . sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if ( vkBeginCommandBuffer( commandBuffer, &beginInfo ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to begin recording command buffer!" );

		VkRenderPassBeginInfo renderPassInfo { };
		renderPassInfo . sType               = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo . renderPass          = renderPass;
		renderPassInfo . framebuffer         = swapChainFramebuffers[ imageIndex ];
		renderPassInfo . renderArea . offset = { 0, 0 };
		renderPassInfo . renderArea . extent = swapChainExtent;

		VkClearValue clearColor          = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
		renderPassInfo . clearValueCount = 1;
		renderPassInfo . pClearValues    = &clearColor;

		vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

		vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline );

		VkViewport viewport { };
		viewport . x        = 0.0f;
		viewport . y        = 0.0f;
		viewport . width    = static_cast< float >( swapChainExtent . width );
		viewport . height   = static_cast< float >( swapChainExtent . height );
		viewport . minDepth = 0.0f;
		viewport . maxDepth = 1.0f;
		vkCmdSetViewport( commandBuffer, 0, 1, &viewport );

		VkRect2D scissor { };
		scissor . offset = { 0, 0 };
		scissor . extent = swapChainExtent;
		vkCmdSetScissor( commandBuffer, 0, 1, &scissor );

		vkCmdDraw( commandBuffer, 3, 1, 0, 0 );

		vkCmdEndRenderPass( commandBuffer );

		if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS ) throw std::runtime_error( "failed to record command buffer!" );
	}

	void createSyncObjects( )
	{
		VkSemaphoreCreateInfo semaphoreInfo { };
		semaphoreInfo . sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo { };
		fenceInfo . sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo . flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if ( vkCreateSemaphore( m_vkDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore ) != VK_SUCCESS ||
			vkCreateSemaphore( m_vkDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore ) != VK_SUCCESS ||
			vkCreateFence( m_vkDevice, &fenceInfo, nullptr, &inFlightFence ) != VK_SUCCESS )
			throw std::runtime_error( "failed to create synchronization objects for a frame!" );
	}

	void drawFrame( )
	{
		vkWaitForFences( m_vkDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX );
		vkResetFences( m_vkDevice, 1, &inFlightFence );

		uint32_t imageIndex;
		vkAcquireNextImageKHR( m_vkDevice, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex );

		vkResetCommandBuffer( commandBuffer, /*VkCommandBufferResetFlagBits*/ 0 );
		recordCommandBuffer( commandBuffer, imageIndex );

		VkSubmitInfo submitInfo { };
		submitInfo . sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore          waitSemaphores[ ] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[ ]     = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo . waitSemaphoreCount        = 1;
		submitInfo . pWaitSemaphores           = waitSemaphores;
		submitInfo . pWaitDstStageMask         = waitStages;

		submitInfo . commandBufferCount = 1;
		submitInfo . pCommandBuffers    = &commandBuffer;

		VkSemaphore signalSemaphores[ ]   = { renderFinishedSemaphore };
		submitInfo . signalSemaphoreCount = 1;
		submitInfo . pSignalSemaphores    = signalSemaphores;

		if ( vkQueueSubmit( graphicsQueue, 1, &submitInfo, inFlightFence ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to submit draw command buffer!" );

		VkPresentInfoKHR presentInfo { };
		presentInfo . sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo . waitSemaphoreCount = 1;
		presentInfo . pWaitSemaphores    = signalSemaphores;

		VkSwapchainKHR swapChains[ ] = { swapChain };
		presentInfo . swapchainCount = 1;
		presentInfo . pSwapchains    = swapChains;

		presentInfo . pImageIndices = &imageIndex;

		vkQueuePresentKHR( presentQueue, &presentInfo );
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &availableFormats )
	{
		for ( const auto &availableFormat: availableFormats )
			if (
				availableFormat . format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat . colorSpace ==
				VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
				return availableFormat;

		return availableFormats[ 0 ];
	}

	VkPresentModeKHR chooseSwapPresentMode( const std::vector< VkPresentModeKHR > &availablePresentModes )
	{
		for ( const auto &availablePresentMode: availablePresentModes )
			if ( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
				return
						availablePresentMode;

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR &capabilities )
	{
		if ( capabilities . currentExtent . width != std::numeric_limits< uint32_t >::max( ) ) return capabilities . currentExtent;
		else {
			int width, height;
			glfwGetFramebufferSize( window, &width, &height );

			VkExtent2D actualExtent = {
						static_cast< uint32_t >( width )
						, static_cast< uint32_t >( height )
					};

			actualExtent . width = std::clamp( actualExtent . width
							, capabilities . minImageExtent . width
							, capabilities . maxImageExtent . width
							);
			actualExtent . height = std::clamp( actualExtent . height
							, capabilities . minImageExtent . height
							, capabilities . maxImageExtent . height
							);

			return actualExtent;
		}
	}

	SwapChainSupportDetails querySwapChainSupport( VkPhysicalDevice device )
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface, &details . capabilities );

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, nullptr );

		if ( formatCount != 0 ) {
			details . formats . resize( formatCount );
			vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, details . formats . data( ) );
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, nullptr );

		if ( presentModeCount != 0 ) {
			details . presentModes . resize( presentModeCount );
			vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, details . presentModes . data( ) );
		}

		return details;
	}

	bool isDeviceSuitable( VkPhysicalDevice device )
	{
		QueueFamilyIndices indices = findQueueFamilies( device );

		bool                    swapChainAdequate = false;
		SwapChainSupportDetails swapChainSupport  = querySwapChainSupport( device );
		swapChainAdequate                         = !swapChainSupport . formats . empty( ) && !swapChainSupport . presentModes . empty( );

		return indices . isComplete( ) && swapChainAdequate;
	}

	QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device )
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

		std::vector< VkQueueFamilyProperties > queueFamilies( queueFamilyCount );
		vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies . data( ) );

		int i = 0;
		for ( const auto &queueFamily: queueFamilies ) {
			if ( queueFamily . queueFlags & VK_QUEUE_GRAPHICS_BIT ) indices . graphicsFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface, &presentSupport );

			if ( presentSupport ) indices . presentFamily = i;

			if ( indices . isComplete( ) ) break;

			i++;
		}

		return indices;
	}
};
