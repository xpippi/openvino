# Copyright (C) 2018-2023 Intel Corporation
# SPDX-License-Identifier: Apache-2.0
#

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    ie_add_compiler_flags(/wd4305)
elseif(OV_COMPILER_IS_CLANG)
    ie_add_compiler_flags(-Wno-delete-non-abstract-non-virtual-dtor)
endif()

ov_add_frontend(NAME paddle
                LINKABLE_FRONTEND
                PROTOBUF_LITE
                FILEDESCRIPTION "FrontEnd to load and convert PaddlePaddle file format"
                LINK_LIBRARIES openvino::util openvino::core::dev)
