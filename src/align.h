// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#pragma once

#include "core/processing.h"
#include "image.h"
#include "source.h"

namespace librealsense
{
    class synthetic_source : public synthetic_source_interface
    {
    public:
        synthetic_source(frame_source& actual)
            : _actual_source(actual), _c_wrapper(new rs2_source { this })
        {
        }

        rs2_frame* allocate_video_frame(rs2_stream stream, rs2_frame* original, 
                                        rs2_format new_format = RS2_FORMAT_ANY, 
                                        int new_bpp = 0,
                                        int new_width = 0, 
                                        int new_height = 0, 
                                        int new_stride = 0) override;

        rs2_frame* allocate_composite_frame(std::vector<frame_holder> frames) override;

        void frame_ready(frame_holder result) override;

        rs2_source* get_c_wrapper() override { return _c_wrapper.get(); }

    private:
        frame_source& _actual_source;
        std::shared_ptr<rs2_source> _c_wrapper;
    };

    class processing_block : public processing_block_interface
    {
    public:
        processing_block(std::shared_ptr<uvc::time_service> ts);

        void set_processing_callback(frame_processor_callback callback) override;
        void set_output_callback(frame_callback_ptr callback) override;
        void invoke(frame_holder frames) override;

        synthetic_source_interface& get_source() override { return _source_wrapper; }

    private:
        frame_source _source;
        std::mutex _mutex;
        frame_processor_callback _callback;
        synthetic_source _source_wrapper;
    };

    class pointcloud : public processing_block
    {
    public:
        pointcloud(std::shared_ptr<uvc::time_service> ts,
                   const rs2_intrinsics* depth_intrinsics = nullptr,
                   const float* depth_units = nullptr, 
                   const rs2_intrinsics* mapped_intrinsics = nullptr,
                   const rs2_extrinsics* extrinsics = nullptr);

    private:
        std::mutex              _mutex;

        const rs2_intrinsics*   _depth_intrinsics_ptr;
        const float*            _depth_units_ptr;
        const rs2_intrinsics*   _mapped_intrinsics_ptr;
        const rs2_extrinsics*   _extrinsics_ptr;

        rs2_intrinsics          _depth_intrinsics;
        rs2_intrinsics          _mapped_intrinsics;
        float                   _depth_units;
        rs2_extrinsics          _extrinsics;
    };
}