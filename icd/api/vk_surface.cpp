/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2014-2019 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/
/**
***********************************************************************************************************************
* @file  vk_surface.cpp
* @brief Contains implementation of Vulkan Surface object.
***********************************************************************************************************************
*/
#include "vk_display_manager.h"
#include "vk_instance.h"
#include "vk_physical_device_manager.h"
#include "vk_surface.h"

namespace vk
{

// =====================================================================================================================
VkResult Surface::Create(
    Instance*                           pInstance,
    const VkStructHeader*               pCreateInfo,
    const VkAllocationCallbacks*        pAllocator,
    VkSurfaceKHR*                       pSurfaceHandle)
{
    Pal::OsDisplayHandle osDisplayHandle = 0;

    VkIcdSurfaceXcb  xcbSurface  = {};
    VkIcdSurfaceXlib xlibSurface = {};
    VkIcdSurfaceDisplay displaySurface = {};
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    VkIcdSurfaceWayland waylandSurface = {};
#endif

    union
    {
        const VkStructHeader*                    pHeader;

        const VkXcbSurfaceCreateInfoKHR*             pVkXcbSurfaceCreateInfoKHR;
        const VkXlibSurfaceCreateInfoKHR*            pVkXlibSurfaceCreateInfoKHR;
        const VkDisplaySurfaceCreateInfoKHR*         pVkDisplaySurfaceCreateInfoKHR;
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        const VkWaylandSurfaceCreateInfoKHR*     pVkWaylandSurfaceCreateInfoKHR;
#endif
    };

    VkResult result = VK_SUCCESS;

    for (pHeader = pCreateInfo; pHeader != nullptr; pHeader = pHeader->pNext)
    {
        switch (static_cast<uint32_t>(pHeader->sType))
        {

        case VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR:
        {
            xcbSurface.base.platform = VK_ICD_WSI_PLATFORM_XCB;
            xcbSurface.connection    = pVkXcbSurfaceCreateInfoKHR->connection;
            xcbSurface.window        = pVkXcbSurfaceCreateInfoKHR->window;
            break;
        }

        case VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR:
        {
            xlibSurface.base.platform = VK_ICD_WSI_PLATFORM_XLIB;
            xlibSurface.dpy           = pVkXlibSurfaceCreateInfoKHR->dpy;
            xlibSurface.window        = pVkXlibSurfaceCreateInfoKHR->window;
            break;
        }

        case VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR:
        {
            displaySurface.base.platform = VK_ICD_WSI_PLATFORM_DISPLAY;
            displaySurface.displayMode = pVkDisplaySurfaceCreateInfoKHR->displayMode;
            displaySurface.planeIndex  = pVkDisplaySurfaceCreateInfoKHR->planeIndex;
            displaySurface.planeStackIndex = pVkDisplaySurfaceCreateInfoKHR->planeStackIndex;
            displaySurface.transform = pVkDisplaySurfaceCreateInfoKHR->transform;
            displaySurface.globalAlpha = pVkDisplaySurfaceCreateInfoKHR->globalAlpha;
            displaySurface.alphaMode   = pVkDisplaySurfaceCreateInfoKHR->alphaMode;
            displaySurface.imageExtent.width  = pVkDisplaySurfaceCreateInfoKHR->imageExtent.width;
            displaySurface.imageExtent.height = pVkDisplaySurfaceCreateInfoKHR->imageExtent.height;
            break;
        }
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        case VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR:
        {
            waylandSurface.base.platform = VK_ICD_WSI_PLATFORM_WAYLAND;
            waylandSurface.display = pVkWaylandSurfaceCreateInfoKHR->display;
            waylandSurface.surface = pVkWaylandSurfaceCreateInfoKHR->surface;
            break;
        }
#endif
        default:
            break;
        };
    }

    if (pCreateInfo == nullptr)
    {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // allocate the memory
    const VkAllocationCallbacks* pAllocCB = pAllocator ? pAllocator : pInstance->GetAllocCallbacks();

    void* pMemory = pAllocCB->pfnAllocation(pAllocCB->pUserData,
                                            sizeof(Surface),
                                            VK_DEFAULT_MEM_ALIGN,
                                            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (pMemory == nullptr)
    {
        result = VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    else
    {
        Surface* pSurface = nullptr;

        if (xcbSurface.base.platform == VK_ICD_WSI_PLATFORM_XCB)
        {
            pSurface = VK_PLACEMENT_NEW(pMemory) Surface(pInstance, osDisplayHandle, xcbSurface);
        }
        else if (displaySurface.base.platform == VK_ICD_WSI_PLATFORM_DISPLAY)
        {
            pSurface = VK_PLACEMENT_NEW(pMemory) Surface(pInstance, osDisplayHandle, displaySurface);
        }
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        else if (waylandSurface.base.platform == VK_ICD_WSI_PLATFORM_WAYLAND)
        {
            pSurface = VK_PLACEMENT_NEW(pMemory) Surface(pInstance, osDisplayHandle, waylandSurface);
        }
#endif
        else
        {
            pSurface = VK_PLACEMENT_NEW(pMemory) Surface(pInstance, osDisplayHandle, xlibSurface);
        }
        *pSurfaceHandle = Surface::HandleFromObject(pSurface);
    }

    return result;
}

// =====================================================================================================================
void Surface::Destroy(
    Instance*                        pInstance,
    const VkAllocationCallbacks*     pAllocator)
{
    pInstance->FreeMem(reinterpret_cast<void*>(this));
}

namespace entry
{

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateXcbSurfaceKHR(
    VkInstance                          instance,
    const VkXcbSurfaceCreateInfoKHR*    pCreateInfo,
    const VkAllocationCallbacks*        pAllocator,
    VkSurfaceKHR*                       pSurface)
{
    return Surface::Create(Instance::ObjectFromHandle(instance),
        reinterpret_cast<const VkStructHeader*>(pCreateInfo), pAllocator, pSurface);
}

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateXlibSurfaceKHR(
    VkInstance                          instance,
    const VkXlibSurfaceCreateInfoKHR*   pCreateInfo,
    const VkAllocationCallbacks*        pAllocator,
    VkSurfaceKHR*                       pSurface)
{
    return Surface::Create(Instance::ObjectFromHandle(instance),
        reinterpret_cast<const VkStructHeader*>(pCreateInfo), pAllocator, pSurface);
}
// =====================================================================================================================
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
VKAPI_ATTR VkResult VKAPI_CALL vkCreateWaylandSurfaceKHR(
    VkInstance                          instance,
    const VkWaylandSurfaceCreateInfoKHR*   pCreateInfo,
    const VkAllocationCallbacks*        pAllocator,
    VkSurfaceKHR*                       pSurface)
{
    return Surface::Create(Instance::ObjectFromHandle(instance),
        reinterpret_cast<const VkStructHeader*>(pCreateInfo), pAllocator, pSurface);
}
#endif

// =====================================================================================================================
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDisplayPlaneSurfaceKHR(
    VkInstance                                  instance,
    const VkDisplaySurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface)
{
    return Surface::Create(Instance::ObjectFromHandle(instance),
        reinterpret_cast<const VkStructHeader*>(pCreateInfo), pAllocator, pSurface);
}

// =====================================================================================================================
VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(
    VkInstance                                   instance,
    VkSurfaceKHR                                 surface,
    const VkAllocationCallbacks*                 pAllocator)
{
    Surface::ObjectFromHandle(surface)->Destroy(Instance::ObjectFromHandle(instance), pAllocator);
}

} // namespace entry
} // namespace vk
