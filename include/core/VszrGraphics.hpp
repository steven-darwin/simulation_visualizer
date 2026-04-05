/**
 * @file VszrGraphics.hpp
 * @author Steven Darwin
 * @version 0.0.1
 * @date Created : 2025-07-28
 * @date Last Modified : 2025-07-28
 *
 * @brief [Header] Class to handle graphics-related things on Vulkan.
 */

#ifndef VSZR_GRAPHICS_HPP
#define VSZR_GRAPHICS_HPP

#include <vector>
#include <array>
#include <memory>
#include <chrono>

#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/VszrUtility.hpp"

class VszrGraphics {
public:
    /** Struct to store vertex data */
    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
    };

    /** Struct to store uniform variable */
    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    /** Constructor of VszrGraphics object
     */
    VszrGraphics(std::shared_ptr<VszrUtility> utility);

    /** Destructor of VszrGraphics object */
    ~VszrGraphics();

    /** Mutator to logical device */
    void setLogicalDevice(VkDevice device);

    /** Mutator to graphics queue */
    void setGraphicsQueue(VkQueue queue);

    /** Method to create rendering pipeline */
    void createGraphicsPipeline(VkExtent2D extent);

    /** Method to destroy rendering pipeline */
    void destroyGraphicsPipeline();

    /** Method to read compiled shader code */
    std::vector<char> readShaderCode(const std::string& file_path);

    /** Method to create shader module */
    VkShaderModule createShaderModule(const std::vector<char>& code);

    /** Method to destroy shader module */
    void destroyShaderModule(VkShaderModule module);

    /** Method to create render pass */
    void createRenderPass(VkSurfaceFormatKHR surface_format);

    /** Method to destroy render pass */
    void destroyRenderPass();

    /** Method to create framebuffer */
    void createFrambuffer(VkExtent2D extent, std::vector<VkImageView>* image_view_list);

    /** Method to destroy frambuffer */
    void destroyFramebuffer();

    /** Method to create command pool */
    void createCommandPool(uint32_t graphics_queue_family_idx);

    /** Method to destroy command pool */
    void destroyCommandPool();

    /** Method to create command buffer */
    void createCommandBuffer(int max_frames);

    /** Method to record command buffer */
    void recordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index, VkExtent2D extent, int current_frame);

    /** Accessor to command buffer */
    VkCommandBuffer getCommandBuffer(int index);

    /** Accessor to graphics queue */
    VkQueue getGraphicsQueue();

    /** Method to create abstract buffer */
    void createBuffer(VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);

    /** Method to create vertex buffer */
    void createVertexBuffer(VkPhysicalDevice physical_device);

    /** Method to create index buffer */
    void createIndexBuffer(VkPhysicalDevice physical_device);

    /** Method to create copy buffer */
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    /** Method to create uniform buffers */
    void createUniformBuffers(VkPhysicalDevice physical_device, int max_frames);

    /** Method to update uniform buffers */
    void updateUniformBuffer(uint32_t current_image, VkExtent2D extent);
    
    /** Method to destroy buffer */
    void destroyBuffer(int max_frames);

    /** Method to create descriptor set layout */
    void createDescriptorSetLayout();

    /** Method to destroy descriptor set layout */
    void destroyDescriptorSetLayout();

    /** Method to create descriptor pool */
    void createDescriptorPool(int max_frames);

    /** Method to destroy descriptor pool */
    void destroyDescriptorPool();

    /** Method to create descriptor pool */
    void createDescriptorSets(int max_frames);

private:
    /** Attribute to store descriptor sets */
    std::vector<VkDescriptorSet> _descriptorSets;

    /** Attribute to store handle for desciptor pool */
    VkDescriptorPool _descriptorPool;

    /** Attribute to store handle for mapped uniform buffers */
    std::vector<void*> _uniformBuffersMapped;

    /** Attribute to store handle for uniform buffer memory */
    std::vector<VkDeviceMemory> _uniformBuffersMemory;

    /** Attribute to store handle for uniform buffer */
    std::vector<VkBuffer> _uniformBuffers;

    /** Attribute to store handle for index buffer memory */
    VkDeviceMemory _indexBufferMemory;

    /** Attribute to store handle for index buffer */
    VkBuffer _indexBuffer;

    /** Attribute to store index data */
    std::vector<uint16_t> _indices;

    /** Attribute to store handle for vertex buffer memory */
    VkDeviceMemory _vertexBufferMemory;

    /** Attribute to store handle for vertex buffer */
    VkBuffer _vertexBuffer;

    /** Attribute to store vertex data */
    std::vector<Vertex> _vertices;

    /** Attribute to store handle for command buffer */
    std::vector<VkCommandBuffer> _commandBuffers;

    /** Attribute to store handle for command pool */
    VkCommandPool _commandPool;

    /** Attribute to store handle for framebuffer */
    std::vector<VkFramebuffer> _swapChainFramebuffers;

    /** Attribute to store handle for render pass */
    VkRenderPass _renderPass;

    /** Attribute to store handle for desciptor set layout */
    VkDescriptorSetLayout _descriptorSetLayout;

    /** Attribute to store handle for pipeline layout */
    VkPipelineLayout _pipelineLayout;

    /** Attribute to store handle for rendering pipeline */
    VkPipeline _graphicsPipeline;

    /** Attribute to store handle for graphics queue */
    VkQueue _graphicsQueue;

    /** Attriubte to store handle for logical device */
    VkDevice _logicalDevice;

    /** Attribute to store object that will handle utility-related things */
    std::shared_ptr<VszrUtility> _utilityHandler = nullptr;

};

#endif