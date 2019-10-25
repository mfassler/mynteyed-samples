// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//		 http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <iostream>
#include <functional>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "mynteyed/camera.h"
#include "mynteyed/utils.h"

#include "util/cam_utils.h"
#include "util/counter.h"
#include "util/cv_painter.h"

using namespace std;
using namespace MYNTEYE_NAMESPACE;



#include "jet_colormap.h"  // gives us: colormap[256][3]
cv::Vec3b depth_to_color(float d) {

	float dmin = 0.0;  // if d is here, then ii should be 255.0
	float dmax = 6000.0;  // if d is here, then ii should be 0.0

	float m = -255.0 / (dmax - dmin);
	float b = 255 - (m * dmin);

	float ii = m*d + b;

	int i = (int) ii;
	if (i < 0) {
		i = 0;
	}  else if (i > 255) {
		i = 255;
	}

	return cv::Vec3b(colormap[i][0], colormap[i][1], colormap[i][2]);
}




int main(int argc, char const* argv[]) {

	Camera cam;
	DeviceInfo dev_info;
	if (!util::select(cam, &dev_info)) {
		return 1;
	}
	util::print_stream_infos(cam, dev_info.index);

	cout << "Open device: " << dev_info.index << ", "
			<< dev_info.name << endl << endl;

	OpenParams params(dev_info.index);
	{
		// Framerate: 10(default), [0,60], [0,30](STREAM_2560x720)
		params.framerate = 30;

		// Color mode: raw(default), rectified
		// params.color_mode = ColorMode::COLOR_RECTIFIED;

		// Depth mode: colorful(default), gray, raw
		// Note: must set DEPTH_RAW to get raw depth values
		params.depth_mode = DepthMode::DEPTH_RAW;

		// Stream mode: left color only
		// params.stream_mode = StreamMode::STREAM_640x480;	// vga
		params.stream_mode = StreamMode::STREAM_1280x720;	// hd
		// Stream mode: left+right color
		// params.stream_mode = StreamMode::STREAM_1280x480;	// vga
		// params.stream_mode = StreamMode::STREAM_2560x720;	// hd

		// Auto-exposure: true(default), false
		// params.state_ae = false;

		// Auto-white balance: true(default), false
		// params.state_awb = false;

		// Infrared intensity: 0(default), [0,10]
		params.ir_intensity = 4;
	}

	cam.Open(params);

	cout << endl;
	if (!cam.IsOpened()) {
		cerr << "Error: Open camera failed" << endl;
		return 1;
	}
	cout << "Open device success" << endl << endl;

	cout << "Press ESC/Q on Windows to terminate" << endl;

	cv::namedWindow("color");
	cv::namedWindow("depth");


	CVPainter painter;
	util::Counter counter;
	for (;;) {
		cam.WaitForStream();
		counter.Update();

		auto image_color = cam.GetStreamData(ImageType::IMAGE_LEFT_COLOR);
		if (image_color.img) {
			cv::Mat color = image_color.img->To(ImageFormat::COLOR_BGR)->ToMat();
			painter.DrawSize(color, CVPainter::TOP_LEFT);
			painter.DrawStreamData(color, image_color, CVPainter::TOP_RIGHT);
			painter.DrawInformation(color, util::to_string(counter.fps()),
					CVPainter::BOTTOM_RIGHT);

			cv::imshow("color", color);
		}

		auto image_depth = cam.GetStreamData(ImageType::IMAGE_DEPTH);
		if (image_depth.img) {
			cv::Mat depth = image_depth.img->To(ImageFormat::DEPTH_RAW)->ToMat();

			//unsigned int *srcPtr;

			//cv::Mat jetMap(depth.rows, depth.cols, cv::CV_8UC3);
			cv::Mat jetMap(depth.rows, depth.cols, 16);

			//cv::applyColorMap(depth, jetMap, cv::COLORMAP_JET);
			for (int y=0; y<depth.rows; ++y) {
				//srcPtr = (unsigned int *)depth.ptr(y);

				for (int x=0; x<depth.cols; ++x) {
					//unsigned int val = srcPtr[x];
					unsigned int val = depth.at<uint16_t>(y, x);
					if (val < 1) {
						jetMap.at<cv::Vec3b>(y, x) = cv::Vec3b(128, 128, 128);
					} else {
						jetMap.at<cv::Vec3b>(y, x) = depth_to_color(val);
					}
				}
			}

			cv::imshow("depth", jetMap);
		}

		char key = static_cast<char>(cv::waitKey(1));
		if (key == 27 || key == 'q' || key == 'Q') {	// ESC/Q
			break;
		}
	}

	cam.Close();
	cv::destroyAllWindows();
	return 0;
}
