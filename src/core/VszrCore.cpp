/**
 * @file VszrCore.cpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2025-07-28
 * @date Last Modified : 2025-07-28
 *
 * @brief [Implementation] Mediator class for visualization core process.
 */

#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_vulkan.h"

#include "general/Generic.hpp"
#include "core/VszrCore.hpp"

void VszrCore::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<VszrCore*>(glfwGetWindowUserPointer(window));
    app->_framebufferResized = true;
}

VszrCore::VszrCore() {
    _utilityHandler = std::make_shared<VszrUtility>("../../config/vulkan_setup.json");
    _deviceHandler = std::make_unique<VszrDevice>(_utilityHandler);
    _graphicsHandler = std::make_unique<VszrGraphics>(_utilityHandler);
    _presentationHandler = std::make_unique<VszrPresentation>(_utilityHandler);

    _maxFrames = _utilityHandler->getVulkanConfig().at("runtime").at("max_frames").get<int>();

    std::vector<std::string> reqInstanceLayerList = getRequiredInstanceLayer();
    _reqInstanceLayerList.resize(reqInstanceLayerList.size());
    for (unsigned short i = 0; i < reqInstanceLayerList.size(); ++i) {
        if (reqInstanceLayerList[i] == "VK_LAYER_KHRONOS_validation") {
            _enabledValidationLayer = true;
        }

        char* buffer = new char[reqInstanceLayerList[i].length() + 1];
        std::memcpy(buffer, reqInstanceLayerList[i].data(), reqInstanceLayerList[i].length());
        buffer[reqInstanceLayerList[i].length()] = '\0';
        _reqInstanceLayerList[i] = buffer;
    }

    std::vector<std::string> reqInstanceExtensionList = getRequiredInstanceExtension();
    _reqInstanceExtensionList.resize(reqInstanceExtensionList.size());
    for (unsigned short i = 0; i < reqInstanceExtensionList.size(); ++i) {
        char* buffer = new char[reqInstanceExtensionList[i].length() + 1];
        std::memcpy(buffer, reqInstanceExtensionList[i].data(), reqInstanceExtensionList[i].length());
        buffer[reqInstanceExtensionList[i].length()] = '\0';
        _reqInstanceExtensionList[i] = buffer;
    }
    if (_enabledValidationLayer) {
        _reqInstanceExtensionList.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}

VszrCore::~VszrCore() {
    // TBA
}

void VszrCore::init() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO();

    std::string window_title = _utilityHandler->getVulkanConfig().at("window").at("title").get<std::string>();
    double window_width = _utilityHandler->getVulkanConfig().at("window").at("width").get<double>();
    double window_height = _utilityHandler->getVulkanConfig().at("window").at("height").get<double>();
    _presentationHandler->initWindow(window_title.c_str(), window_width, window_height);
    glfwSetWindowUserPointer(_presentationHandler->getWindow(), this);
    glfwSetFramebufferSizeCallback(_presentationHandler->getWindow(), framebufferResizeCallback);

    this->createVulkanInstance();
    _deviceHandler->setVulkanInstance(_vulkanInstance);
    _presentationHandler->setVulkanInstance(_vulkanInstance);
    _utilityHandler->setVulkanInstance(_vulkanInstance);

    _utilityHandler->createDebugMessenger(_enabledValidationLayer);

    _presentationHandler->createWindowSurface();

    _deviceHandler->choosePhysicalDevice(_availableCapability, _availableFormatList, _availablePresentModeList, _presentationHandler->getWindowSurface());

    _deviceHandler->createLogicalDevice();
    _logicalDevice = _deviceHandler->getLogicalDevice();
    _graphicsHandler->setLogicalDevice(_deviceHandler->getLogicalDevice());
    _graphicsHandler->setGraphicsQueue(_deviceHandler->getQueue(scmp::vszr::GRAPHICS));
    _presentationHandler->setLogicalDevice(_deviceHandler->getLogicalDevice());
    _presentationHandler->setPresentQueue(_deviceHandler->getQueue(scmp::vszr::PRESENTATION));

    _presentationHandler->createSwapChain(_availableCapability, _availableFormatList, _availablePresentModeList, _deviceHandler->getQueueFamilyIndex(scmp::vszr::GRAPHICS), _deviceHandler->getQueueFamilyIndex(scmp::vszr::PRESENTATION));
    _presentationHandler->createImageViews();

    _graphicsHandler->createRenderPass(_presentationHandler->getSwapSurfaceFormat());
    _graphicsHandler->createDescriptorSetLayout();
    _graphicsHandler->createGraphicsPipeline(_presentationHandler->getSwapExtent());
    _graphicsHandler->createFrambuffer(_presentationHandler->getSwapExtent(), _presentationHandler->getSwapChainImageViews());

    _graphicsHandler->createCommandPool(_deviceHandler->getQueueFamilyIndex(scmp::vszr::GRAPHICS));
    _graphicsHandler->createVertexBuffer(_deviceHandler->getPhysicalDevice());
    _graphicsHandler->createIndexBuffer(_deviceHandler->getPhysicalDevice());
    _graphicsHandler->createUniformBuffers(_deviceHandler->getPhysicalDevice(), _maxFrames);
    _graphicsHandler->createDescriptorPool(_maxFrames);
    _graphicsHandler->createDescriptorSets(_maxFrames);
    _graphicsHandler->createCommandBuffer(_maxFrames);

    this->createSyncObjects();
}

void VszrCore::run() {
    while (!glfwWindowShouldClose(_presentationHandler->getWindow())) {
        glfwPollEvents();
        
        drawFrame(_presentationHandler->getSwapChain(), _graphicsHandler->getCommandBuffer(_currentFrame));
        _currentFrame = (_currentFrame + 1) % _maxFrames;
    }

    vkDeviceWaitIdle(_logicalDevice);
}

void VszrCore::clean() {
    this->cleanupSwapChain();

    this->destroySyncObjects();

    _graphicsHandler->destroyBuffer(_maxFrames);

    _graphicsHandler->destroyCommandPool();

    _graphicsHandler->destroyGraphicsPipeline();
    _graphicsHandler->destroyDescriptorPool();
    _graphicsHandler->destroyDescriptorSetLayout();
    _graphicsHandler->destroyRenderPass();

    _deviceHandler->destroyLogicalDevice();

    _presentationHandler->destroyWindowSurface();

    _utilityHandler->destroyDebugMessenger(_enabledValidationLayer);

    this->destroyVulkanInstance();

    _presentationHandler->destroyWindow();

    //ImGui::DestroyContext();
    
    glfwTerminate();
}

void VszrCore::createVulkanInstance() {
    if (!checkInstanceLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (auto iter = glfwExtensions; iter != glfwExtensions + glfwExtensionCount; iter++) {
        _reqInstanceExtensionList.push_back(*iter);
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(_reqInstanceExtensionList.size());
    createInfo.ppEnabledExtensionNames = _reqInstanceExtensionList.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (_enabledValidationLayer) {
        createInfo.enabledLayerCount = _reqInstanceLayerList.size();
        createInfo.ppEnabledLayerNames = _reqInstanceLayerList.data();

        _utilityHandler->populateDebugMessenger(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }

    if (vkCreateInstance(&createInfo, nullptr, &_vulkanInstance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

}

void VszrCore::destroyVulkanInstance() {
    vkDestroyInstance(_vulkanInstance, nullptr);
}

std::vector<std::string> VszrCore::getRequiredInstanceLayer() {
    return _utilityHandler->getVulkanConfig().at("instance").at("layer").get<std::vector<std::string>>();
}

bool VszrCore::checkInstanceLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : _reqInstanceLayerList) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<std::string> VszrCore::getRequiredInstanceExtension() {
    std::vector<std::string> reqExtensionList;
    return reqExtensionList;
}

bool VszrCore::checkInstanceExtensionSupport() {
    return false;
}

void VszrCore::drawFrame(VkSwapchainKHR swap_chain, VkCommandBuffer command_buffer) {
    vkWaitForFences(_logicalDevice, 1, &_inFlightFence[_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(_logicalDevice, swap_chain, UINT64_MAX, _imageAvailableSemaphore[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain(_presentationHandler->getWindow(), _availableCapability, _availableFormatList, _availablePresentModeList);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(_logicalDevice, 1, &_inFlightFence[_currentFrame]);

    vkResetCommandBuffer(command_buffer, 0);
    _graphicsHandler->recordCommandBuffer(command_buffer, imageIndex, _presentationHandler->getSwapExtent(), _currentFrame);

    _graphicsHandler->updateUniformBuffer(_currentFrame, _presentationHandler->getSwapExtent());

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore[_currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer;

    VkSemaphore signalSemaphores[] = { _renderFinishedSemaphore[_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_graphicsHandler->getGraphicsQueue(), 1, &submitInfo, _inFlightFence[_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swap_chain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_presentationHandler->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized) {
        recreateSwapChain(_presentationHandler->getWindow(), _availableCapability, _availableFormatList, _availablePresentModeList);
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

void VszrCore::createSyncObjects() {
    _imageAvailableSemaphore.resize(_maxFrames);
    _renderFinishedSemaphore.resize(_maxFrames);
    _inFlightFence.resize(_maxFrames);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < _maxFrames; i++) {
        if (vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &_imageAvailableSemaphore[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &_renderFinishedSemaphore[i]) != VK_SUCCESS ||
            vkCreateFence(_logicalDevice, &fenceInfo, nullptr, &_inFlightFence[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphores!");
        }
    }
}

void VszrCore::destroySyncObjects() {
    for (size_t i = 0; i < _maxFrames; i++) {
        vkDestroySemaphore(_logicalDevice, _imageAvailableSemaphore[i], nullptr);
        vkDestroySemaphore(_logicalDevice, _renderFinishedSemaphore[i], nullptr);
        vkDestroyFence(_logicalDevice, _inFlightFence[i], nullptr);
    }
}

void VszrCore::recreateSwapChain(GLFWwindow* window, VkSurfaceCapabilitiesKHR& available_capability, std::vector<VkSurfaceFormatKHR>& available_format_list, std::vector<VkPresentModeKHR>& available_present_mode_list) {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(_logicalDevice);

    cleanupSwapChain();

    _presentationHandler->createSwapChain(available_capability, available_format_list, available_present_mode_list, _deviceHandler->getQueueFamilyIndex(scmp::vszr::GRAPHICS), _deviceHandler->getQueueFamilyIndex(scmp::vszr::PRESENTATION));
    _presentationHandler->createImageViews();
    _graphicsHandler->createFrambuffer(_presentationHandler->getSwapExtent(), _presentationHandler->getSwapChainImageViews());
}

void VszrCore::cleanupSwapChain() {
    _graphicsHandler->destroyFramebuffer();
    _presentationHandler->destroyImageViews();
    _presentationHandler->destroySwapChain();
}