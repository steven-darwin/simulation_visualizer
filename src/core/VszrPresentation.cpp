/**
 * @file VszrPresentation.cpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2025-07-28
 * @date Last Modified : 2025-07-28
 *
 * @brief [Implementation] Class to handle presentation-related things on Vulkan.
 */

#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "core/VszrPresentation.hpp"

VszrPresentation::VszrPresentation(std::shared_ptr<VszrUtility> utility) {
    _utilityHandler = utility;
}

VszrPresentation::~VszrPresentation() {
    // TBA   
}

void VszrPresentation::setVulkanInstance(VkInstance instance) {
    _vulkanInstance = instance;
}

void VszrPresentation::initWindow(const char* title, double width, double height) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

GLFWwindow* VszrPresentation::getWindow() {
    return _window;
}

void VszrPresentation::destroyWindow() {
    glfwDestroyWindow(_window);
}

void VszrPresentation::setLogicalDevice(VkDevice device) {
    _logicalDevice = device;
}

void VszrPresentation::setPresentQueue(VkQueue queue) {
    _presentQueue = queue;
}

void VszrPresentation::createWindowSurface() {
    if (glfwCreateWindowSurface(_vulkanInstance, _window, nullptr, &_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

VkSurfaceKHR VszrPresentation::getWindowSurface() {
    return _surface;
}

void VszrPresentation::destroyWindowSurface() {
    vkDestroySurfaceKHR(_vulkanInstance, _surface, nullptr);
}

void VszrPresentation::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_format_list) {
    for (const auto& available_format : available_format_list) {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            _surfaceFormat = available_format;
            return;
        }
    }

    _surfaceFormat = available_format_list[0];
}

void VszrPresentation::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_mode_list) {
    for (const auto& available_present_mode : available_present_mode_list) {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            _presentMode = available_present_mode;
            return;
        }
    }

    _presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void VszrPresentation::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& available_capability) {
    if (available_capability.currentExtent.width != UINT32_MAX) {
        _extent = available_capability.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, available_capability.minImageExtent.width, available_capability.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, available_capability.minImageExtent.height, available_capability.maxImageExtent.height);

        _extent = actualExtent;
    }
}

void VszrPresentation::createSwapChain(VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list, uint32_t graphics_queue_family_idx, uint32_t presentation_queue_family_idx) {
    chooseSwapExtent(available_capability);
    chooseSwapSurfaceFormat(available_format_list);
    chooseSwapPresentMode(available_present_mode_list);

    uint32_t imageCount = available_capability.minImageCount + 1;
    if (available_capability.maxImageCount > 0 && imageCount > available_capability.maxImageCount) {
        imageCount = available_capability.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = _surfaceFormat.format;
    createInfo.imageColorSpace = _surfaceFormat.colorSpace;
    createInfo.imageExtent = _extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { graphics_queue_family_idx, presentation_queue_family_idx };
    if (graphics_queue_family_idx != presentation_queue_family_idx) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = available_capability.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = _presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(_logicalDevice, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, _swapChainImages.data());
}

void VszrPresentation::destroySwapChain() {
    vkDestroySwapchainKHR(_logicalDevice, _swapChain, nullptr);
}

void VszrPresentation::createImageViews() {
    _swapChainImageViews.resize(_swapChainImages.size());

    for (size_t i = 0; i < _swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = _swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = _surfaceFormat.format;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(_logicalDevice, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }

    }
}

void VszrPresentation::destroyImageViews() {
    for (auto imageView : _swapChainImageViews) {
        vkDestroyImageView(_logicalDevice, imageView, nullptr);
    }
}

VkExtent2D VszrPresentation::getSwapExtent() {
    return _extent;
}

VkSurfaceFormatKHR VszrPresentation::getSwapSurfaceFormat() {
    return _surfaceFormat;
}

VkSwapchainKHR VszrPresentation::getSwapChain() {
    return _swapChain;
}

std::vector<VkImageView>* VszrPresentation::getSwapChainImageViews() {
    return &_swapChainImageViews;
}

VkQueue VszrPresentation::getPresentQueue() {
    return _presentQueue;
}