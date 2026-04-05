/**
 * @file VszrCore.hpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2025-07-28
 * @date Last Modified : 2025-07-28
 *
 * @brief [Header] Mediator class for visualization core process.
 */

#ifndef VSZR_CORE_HPP
#define VSZR_CORE_HPP

#include <string>
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>

#include "core/VszrDevice.hpp"
#include "core/VszrGraphics.hpp"
#include "core/VszrPresentation.hpp"
#include "core/VszrUtility.hpp"

class VszrCore {
public:
    /** Callback function for when framebufer is resized */
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    /** Constructor of VszrCore object */
    VszrCore();

    /** Destructor of VszrCore object */
    ~VszrCore();

    /** Method to initialize and setup Vulkan */
    void init();

    /** Method to run Vulkan */
    void run();

    /** Method to clean and destroy Vulkan */
    void clean();

    /** Method to create Vulkan instance */
    void createVulkanInstance();

    /** Method to destroy Vulkan instance */
    void destroyVulkanInstance();

    /** Method to get required instance layer */
    std::vector<std::string> getRequiredInstanceLayer();

    /** Method to check instance layer support */
    bool checkInstanceLayerSupport();

    /** Method to get required instance extension */
    std::vector<std::string> getRequiredInstanceExtension();

    /** Method to check instance extension support */
    bool checkInstanceExtensionSupport();

    /** Method to draw frame */
    void drawFrame(VkSwapchainKHR swap_chain, VkCommandBuffer command_buffer);

    /** Method to create sync objects */
    void createSyncObjects();

    /** Method to destroy sync objects */
    void destroySyncObjects();

    /** Method to recreate swap chain */
    void recreateSwapChain(GLFWwindow* window, VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list);

    /** Method to clean up swap chain */
    void cleanupSwapChain();
private:
    /** Attribute to store an indicator if framebuffer is resized */
    bool _framebufferResized = false;

    /** Attribute to store current frame index in flight */
    int _currentFrame = 0;

    /** Attribute to store max frames in flight */
    int _maxFrames;

    /** Semaphore to signal available image */
    std::vector<VkSemaphore> _imageAvailableSemaphore;

    /** Semaphore to signal finished rendering process */
    std::vector<VkSemaphore> _renderFinishedSemaphore;

    /** Fence to ... */
    std::vector<VkFence> _inFlightFence;

    /** Attribute to store handle for logical device */
    VkDevice _logicalDevice;

    /** Attribute to store available surface present mode */
    std::vector<VkPresentModeKHR> _availablePresentModeList;
    
    /** Attribute to store available surface format */
    std::vector<VkSurfaceFormatKHR> _availableFormatList;
    
    /** Attribute to store available surface capability */
    VkSurfaceCapabilitiesKHR _availableCapability;

    /** Attribute to store list of instance required extensions */
    std::vector<const char*> _reqInstanceExtensionList;

    /** Attribute to store list of instance required layers */
    std::vector<const char*> _reqInstanceLayerList;

    /** Attribute to determine if validation layer is enabled */
    bool _enabledValidationLayer = false;

    /** Attribute to store Vulkan instance */
    VkInstance _vulkanInstance;

    /** Attribute to store object that will handle device-related things */
    std::unique_ptr<VszrDevice> _deviceHandler = nullptr;

    /** Attribute to store object that will handle graphics-related things */
    std::unique_ptr<VszrGraphics> _graphicsHandler = nullptr;

    /** Attribute to store object that will handle presentation-related things */
    std::unique_ptr<VszrPresentation> _presentationHandler = nullptr;

    /** Attribute to store object that will handle utility-related things */
    std::shared_ptr<VszrUtility> _utilityHandler = nullptr;
};

#endif