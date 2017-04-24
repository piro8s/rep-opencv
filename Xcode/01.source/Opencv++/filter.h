//
//  filter.h
//  Opencv++
//
//  Created by 川副 泰広 on 2015/08/17.
//  Copyright (c) Yasuhiro Kawazoe 2015年 MytestApps. All rights reserved.
//////////////////////////////////////////////////////////////////

#include <opencv2/opencv.hpp>
#include "define.h"
// ノイズ除去
void deNoises(cv::Mat src, const int weight);
// 要素画像
cv::Mat setGray(const cv::Mat color, cv::Mat dst);
cv::Mat setEdge(const cv::Mat gray, cv::Mat dst);
cv::Mat makeScreenTone(const int width, const int height, cv::Mat dst);
cv::Mat unifQuant(const cv::Mat src, const int ksize, cv::Mat dst);
// 加工画像
cv::Mat anime(const cv::Mat edge, const cv::Mat LColor, cv::Mat dst);
cv::Mat paint(const cv::Mat LColor, cv::Mat dst, int ksize);
cv::Mat manga(const cv::Mat gray, const cv::Mat edge, const cv::Mat tone, cv::Mat dst);
cv::Mat colorExtra(const cv::Mat src, cv::Mat dst);
cv::Mat colorRef(const cv::Mat src, cv::Mat dst);
//////* filter.h 終 */////////////////////////////////////////////