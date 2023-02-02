// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ocl_builder.h"

#include "kernels_cache.h"
#include "device_info.h"
#include "event_impl.h"
#include "configuration.h"

#include <memory>
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace cldnn {
typedef cl::vector<cl::vector<unsigned char>> kernels_binaries_vector;
typedef cl::vector<kernels_binaries_vector> kernels_binaries_container;
using queue_type = cl::CommandQueueIntel;
namespace gpu {
typedef CL_API_ENTRY cl_command_queue(CL_API_CALL* pfn_clCreateCommandQueueWithPropertiesINTEL)(
    cl_context context,
    cl_device_id device,
    const cl_queue_properties_khr* properties,
    cl_int* errcodeRet);

class gpu_toolkit;
class events_pool;

class gpu_queue {
public:
    const queue_type& queue() const { return _command_queue; }
    gpu_queue(uint32_t id, queue_type queue, std::shared_ptr<gpu_toolkit> context);
    gpu_queue(gpu_queue&& other)
        : id(other.id),
          _context(other._context),
          _command_queue(other._command_queue),
          _queue_counter(other._queue_counter.load()),
          _last_barrier(other._last_barrier.load()),
          _events_pool(std::move(other._events_pool)),
          _last_barrier_ev(other._last_barrier_ev),
          _output_event(other._output_event) {}

    gpu_queue& operator=(gpu_queue&& other) {
        if (this != &other) {
            id = other.id;
            _context = std::move(other._context);
            _command_queue = std::move(other._command_queue);
            _queue_counter = std::move(other._queue_counter.load());
            _last_barrier = std::move(other._last_barrier.load());
            _events_pool = std::move(std::move(other._events_pool));
            _last_barrier_ev = std::move(other._last_barrier_ev);
            _output_event = std::move(other._output_event);
        }
        return *this;
    }

    ~gpu_queue() = default;

    void sync_events(std::vector<event_impl::ptr> const& deps);
    void release_pending_memory();
    void flush();

    void set_output_event(bool out_event) { _output_event = out_event; }

    event_impl::ptr enqueue_kernel(kernels_cache::kernel_type const& kern,
                                   cl::NDRange const& global,
                                   cl::NDRange const& local,
                                   std::vector<event_impl::ptr> const& deps);
    event_impl::ptr enqueue_marker(std::vector<event_impl::ptr> const& deps);
    event_impl::ptr group_events(std::vector<event_impl::ptr> const& deps);
    void reset_events();
    event_impl::ptr create_user_event(bool set);
    void release_events_pool();
    std::shared_ptr<gpu_toolkit> context() { return _context.lock(); }

private:
    uint32_t id;
    std::weak_ptr<gpu_toolkit> _context;
    queue_type _command_queue;
    std::atomic<uint64_t> _queue_counter{0};
    std::atomic<uint64_t> _last_barrier{0};
    std::shared_ptr<events_pool> _events_pool;
    cl::Event _last_barrier_ev;
    bool _output_event = false;
};

}  // namespace gpu
}  // namespace cldnn
