/**
 * @file VszrDevice.cpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2025-07-28
 * @date Last Modified : 2025-07-28
 *
 * @brief [Implementation] Class to handle device-related things on Vulkan.
 */

#include <iostream>
#include <vector>
#include <string>

#include "general/Generic.hpp"
#include "core/VszrDevice.hpp"

VszrDevice::VszrDevice(std::shared_ptr<VszrUtility> utility) {
    _utilityHandler = utility;

    std::vector<std::string> reqDeviceExtensionList = getRequiredDeviceExtension();
    _reqDeviceExtensionList.resize(reqDeviceExtensionList.size());
    for (unsigned short i = 0; i < reqDeviceExtensionList.size(); ++i) {
        char* buffer = new char[reqDeviceExtensionList[i].length() + 1];
        std::memcpy(buffer, reqDeviceExtensionList[i].data(), reqDeviceExtensionList[i].length());
        buffer[reqDeviceExtensionList[i].length()] = '\0';
        _reqDeviceExtensionList[i] = buffer;
    }
}

VszrDevice::~VszrDevice() {
    // TBA
}

void VszrDevice::setVulkanInstance(VkInstance instance) {
    _vulkanInstance = instance;
}

void VszrDevice::choosePhysicalDevice(VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (checkPhysicalDeviceSutability(device, available_capability, available_format_list, available_present_mode_list, surface)) {
            _physicalDevice = device;
            break;
        }
    }

    if (_physicalDevice == nullptr) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool VszrDevice::checkPhysicalDeviceSutability(VkPhysicalDevice device_candidate, VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list, VkSurfaceKHR surface) {
    std::unordered_map<scmp::vszr::CommandType, std::optional<uint32_t>> queue_family_indices;
    getQueueFamilies(device_candidate, queue_family_indices, surface);

    for (auto& iter : queue_family_indices) {
        if (!iter.second.has_value()) {
            return false;
        }
    }

    VkSurfaceCapabilitiesKHR capability_candidate;
    std::vector<VkSurfaceFormatKHR> format_list_candidate;
    std::vector<VkPresentModeKHR> present_mode_list_candidate;

    if (checkDeviceExtensionSupport(device_candidate) && checkDeviceSwapChainSupport(device_candidate, capability_candidate, format_list_candidate, present_mode_list_candidate, surface)) {
        _queueFamilyIndices = queue_family_indices;
        
        available_capability = capability_candidate;
        for (auto format : format_list_candidate) {
            available_format_list.push_back(format);
        }
        for (auto present_mode : present_mode_list_candidate) {
            available_present_mode_list.push_back(present_mode);
        }

        return true;
    }
    else {
        return false;
    }
}

void VszrDevice::getQueueFamilies(VkPhysicalDevice device, std::unordered_map<scmp::vszr::CommandType, std::optional<uint32_t>>& queue_family_indices, VkSurfaceKHR surface) {
    auto queue_list = _utilityHandler->getVulkanConfig().at("logical_device").at("queue");
    for (auto& queue : queue_list.items()) {
        std::string queue_type = queue.key();
        std::optional<uint32_t> queue_idx;

        if (queue_type == "graphics") {
            queue_family_indices.insert(std::make_pair(scmp::vszr::GRAPHICS, queue_idx));
        }
        else if (queue_type == "presentation") {
            queue_family_indices.insert(std::make_pair(scmp::vszr::PRESENTATION, queue_idx));
        }
        else {
            // do nothing
        }
    }

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    unsigned int idx = 0;
    for (const auto& queueFamily : queueFamilies) {
        for (auto& reqQueue : queue_family_indices) {
            switch (reqQueue.first) {
            case scmp::vszr::GRAPHICS:
                if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    reqQueue.second = idx;
                }
                break;
            case scmp::vszr::PRESENTATION:
                VkBool32 present_support = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &present_support);
                if (present_support) {
                    reqQueue.second = idx;
                }
                break;
            }
        }

        idx++;
    }
}

void VszrDevice::createLogicalDevice() {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float queuePriority = 1.0f;
    for (auto queueFamilyIdx : _queueFamilyIndices) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIdx.second.value();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    VkPhysicalDeviceFeatures deviceFeatures{};
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(_reqDeviceExtensionList.size());
    createInfo.ppEnabledExtensionNames = _reqDeviceExtensionList.data();

    if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
}

void VszrDevice::destroyLogicalDevice() {
    vkDestroyDevice(_logicalDevice, nullptr);
}

VkQueue VszrDevice::getQueue(scmp::vszr::CommandType type) {
    VkQueue queue;
    vkGetDeviceQueue(_logicalDevice, _queueFamilyIndices.at(type).value(), 0, &queue);
    return queue;
}

uint32_t VszrDevice::getQueueFamilyIndex(scmp::vszr::CommandType type) {
    return _queueFamilyIndices.at(type).value();
}

std::vector<std::string> VszrDevice::getRequiredDeviceExtension() {
    return _utilityHandler->getVulkanConfig().at("physical_device").at("extension").get<std::vector<std::string>>();
}

bool VszrDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    for (const char* extensionName : _reqDeviceExtensionList) {
        bool extensionFound = false;

        for (const auto& extensionProperties : availableExtensions) {
            if (strcmp(extensionName, extensionProperties.extensionName) == 0) {
                extensionFound = true;
                break;
            }
        }

        if (!extensionFound) {
            return false;
        }
    }

    return true;
}

bool VszrDevice::checkDeviceSwapChainSupport(VkPhysicalDevice device, VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list, VkSurfaceKHR surface) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &available_capability);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        available_format_list.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, available_format_list.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        available_present_mode_list.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, available_present_mode_list.data());
    }

    return !available_format_list.empty() && !available_present_mode_list.empty();
}

VkPhysicalDevice VszrDevice::getPhysicalDevice() {
    return _physicalDevice;
}

VkDevice VszrDevice::getLogicalDevice() {
    return _logicalDevice;
}