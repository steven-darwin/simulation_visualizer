/**
 * @file VszrDevice.hpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2025-07-28
 * @date Last Modified : 2025-07-28
 *
 * @brief [Header] Class to handle device-related things on Vulkan.
 */

#ifndef VSZR_DEVICE_HPP
#define VSZR_DEVICE_HPP

#include <unordered_map>
#include <memory>
#include <optional>

#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>

#include "general/Generic.hpp"
#include "core/VszrUtility.hpp"

class VszrDevice {
public:
    /** Constructor of VszrDevice object
     */
    VszrDevice(std::shared_ptr<VszrUtility> utility);

    /** Destructor of VszrDevice object */
    ~VszrDevice();

    /** Mutator of _vulkanInstance */
    void setVulkanInstance(VkInstance instance);

    /** Method to choose physical device */
    void choosePhysicalDevice(VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list, VkSurfaceKHR surface = nullptr);

    /** Method to check physical device suitablity */
    bool checkPhysicalDeviceSutability(VkPhysicalDevice device_candidate, VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list, VkSurfaceKHR surface = nullptr);

    /** Method to find queue families on chosen physical device */
    void getQueueFamilies(VkPhysicalDevice devicee, std::unordered_map<scmp::vszr::CommandType, std::optional<uint32_t>>& queue_familiy_indices, VkSurfaceKHR surface = nullptr);

    /** Method to create logical device */
    void createLogicalDevice();

    /** Method to destroy logical device */
    void destroyLogicalDevice();

    /** Method to get queue */
    VkQueue getQueue(scmp::vszr::CommandType type);

    /** Method to get queue family index */
    uint32_t getQueueFamilyIndex(scmp::vszr::CommandType type);

    /** Method to get required device extension */
    std::vector<std::string> getRequiredDeviceExtension();

    /** Method to check logical device extension support */
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    /** Method to check device swap chain support */
    bool checkDeviceSwapChainSupport(VkPhysicalDevice device, VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list, VkSurfaceKHR surface = nullptr);

    /** Accessor to logical device */
    VkPhysicalDevice getPhysicalDevice();

    /** Accessor to logical device */
    VkDevice getLogicalDevice();

private:
    /** Attribute to store list of device required extensions */
    std::vector<const char*> _reqDeviceExtensionList;
    
    /** Attribute to store logical device */
    VkDevice _logicalDevice = nullptr;

    /** Attribute to store queue family indices  */
    std::unordered_map<scmp::vszr::CommandType, std::optional<uint32_t>> _queueFamilyIndices;

    /** Attribute to store physical device*/
    VkPhysicalDevice _physicalDevice = nullptr;

    /** Attribute to store Vulkan instance */
    VkInstance _vulkanInstance;

    /** Attribute to store object that will handle utility-related things */
    std::shared_ptr<VszrUtility> _utilityHandler = nullptr;

};

#endif