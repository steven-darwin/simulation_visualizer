/**
 * @file VszrPresentation.hpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2025-07-28
 * @date Last Modified : 2025-07-28
 *
 * @brief [Header] Class to handle presentation-related things on Vulkan.
 */

#ifndef VSZR_PRESENTATION_HPP
#define VSZR_PRESENTATION_HPP

#include <memory>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "core/VszrUtility.hpp"

class VszrPresentation {
public:
    /** Constructor of VszrPresentation object
     */
    VszrPresentation(std::shared_ptr<VszrUtility> utility);

    /** Destructor of VszrPresentation object */
    ~VszrPresentation();

    /** Mutator of _vulkanInstance */
    void setVulkanInstance(VkInstance instance);

    /** Method to initialize window */
    void initWindow(const char* title, double width, double height);

    /** Accessor to window */
    GLFWwindow* getWindow();

    /** Method to destroy window */
    void destroyWindow();

    /** Mutator to logical device */
    void setLogicalDevice(VkDevice device);

    /** Mutator to graphics queue */
    void setPresentQueue(VkQueue queue);

    /** Method to create window surface */
    void createWindowSurface();

    /** Accessor to window surface */
    VkSurfaceKHR getWindowSurface();

    /** Method to destroy window surface */
    void destroyWindowSurface();

    /** Method to choose swap surface format */
    void chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_format_list);

    /** Method to choose swap present mode */
    void chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_mode_list);

    /** Method to choose swap extent */
    void chooseSwapExtent(const VkSurfaceCapabilitiesKHR& available_capability);

    /** Method to create swap chain */
    void createSwapChain(VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list, uint32_t graphics_queue_family_idx, uint32_t presentation_queue_family_idx);
    
    /** Method to destroy swap chain */
    void destroySwapChain();

    /** Method to create image views to swap chain image */
    void createImageViews();

    /** Method to destroy image views to swap chain image */
    void destroyImageViews();

    /** Accessor to swap extent */
    VkExtent2D getSwapExtent();

    /** Accessor to swap extent */
    VkSurfaceFormatKHR getSwapSurfaceFormat();

    /** Accessor to swap chain */
    VkSwapchainKHR getSwapChain();

    /** Accessor to swap chain image views */
    std::vector<VkImageView>* getSwapChainImageViews();

    /** Accessor to get present queue */
    VkQueue getPresentQueue();

private:
    /** Attribute to store image views to swap chain image */
    std::vector<VkImageView> _swapChainImageViews;

    /** Attribute to store swap chain images */
    std::vector<VkImage> _swapChainImages;

    /** Attribute to store handle for swap chain */
    VkSwapchainKHR _swapChain;

    /** Attribute to store chosen swap extent */
    VkExtent2D _extent;

    /** Attribute to store chosen swap present mode */
    VkPresentModeKHR _presentMode;

    /** Attribute to store chosen swap surface format */
    VkSurfaceFormatKHR _surfaceFormat;

    /** Attribute to store handle for present queue */
    VkQueue _presentQueue;

    /** Attriubte to store handle for logical device */
    VkDevice _logicalDevice;

    /** Attribute to store handle for window surface */
    VkSurfaceKHR _surface = nullptr;

    /** Attribute to store handle for window */
    GLFWwindow* _window = nullptr;

    /** Attribute to store Vulkan instance */
    VkInstance _vulkanInstance;

    /** Attribute to store object that will handle utility-related things */
    std::shared_ptr<VszrUtility> _utilityHandler = nullptr;

};

#endif