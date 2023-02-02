# Copyright (C) 2018-2023 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

from mo.front.extractor import FrontExtractorOp
from mo.ops.concat import Concat


class ConcatFrontExtractor(FrontExtractorOp):
    op = 'Concat'
    enabled = True

    @classmethod
    def extract(cls, node):
        attrs = {'N': node.pb.attr["N"].i, 'simple_concat': True}
        Concat.update_node_stat(node, attrs)
        return cls.enabled
