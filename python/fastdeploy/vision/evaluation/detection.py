# Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import numpy as np
import copy
import collections
import math


def eval_detection(model,
                   data_dir,
                   ann_file,
                   conf_threshold=None,
                   nms_threshold=None,
                   plot=False):
    from .utils import CocoDetection
    from .utils import COCOMetric
    import cv2
    from tqdm import trange
    import time

    if conf_threshold is not None or nms_threshold is not None:
        assert conf_threshold is not None and nms_threshold is not None, "The conf_threshold and nms_threshold should be setted at the same time"
        assert isinstance(conf_threshold, (
            float,
            int)), "The conf_threshold:{} need to be int or float".format(
                conf_threshold)
        assert isinstance(nms_threshold, (
            float,
            int)), "The nms_threshold:{} need to be int or float".format(
                nms_threshold)
    eval_dataset = CocoDetection(
        data_dir=data_dir, ann_file=ann_file, shuffle=False)
    all_image_info = eval_dataset.file_list
    image_num = eval_dataset.num_samples
    eval_dataset.data_fields = {
        'im_id', 'image_shape', 'image', 'gt_bbox', 'gt_class', 'is_crowd'
    }
    eval_metric = COCOMetric(
        coco_gt=copy.deepcopy(eval_dataset.coco_gt), classwise=False)
    scores = collections.OrderedDict()
    twenty_percent_image_num = math.ceil(image_num * 0.2)
    start_time = 0
    end_time = 0
    average_inference_time = 0
    for image_info, i in zip(all_image_info,
                             trange(
                                 image_num, desc="Inference Progress")):
        if i == twenty_percent_image_num:
            start_time = time.time()
        im = cv2.imread(image_info["image"])
        im_id = image_info["im_id"]
        if conf_threshold is None and nms_threshold is None:
            result = model.predict(im.copy())
        else:
            result = model.predict(im, conf_threshold, nms_threshold)
        pred = {
            'bbox':
            [[c] + [s] + b
             for b, s, c in zip(result.boxes, result.scores, result.label_ids)
             ],
            'bbox_num': len(result.boxes),
            'im_id': im_id
        }
        eval_metric.update(im_id, pred)
        if i == image_num - 1:
            end_time = time.time()
    average_inference_time = round(
        (end_time - start_time) / (image_num - twenty_percent_image_num), 4)
    eval_metric.accumulate()
    eval_details = eval_metric.details
    scores.update(eval_metric.get())
    scores.update({'average_inference_time(s)': average_inference_time})
    eval_metric.reset()
    return scores
