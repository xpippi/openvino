# Copyright (C) 2018-2023 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

VPU_DEVICE_NAME = 'VPU'
MYRIAD_DEVICE_NAME = 'MYRIAD'
HDDL_DEVICE_NAME = 'HDDL'
FPGA_DEVICE_NAME = 'FPGA'
CPU_DEVICE_NAME = 'CPU'
GPU_DEVICE_NAME = 'GPU'
HETERO_DEVICE_NAME = 'HETERO'
MULTI_DEVICE_NAME = 'MULTI'
GNA_DEVICE_NAME = 'GNA'
UNKNOWN_DEVICE_TYPE = 'UNKNOWN'

XML_EXTENSION = '.xml'
BIN_EXTENSION = '.bin'
BLOB_EXTENSION = '.blob'

IMAGE_EXTENSIONS = ['JPEG', 'JPG', 'PNG', 'BMP']
BINARY_EXTENSIONS = ['BIN']

DEVICE_DURATION_IN_SECS = {
    CPU_DEVICE_NAME: 60,
    GPU_DEVICE_NAME: 60,
    VPU_DEVICE_NAME: 60,
    MYRIAD_DEVICE_NAME: 60,
    HDDL_DEVICE_NAME: 60,
    FPGA_DEVICE_NAME: 120,
    GNA_DEVICE_NAME: 60,
    UNKNOWN_DEVICE_TYPE: 120
}
