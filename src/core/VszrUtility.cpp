/**
 * @file VszrUtility.cpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2025-10-13
 * @date Last Modified : 2025-10-13
 *
 * @brief [Implementation] Class to store visualizer utility function
 */

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "core/VszrUtility.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL VszrUtility::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult VszrUtility::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VszrUtility::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VszrUtility::VszrUtility(const char* config_file_path) {
    std::ifstream vulkan_config_json(config_file_path);
    _vulkanConfig = json::parse(vulkan_config_json);
}

VszrUtility::~VszrUtility() {
    // TBA
}

json VszrUtility::getVulkanConfig() {
    return _vulkanConfig;
}

void VszrUtility::setVulkanInstance(VkInstance instance) {
    _vulkanInstance = instance;
}

void VszrUtility::createDebugMessenger(bool enable_validation_layer) {
    if (!enable_validation_layer) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessenger(createInfo);

    if (CreateDebugUtilsMessengerEXT(_vulkanInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}
void VszrUtility::populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VszrUtility::destroyDebugMessenger(bool enable_validation_layer) {
    if (enable_validation_layer) {
        DestroyDebugUtilsMessengerEXT(_vulkanInstance, debugMessenger, nullptr);
    }
}