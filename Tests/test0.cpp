#include <gtest/gtest.h>
#include <fstream>
#include <vulkan/vulkan.hpp>
#include <optional>
#include "shader/shader_comp.h"
struct QueueInfo
{
    std::optional<uint32_t> family;
} queueInfo;

vk::Instance instance;
vk::PhysicalDevice phyDevice;
vk::Device              device;
vk::Queue              computeQueue;
vk::DescriptorSetLayout setLayout;

vk::Instance CreateInstance();
vk::PhysicalDevice PickupPhysicalDevice();
vk::Device         CreateDevice();
vk::DescriptorSetLayout CreateSetLayout();
vk::Pipeline            CreateComputePipline();

TEST(VCS, SimpleEx)
{
    instance = CreateInstance();
    phyDevice = PickupPhysicalDevice();
    device    = CreateDevice();
    computeQueue = device.getQueue(queueInfo.family.value(), 0);
    setLayout    = CreateSetLayout();

}

vk::Instance CreateInstance()
{
    vk::InstanceCreateInfo createInfo;
    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    createInfo.setPEnabledLayerNames(layers);
    vk::ApplicationInfo appinfo;
    appinfo.setApiVersion(VK_API_VERSION_1_3);
    createInfo.setPApplicationInfo(&appinfo);

    return vk::createInstance(createInfo);
}
vk::PhysicalDevice PickupPhysicalDevice()
{
    return instance.enumeratePhysicalDevices()[0];
}

vk::Device CreateDevice()
{
    auto properties = phyDevice.getQueueFamilyProperties();
    for (size_t i = 0; i < properties.size(); i++) {
        if (properties[i].queueFlags & vk::QueueFlagBits::eCompute) {
            queueInfo.family = i;
        }
    }

    if (!queueInfo.family) {
        return nullptr;
    }


    vk::DeviceCreateInfo createInfo;
    vk::DeviceQueueCreateInfo queueCreateInfo;
    float                     prioritiy = 1.0;
    queueCreateInfo.setQueueFamilyIndex(queueInfo.family.value())
        .setQueuePriorities(prioritiy)
        .setQueueCount(1);
    createInfo.setQueueCreateInfos(queueCreateInfo);
    return phyDevice.createDevice(createInfo);
}

vk::DescriptorSetLayout CreateSetLayout()
{
    vk::DescriptorSetLayoutCreateInfo createInfo;
    vk::DescriptorSetLayoutBinding    binding;
    binding.setBinding(0)
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setDescriptorCount(1)
        .setStageFlags(vk::ShaderStageFlagBits::eCompute);
    createInfo.setBindings(binding);
    return device.createDescriptorSetLayout(createInfo);
}

vk::Pipeline CreateComputePipline()
{

}


std::vector<char> ReadWholeFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cout << "read " << filename << " failed" << std::endl;
        return std::vector<char>{};
    }

    auto              size = file.tellg();
    std::vector<char> content(size);
    file.seekg(0);

    file.read(content.data(), content.size());

    return content;
}