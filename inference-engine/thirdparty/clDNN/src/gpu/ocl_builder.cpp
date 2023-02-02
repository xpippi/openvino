// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "ocl_builder.h"
#include "configuration.h"
#include "include/to_string_utils.h"
#include "api/device.hpp"
#include <string>
#include <vector>
#include <list>
#include <utility>

// NOTE: Due to buggy scope transition of warnings we need to disable warning in place of use/instantation
//       of some types (even though we already disabled them in scope of definition of these types).
//       Moreover this warning is pretty much now only for annoyance: it is generated due to lack
//       of proper support for mangling of custom GCC attributes into type name (usually when used
//       with templates, even from standard library).
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wignored-attributes"
#endif

namespace cldnn {
namespace gpu {
static constexpr auto INTEL_PLATFORM_VENDOR = "Intel(R) Corporation";

static std::vector<cl::Device> getSubDevices(cl::Device& rootDevice) {
    cl_uint maxSubDevices;
    size_t maxSubDevicesSize;
    const auto err = clGetDeviceInfo(rootDevice(),
                                     CL_DEVICE_PARTITION_MAX_SUB_DEVICES,
                                     sizeof(maxSubDevices),
                                     &maxSubDevices, &maxSubDevicesSize);

    if (err != CL_SUCCESS || maxSubDevicesSize != sizeof(maxSubDevices)) {
        throw cl::Error(err, "clGetDeviceInfo(..., CL_DEVICE_PARTITION_MAX_SUB_DEVICES,...)");
    }

    if (maxSubDevices == 0) {
        return {};
    }

    const auto partitionProperties = rootDevice.getInfo<CL_DEVICE_PARTITION_PROPERTIES>();
    const auto partitionable = std::any_of(partitionProperties.begin(), partitionProperties.end(),
                                            [](const decltype(partitionProperties)::value_type& prop) {
                                                return prop == CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN;
                                            });

    if (!partitionable) {
        return {};
    }

    const auto partitionAffinityDomain = rootDevice.getInfo<CL_DEVICE_PARTITION_AFFINITY_DOMAIN>();
    const decltype(partitionAffinityDomain) expectedFlags =
        CL_DEVICE_AFFINITY_DOMAIN_NUMA | CL_DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE;

    if ((partitionAffinityDomain & expectedFlags) != expectedFlags) {
        return {};
    }

    std::vector<cl::Device> subDevices;
    cl_device_partition_property partitionProperty[] = {CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN,
                                                        CL_DEVICE_AFFINITY_DOMAIN_NUMA, 0};

    rootDevice.createSubDevices(partitionProperty, &subDevices);

    return subDevices;
}

std::map<std::string, device_impl::ptr> ocl_builder::get_available_devices(void* user_context, void* user_device) const {
    bool host_out_of_order = true;  // Change to false, if debug requires in-order queue.
    std::vector<device_impl::ptr> dev_orig, dev_sorted;
    if (user_context != nullptr) {
        dev_orig = build_device_list_from_user_context(host_out_of_order, user_context);
    } else if (user_device != nullptr) {
        dev_orig = build_device_list_from_user_device(host_out_of_order, user_device);
    } else {
        dev_orig = build_device_list(host_out_of_order);
    }

    std::map<std::string, device_impl::ptr> ret;
    for (auto& dptr : dev_orig) {
        if (dptr->get_info().dev_type == cldnn::device_type::integrated_gpu)
            dev_sorted.insert(dev_sorted.begin(), dptr);
        else
            dev_sorted.push_back(dptr);
    }
    uint32_t idx = 0;
    for (auto& dptr : dev_sorted) {
        auto map_id = std::to_string(idx++);
        ret[map_id] = dptr;

        auto rootDevice = dptr->get_device();
        auto subDevices = getSubDevices(rootDevice);
        if (!subDevices.empty()) {
            uint32_t sub_idx = 0;
            for (auto& subdevice : subDevices) {
                auto subdPtr = device_impl::ptr(new device_impl(subdevice, cl::Context(subdevice),
                                                                dptr->get_platform(),
                                                                device_info_internal(subdevice)), false);
                ret[map_id+"."+std::to_string(sub_idx++)] = subdPtr;
            }
        }
    }
    return ret;
}

std::vector<device_impl::ptr> ocl_builder::build_device_list(bool out_out_order) const {
    cl_uint n = 0;
    // Get number of platforms availible
    cl_int err = clGetPlatformIDs(0, NULL, &n);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("[CLDNN ERROR]. clGetPlatformIDs error " + std::to_string(err));
    }

    // Get platform list
    std::vector<cl_platform_id> platform_ids(n);
    err = clGetPlatformIDs(n, platform_ids.data(), NULL);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("[CLDNN ERROR]. clGetPlatformIDs error " + std::to_string(err));
    }

    std::vector<device_impl::ptr> ret;
    for (auto& id : platform_ids) {
        cl::Platform platform = cl::Platform(id);

        if (platform.getInfo<CL_PLATFORM_VENDOR>() != INTEL_PLATFORM_VENDOR)
            continue;

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        for (auto& device : devices) {
            if (!does_device_match_config(out_out_order, device)) continue;
            ret.emplace_back(device_impl::ptr{ new device_impl(device, cl::Context(device),
                                                        id, device_info_internal(device)), false});
        }
    }
    if (ret.empty()) {
        throw std::runtime_error("[CLDNN ERROR]. No GPU device was found.");
    }
    return ret;
}

std::vector<device_impl::ptr>  ocl_builder::build_device_list_from_user_context(bool out_out_order, void* user_context) const {
    cl::Context ctx = cl::Context(static_cast<cl_context>(user_context), true);
    auto all_devices = ctx.getInfo<CL_CONTEXT_DEVICES>();

    std::vector<device_impl::ptr> ret;
    for (auto& device : all_devices) {
        if (!does_device_match_config(out_out_order, device)) continue;
        ret.emplace_back(device_impl::ptr{ new device_impl(device, cl::Context(device),
                                                        device.getInfo<CL_DEVICE_PLATFORM>(),
                                                        device_info_internal(device)), false});
    }

    if (ret.empty()) {
        throw std::runtime_error("[CLDNN ERROR]. User defined context does not have GPU device included!");
    }
    return ret;
}

std::vector<device_impl::ptr>  ocl_builder::build_device_list_from_user_device(bool out_out_order, void* user_device) const {
    cl_uint n = 0;
    // Get number of platforms availible
    cl_int err = clGetPlatformIDs(0, NULL, &n);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("[CLDNN ERROR]. clGetPlatformIDs error " + std::to_string(err));
    }

    // Get platform list
    std::vector<cl_platform_id> platform_ids(n);
    err = clGetPlatformIDs(n, platform_ids.data(), NULL);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("[CLDNN ERROR]. clGetPlatformIDs error " + std::to_string(err));
    }

    std::vector<device_impl::ptr> ret;
    for (auto& id : platform_ids) {
        cl::PlatformVA platform = cl::PlatformVA(id);

        if (platform.getInfo<CL_PLATFORM_VENDOR>() != INTEL_PLATFORM_VENDOR)
            continue;

        std::vector<cl::Device> devices;
#ifdef _WIN32
        platform.getDevices(CL_D3D11_DEVICE_KHR,
            user_device,
            CL_PREFERRED_DEVICES_FOR_D3D11_KHR,
#else
        platform.getDevices(CL_VA_API_DISPLAY_INTEL,
            user_device,
            CL_PREFERRED_DEVICES_FOR_VA_API_INTEL,
#endif
            &devices);

        for (auto& device : devices) {
            if (!does_device_match_config(out_out_order, device)) continue;
            cl_context_properties props[] = {
#ifdef _WIN32
                CL_CONTEXT_D3D11_DEVICE_KHR,
#else
                CL_CONTEXT_VA_API_DISPLAY_INTEL,
#endif
                (intptr_t)user_device,
                CL_CONTEXT_INTEROP_USER_SYNC, CL_FALSE,
                CL_CONTEXT_PLATFORM, (cl_context_properties)id,
                0 };
            ret.emplace_back(device_impl::ptr{ new device_impl(device, cl::Context(device, props),
                                                            id, device_info_internal(device)), false });
        }
    }
    if (ret.empty()) {
        throw std::runtime_error("[CLDNN ERROR]. No corresponding GPU device was found.");
    }
    return ret;
}

bool ocl_builder::does_device_match_config(bool out_of_order, const cl::Device& device) const {
    // Is it intel gpu
    if (device.getInfo<CL_DEVICE_TYPE>() != device_type ||
        device.getInfo<CL_DEVICE_VENDOR_ID>() != device_vendor) {
        return false;
    }

    // Does device support OOOQ?
    if (out_of_order) {
        auto queue_properties = device.getInfo<CL_DEVICE_QUEUE_PROPERTIES>();
        using cmp_t = std::common_type<decltype(queue_properties),
            typename std::underlying_type<cl::QueueProperties>::type>::type;
        if (!(static_cast<cmp_t>(queue_properties) & static_cast<cmp_t>(cl::QueueProperties::OutOfOrder))) {
            return false;
        }
    }

    return true;
}

}  // namespace gpu
}  // namespace cldnn
