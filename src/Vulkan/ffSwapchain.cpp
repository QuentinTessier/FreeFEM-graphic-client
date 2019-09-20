#include "ffSwapchain.h"

namespace ffGraph {
namespace Vulkan {

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

SwapchainSupportDetails GetSwapchainSupport(const VkPhysicalDevice& PhysicalDevice, const VkSurfaceKHR& Surface)
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &details.Capabilities);

    uint32_t FormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, 0);

    if (FormatCount > 0) {
        details.Formats.resize(FormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, details.Formats.data());
    }

    uint32_t PresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, 0);

    if (PresentModeCount > 0) {
        details.PresentModes.resize(PresentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, details.PresentModes.data());
    }
    return details;
}

VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats)
{
    for (const auto& format : AvailableFormats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }
    return AvailableFormats[0];
}

VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentMode)
{
    for (const auto& PresentMode : AvailablePresentMode) {
        if (PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return PresentMode;
    }
    return AvailablePresentMode[0];
}

VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& Capabilities, const NativeWindow& Window)
{
    VkExtent2D FinalExtent = {0, 0};

    FinalExtent.width = std::max(Capabilities.minImageExtent.width, std::min(Capabilities.maxImageExtent.width, Window.WindowSize.width));
    FinalExtent.height = std::max(Capabilities.minImageExtent.height, std::min(Capabilities.maxImageExtent.height, Window.WindowSize.height));

    return FinalExtent;
}

void ffCreateSwapchainImageViews(ffSwapchain& Swapchain, const VkDevice& Device)
{
    Swapchain.Views.resize(Swapchain.Images.size());

    VkImageViewCreateInfo CreateInfos = {};
    CreateInfos.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    CreateInfos.viewType = VK_IMAGE_VIEW_TYPE_2D;
    CreateInfos.format = Swapchain.Format.format;
    CreateInfos.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    CreateInfos.subresourceRange.baseArrayLayer = 0;
    CreateInfos.subresourceRange.levelCount = 1;
    CreateInfos.subresourceRange.baseArrayLayer = 0;
    CreateInfos.subresourceRange.layerCount = 1;

    for (size_t i = 0; i < Swapchain.Images.size(); ++i) {
        CreateInfos.image = Swapchain.Images[i];

        if (vkCreateImageView(Device, &CreateInfos, 0, &Swapchain.Views[i]))
            return;
    }
}

ffSwapchain ffNewSwapchain(const VkPhysicalDevice& PhysicalDevice, const VkDevice& Device, const VkSurfaceKHR Surface, const NativeWindow& Window)
{
    ffSwapchain n;
    SwapchainSupportDetails Details = GetSwapchainSupport(PhysicalDevice, Surface);

    VkSurfaceFormatKHR SurfaceFormat = ChooseSwapchainSurfaceFormat(Details.Formats);
    VkPresentModeKHR PresentMode = ChooseSwapchainPresentMode(Details.PresentModes);
    VkExtent2D Extent = chooseSwapchainExtent(Details.Capabilities, Window);

    uint32_t ImageCount = Details.Capabilities.minImageCount + 1;
    if (Details.Capabilities.maxImageCount > 0 && ImageCount < Details.Capabilities.maxImageCount)
        ImageCount = Details.Capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR SwapchainCreateInfos = {};
    SwapchainCreateInfos.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfos.surface = Surface;

    SwapchainCreateInfos.minImageCount = ImageCount;
    SwapchainCreateInfos.imageFormat = SurfaceFormat.format;
    SwapchainCreateInfos.imageColorSpace = SurfaceFormat.colorSpace;
    SwapchainCreateInfos.imageExtent = Extent;
    SwapchainCreateInfos.imageArrayLayers = 1;
    SwapchainCreateInfos.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    SwapchainCreateInfos.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    SwapchainCreateInfos.preTransform = Details.Capabilities.currentTransform;
    SwapchainCreateInfos.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfos.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(Device, &SwapchainCreateInfos, 0, &n.Handle))
        return n;
    n.Format = SurfaceFormat;
    uint32_t ImageCount = 0;
    vkGetSwapchainImagesKHR(Device, n.Handle, &ImageCount, 0);
    n.Images.resize(ImageCount);
    vkGetSwapchainImagesKHR(Device, n.Handle, &ImageCount, n.Images.data());

    return n;
}

}
}