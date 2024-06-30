#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H  
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>

#include <array>
#include <vector>
#include <optional>
#include <string>

struct queueFamilyIndices_t {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	inline bool isComplete(void) {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct swapChainSupportDetails_t {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct vertex_t {
	glm::vec2 pos;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(vertex_t);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(vertex_t, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(vertex_t, texCoord);

		return attributeDescriptions;
	}
};

struct uniformBuffer_t {
	glm::vec4 heights;
};

struct renderer_t {
	const char* appName_c = "editText_inator";
	const uint32_t version_c = 01; // 01 = 0.1
	const std::vector<const char*> deviceExtensions_c = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	static constexpr uint8_t maxFramesInFlight_c = 2;

	GLFWwindow* window;
	uint32_t winSizeX = 800, winSizeY = 800;
	bool framebuffResized = false;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkCommandPool commandPool;
	std::array<VkBuffer, maxFramesInFlight_c> vertexBuffer;
	std::array<VkDeviceMemory, maxFramesInFlight_c> vertexBufferMemory;
	std::array<VkBuffer, maxFramesInFlight_c> indexBuffer;
	std::array<VkDeviceMemory, maxFramesInFlight_c> indexBufferMemory;

	std::vector<VkImage> textureImage;
	std::vector<VkDeviceMemory> textureImageMemory;
	std::vector<VkImageView> textureImageView;
	std::vector<VkSampler> textureSampler;

	static const uint8_t fontSize_c = 27;
	static const uint32_t glyphsNum_c = 255;
	struct glyphInfo_t {
		int32_t x0, y0, x1, y1;    // coords of glyph in the texture atlas
		int32_t x_off, y_off;    // left & top bearing when rendering
		int32_t advance;            // x advance when rendering
	};
	double fontTexSize;
	glyphInfo_t m_info[glyphsNum_c];

	std::vector<vertex_t> vertices = {};
	std::vector<uint32_t> indices = {};

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t currentFrame = 0;

	void initRenderer(void);
	
	static void framebuffResizeCallback(GLFWwindow*, int32_t, int32_t);
	void initWindow(void);
	void initVk(void);
	void createWinSurface(void);

	void createImage(uint32_t, uint32_t, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, VkImage&, VkDeviceMemory&);
	void transitionImageLayout(VkImage, VkFormat, VkImageLayout, VkImageLayout);
	void createFontTexture(void);

	queueFamilyIndices_t findQueueFamilies(VkPhysicalDevice _device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice _device);
	swapChainSupportDetails_t querySwapChainSupport(VkPhysicalDevice _device);
	uint32_t findMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties);
	bool isDeviceSuitable(VkPhysicalDevice _device);
	void pickPhysicalDevice(void);
	void createLogicalDevice(void);
	void createSwapChain(void);

	void createVkInstance(void);
	void setupDebugMessenger(void);
	void createImageViews(void);
	void createRenderPass(void);
	void createDescriptorSetLayout(void);
	void createGraphicsPipeline(void);
	void createFramebuffers(void);
	void createCommandPool(void);
	VkImageView createImageView(VkImage _image, VkFormat _format);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	VkCommandBuffer beginSingleTimeCommands(void);
	void endSingleTimeCommands(VkCommandBuffer);
	void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize);
	void createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
	void createVertexBuffers(void);
	void createIndexBuffer(void);
	void createUniformBuffers(void);
	void createDescriptorPool(void);
	void createDescriptorSets(void);
	void createCommandBuffers(void);
	void createSyncObjects(void);
	
	void recreateSwapChain(void);
	void recordCommandBuffer(VkCommandBuffer, uint32_t);

	void clearBuffers(void);
	void pushQuad(glm::vec2 _pos, glm::vec2 _size, glm::vec2 _texPos, glm::vec2 _texSize);
	void pushText(glm::vec2 _pos, const std::string& _str);
	void updateUBO(void);
	void updateRenderBuffers(void);
	void drawFrame(void);

	void cleanupSwapChain(void);
	void cleanup(void);
};