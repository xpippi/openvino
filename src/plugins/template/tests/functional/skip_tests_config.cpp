// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "functional_test_utils/skip_tests_config.hpp"
#include "openvino/core/core_visibility.hpp"

#include <string>
#include <vector>

std::vector<std::string> disabledTestPatterns() {
    std::vector<std::string> retVector{
        R"(.*ExclusiveAsyncRequests.*)",
        R"(.*ReusableCPUStreamsExecutor.*)",
        R"(.*SplitLayerTest.*numSplits=30.*)",
        // CVS-51758
        R"(.*InferRequestPreprocessConversionTest.*oLT=(NHWC|NCHW).*)",
        R"(.*InferRequestPreprocessDynamicallyInSetBlobTest.*oPRC=0.*oLT=1.*)",
        // Not Implemented
        R"(.*(Multi|Auto|Hetero).*Behavior.*OVCompiledModelBaseTest.*(CheckExecGraphInfoBeforeExecution|CheckExecGraphInfoAfterExecution).*)",
        R"(.*(Multi|Auto|Hetero).*Behavior.*OVCompiledModelBaseTest.*(checkGetExecGraphInfoIsNotNullptr).*)",
        R"(.*OVClassExecutableNetworkGetMetricTest_EXEC_DEVICES.*CanGetExecutionDeviceInfo.*)",
        R"(.*OVClassHeteroExecutableNetworkGetMetricTest_SUPPORTED_CONFIG_KEYS.*GetMetricNoThrow.*)",
        R"(.*OVClassHeteroExecutableNetworkGetMetricTest_SUPPORTED_METRICS.*GetMetricNoThrow.*)",

        // TODO: Round with f16 is not supported
        R"(.*smoke_Hetero_BehaviorTests.*OVExecGraphImportExportTest.*readFromV10IR.*)",
        // TODO: support import / export of precisions in template plugin
        R"(.*smoke_Hetero_BehaviorTests.*OVExecGraphImportExportTest.ieImportExportedFunction.*)",
        R"(.*smoke_BehaviorTests.*OVExecGraphImportExportTest.ieImportExportedFunction.*)",
        // TODO: Round with f16 is not supported
        R"(.*smoke_Hetero_BehaviorTests.*OVExecGraphImportExportTest.*readFromV10IR.*)",

        R"(.*importExportedIENetworkParameterResultOnly.*elementType=(i8|u8).*)",
        R"(.*importExportedIENetworkParameterResultOnly.*elementType=(i16|u16).*)",
        R"(.*importExportedIENetworkParameterResultOnly.*elementType=(i64|u64).*)",
        R"(.*importExportedIENetworkParameterResultOnly.*elementType=u32.*)",
        R"(.*importExportedIENetworkConstantResultOnly.*elementType=(u32|u64).*)",

        // CVS-64094
        R"(.*ReferenceLogSoftmaxLayerTest.*4.*iType=f16.*axis=.*1.*)",
        // CVS-64012
        R"(.*ReferenceDeformableConvolutionLayerTest.*f16.*real_offset_padding_stride_dialation.*)",
        R"(.*ReferenceDeformableConvolutionLayerTest.*bf16.*)",
        R"(.*ReferenceDeformableConvolutionV8LayerTest.*f16.*real_offset_padding_stride_dialation.*)",
        R"(.*ReferenceDeformableConvolutionV8LayerTest.*bf16.*)",
        R"(.*ReferenceDeformableConvolutionV8LayerTest.*f64.*mask.*)",
        // CVS-63973
        R"(.*ReferencePSROIPoolingLayerTest.*bf16.*)",
        // CVS-63977
        R"(.*ReferenceProposalV1LayerTest.*f16.*)",
        // CVS-64082
        R"(.*ReferenceProposalV4LayerTest.*f16.*)",
        // CVS-64101
        R"(.*ReferenceExperimentalGPLayerTest.*bf16.*)",
        // CVS-64105
        R"(.*ReferenceGatherElementsTestNegative.*)",
        // CVS-64052
        R"(.*ReferenceStridedSliceLayerTest.*strided_slice_stride_optional_dynamic)",
        // CVS-64017
        R"(.*ReferenceGatherTest.*dType=i16.*)",
        R"(.*ReferenceGatherTest.*dType=u16.*)",
        R"(.*ReferenceGatherTest.*dType=bf16.*)",
        R"(.*ReferenceGatherTest.*dType=f64.*)",
        // CVS-64110
        R"(.*ReferenceGatherTestV7.*dType=i16.*)",
        R"(.*ReferenceGatherTestV7.*dType=u16.*)",
        R"(.*ReferenceGatherTestV7.*dType=bf16.*)",
        R"(.*ReferenceGatherTestV7.*dType=f64.*)",
        // CVS-64037
        R"(.*ReferencePadTest.*pad_exterior_2d_0x0)",
        R"(.*ReferencePadTest.*pad_exterior_2d_0x3)",
        R"(.*ReferencePadTest.*pad_exterior_2d_3x0)",
        // CVS-70975
        R"(.*ReferencePadTestParamsTooLarge.*)",
        // CVS-64006
        R"(.*ReferenceBatchToSpaceLayerTest.*dType=i4.*)",
        R"(.*ReferenceBatchToSpaceLayerTest.*dType=u4.*)",
        // CVS-64113
        R"(.*ReferenceRollLayerTest.*dType=i4.*)",
        R"(.*ReferenceRollLayerTest.*dType=u4.*)",
        // CVS-64050
        R"(.*ReferenceSpaceToBatchLayerTest.*dType=i4.*)",
        R"(.*ReferenceSpaceToBatchLayerTest.*dType=u4.*)",
        // CVS-64066
        R"(.*ReferenceGRUCellTestHardsigmoidActivationFunction.*gru_cell_hardsigmoid_activation_function)",
        // CVS-71381
        R"(.*ReferenceExpLayerTest.*u32.*)",
        R"(.*ReferenceExpLayerTest.*u64.*)",
        // CVS-64054
        R"(.*ReferenceTopKTest.*aType=i8.*)",
        R"(.*ReferenceTopKTest.*aType=i16.*)",
        R"(.*ReferenceTopKTest.*aType=u8.*)",
        R"(.*ReferenceTopKTest.*aType=u16.*)",
        R"(.*ReferenceTopKTest.*aType=bf16.*)",
        R"(.*ReferenceTopKTest.*aType=f64.*)",
        // CVS-63947
        R"(.*ReferenceConcatTest.*concat_zero_.*)",
        // CVS-64102
        R"(.*ReferenceExperimentalPGGLayerTest.*iType=bf16.*stride_x=(32|64).*)",
        // CVS-72215
        R"(.*ReferenceTileTest.*aType=i4.*)",
        R"(.*ReferenceTileTest.*aType=u4.*)",
        // CVS-71891
        R"(.*ReferenceTileTest.*rType=i4.*)",
        R"(.*ReferenceTileTest.*rType=u4.*)",
        // CVS-95608
        R"(.*CachingSupportCase.*CompileModelCacheTestBase.*)",
        // New plugin API doesn't support legacy NV12 I420 preprocessing
        R"(.*ConvertNV12WithLegacyTest.*)",
        R"(.*ConvertI420WithLegacyTest.*)",
        // Plugin version was changed to ov::Version
        R"(.*VersionTest.*pluginCurrentVersionIsCorrect.*)",
        // New plugin doesn't support dynamic preprocessing, here we set blob with changed layout
        R"(.*InferRequestPreprocessDynamicallyInSetBlobTest.*_iPRC=0.*_iLT=1.*)",
        // New plugin API doesn't support changes of pre-processing
        R"(.*InferRequestPreprocessTest.*SetPreProcessToInputInfo.*)",
        R"(.*InferRequestPreprocessTest.*SetPreProcessToInferRequest.*)",
        // New plugin work with tensors, so it means that blob in old API can have different pointers
        R"(.*InferRequestIOBBlobTest.*secondCallGetInputDoNotReAllocateData.*)",
        R"(.*InferRequestIOBBlobTest.*secondCallGetOutputDoNotReAllocateData.*)",
        R"(.*InferRequestIOBBlobTest.*secondCallGetInputAfterInferSync.*)",
        R"(.*InferRequestIOBBlobTest.*secondCallGetOutputAfterInferSync.*)",
        // Old API cannot deallocate tensor
        R"(.*InferRequestIOBBlobTest.*canProcessDeallocatedOutputBlobAfterGetAndSetBlob.*)",
        // Why query state should throw an exception
        R"(.*InferRequestQueryStateExceptionTest.*inferreq_smoke_QueryState_ExceptionTest.*)",
        R"(.*OVInferRequestCheckTensorPrecision.*get(Input|Output|Inputs|Outputs)From.*FunctionWith(Single|Several).*type=(u4|u1|i4|boolean).*)",
    };

#ifdef _WIN32
    // CVS-63989
    retVector.emplace_back(R"(.*ReferenceSigmoidLayerTest.*u64.*)");
    // CVS-64054
    retVector.emplace_back(R"(.*ReferenceTopKTest.*topk_max_sort_none)");
    retVector.emplace_back(R"(.*ReferenceTopKTest.*topk_min_sort_none)");
#endif

#if defined(OPENVINO_ARCH_ARM64) || defined(OPENVINO_ARCH_ARM)
    retVector.emplace_back(R"(.*smoke_TopK_With_Hardcoded_Refs/ReferenceTopKTestMaxMinSort.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_TopK_With_Hardcoded_Refs/ReferenceTopKTestBackend.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_TopK_With_Hardcoded_Refs/ReferenceTopKTestMaxMinSortV3.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_TopK_With_Hardcoded_Refs/ReferenceTopKTestBackendV3.CompareWithRefs.*)");
    // fails only on Linux arm64
    retVector.emplace_back(R"(.*ReferenceConversionLayerTest.CompareWithHardcodedRefs/conversionType=Convert_shape=[4]_iType=f16_oType=u4.*)");
    retVector.emplace_back(R"(.*ReferenceConversionLayerTest.CompareWithHardcodedRefs/conversionType=Convert_shape=[4]_iType=bf16_oType=u4.*)");
    retVector.emplace_back(R"(.*ReferenceConversionLayerTest.CompareWithHardcodedRefs/conversionType=Convert_shape=[4]_iType=f32_oType=u4.*)");
#endif
    return retVector;
}
