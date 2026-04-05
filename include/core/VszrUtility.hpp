/**
 * @file VszrUtility.hpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2025-10-13
 * @date Last Modified : 2025-10-13
 *
 * @brief [Header] Class to store visualizer utility function.
 */

#ifndef VSZR_UTILITY_HPP
#define VSZR_UTILITY_HPP

#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>

class VszrUtility {
public:
    /** Callback function for debugging */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    /** Proxy to vkCreateDebugUtilsMessengerEXT */
    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    /** Proxy to vkDestroyDebugUtilsMessengerEXT */
    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    /** Constructor of VszrUtility object
     */
    VszrUtility(const char* config_file_path);

    /** Destructor of VszrUtility object */
    ~VszrUtility();

    /** Accessor of vulkan config */
    json getVulkanConfig();

    /** Mutator of _vulkanInstance */
    void setVulkanInstance(VkInstance instance);

    /** Method to setup debug messenger */
    void createDebugMessenger(bool enable_validation_layer);
    
    /** Method to populate debug messenger struct */
    void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    /** Method to destroy debug messenger */
    void destroyDebugMessenger(bool enable_validation_layer);

private:
    /** Attribute to store debugging function handle */
    VkDebugUtilsMessengerEXT debugMessenger;

    /** Attribute to store vulkan config */
    json _vulkanConfig;

    /** Attribute to store Vulkan instance */
    VkInstance _vulkanInstance;
};

#endif