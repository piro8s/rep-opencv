//
//  filter.cpp
//  Opencv++
//
//  Created by 川副 泰広 on 2015/08/17.
//  Copyright (c) Yasuhiro Kawazoe 2015年 MytestApps. All rights reserved.
/////////////////////////////////////////////////////////////////////////

#include "filter.h"

/* CONTENTS */
////////////////////////////////////////////////////////////////////
///* ノイズ除去 *///
// deNoises(InputImage, weight)
//
///* 要素画像 *///
// setGray(InputImage, OutputImage)             : グレースケール
// setEdge(InputImage, OutputImage)             : 輪郭強調
// makeScreenTone(width, height, OutputImage)   : スクリーントーンの作成
// unifQuant(InputImage, ksize, OutputImage)    : 均等量子化法による減色
//
///* 加工画像 *///
// anime(EdgeImage, LowColorImage, OutputImage)        : アニメ(イラスト)調
// paint(LowColorImage, OutputImane, ksize)            : 絵画調
// manga(GrayImage, EdgeImage, ScreenTone, OutputImane): マンガ調
// colorExtra(InputImage, OutputImage)                 : カラー抽出(肌色)
// colorRef(InputImage, OutputImage)                   : ネガポジ反転
////////////////////////////////////////////////////////////////////


//////* ノイズ除去 */////////////////////////////////
void deNoises(cv::Mat img_Src, const int weight)
{
    const int width = WIDTH_WINDOW;
    const int height = HEIGHT_WINDOW;
    const int AllPixels = 3;        // box=3x3
    // ^直すなら6x6, 9x9, 12x12...のボックスで処理
    
    std::vector<cv::Vec3b> vec(AllPixels);
    
    for(int y=0; y<(height-AllPixels); y+=AllPixels) {
        for(int x=0; x<width; x++){
            int counter = 0;
            for(int i=0; i<AllPixels; i++){
                // ピクセル値を格納
                vec[i] = img_Src.at<cv::Vec3b>(y+i, x);
                for(int ch=0; ch<3; ch++){
                    // ボックス内の黒ピクセル数をカウント
                    if(vec[i][ch]==0) counter++;
                }
            }
            // 黒ピクセル数がweight以下ならノイズを除去
            for(int i=0; i<AllPixels; i++){
                if((0<counter)&&(counter<=weight)){
                    for(int ch=0; ch<3; ch++){
                        vec[i][ch] = 255;
                    }
                }
                img_Src.at<cv::Vec3b>(y+i, x) = vec[i];
            }
        }
    }
}
//////* ノイズ除去 終 *//////////////////////////////

//////* 要素画 *////////////////////////////////////
///* グレースケール *///
cv::Mat setGray(const cv::Mat img_Src, cv::Mat img_Gray)
{
    cv::cvtColor(img_Src, img_Gray, CV_BGR2GRAY);
    return img_Gray;
}

///* 輪郭強調 *///
cv::Mat setEdge(const cv::Mat img_Gray, cv::Mat img_Edge)
{
    // エッジ検出
    const int BlockSize = 3;    // 1つのボックスの大きさ：値が小さいと線が細くなる
    const double ContThre = 7.0;// エッジの強さ：値が小さいと細く描写できる
    cv::adaptiveThreshold(img_Gray, img_Edge, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, BlockSize, ContThre);
    
    // ノイズ除去(ボックス内の黒ピクセル数がweight以上ならノイズ除去)
    const int weight = 1;
    deNoises(img_Edge, weight);
    
    return img_Edge;
}

///* 均等量子化法(uniform quantization)による減色 *///
// 色の空間分布に関係なく減色しているので、減色具合に課題
cv::Mat unifQuant(const cv::Mat img_Src, const int ksize, cv::Mat img_MCut)
{
    const int width = img_Src.cols;
    const int height = img_Src.rows;
    
    img_MCut = img_Src.clone();
    
    // RGB色空間の各ボックス領域にある色を、各中心の色に置換
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            cv::Vec3b color = img_Src.at<cv::Vec3b>(y, x);
            for(int i=0; i<ksize; i++){
                // ボックスの1辺の始点と終点
                int range_s = (256/ksize) * ( i );
                int range_e = (256/ksize) * (i+1);
                // 色の分布がそのボックス内なら重心に置換
                for(int ch=0; ch<3; ch++){
                    if((range_s<=color[ch])&&(color[ch]<range_e))
                        color[ch] = (range_s + range_e)/2;
                }
            }
            img_MCut.at<cv::Vec3b>(y, x) = color;
        }
    }

    return img_MCut;
}

///* スクリーントーン *///
// なくてもいいかもしれない...
cv::Mat makeScreenTone(const int width, const int height, cv::Mat img_Tone)
{
    const int line_interval = 5;
    const int line_thickness = 1;
    
    // 背景を白に初期化
    img_Tone = cv::Mat::ones(height, width, CV_8UC1)*255;
    // 斜めのラインを引く
    for(int i=0; line_interval*i<(width + height); i++){
        cv::line(img_Tone,
                 cv::Point(line_interval*i, 0),
                 cv::Point(0, line_interval*i),
                 0,line_thickness, CV_AA);
    }
    return img_Tone;
}
//////* 要素画 終 */////////////////////////////////

//////* エフェクト画 *//////////////////////////////
///* アニメ(イラスト)調 *///
cv::Mat anime(const cv::Mat img_Edge, const cv::Mat img_LowColor, cv::Mat img_Anime)
{
    // 輪郭をつける
    img_LowColor.copyTo(img_Anime, img_Edge);
    return img_Anime;
}

///* 絵画調 *///
cv::Mat paint(const cv::Mat img_LowColor, cv::Mat img_Paint, int median_box)
{
    // ぼかす
    cv::medianBlur(img_LowColor, img_Paint, median_box);
    return img_Paint;
}

///* マンガ調 *///
// 何値化するか要検討
cv::Mat manga(const cv::Mat img_Gray, const cv::Mat img_Edge, const cv::Mat img_Tone, cv::Mat img_Manga)
{
    const int width = WIDTH_WINDOW;
    const int height = HEIGHT_WINDOW;
    
    cv::Mat img_temp1, img_temp2, img_tone;

    img_temp1 = img_Gray.clone();
    img_tone = img_Tone.clone();
    
    // 4値化 + 軽めのトーンをあてる
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            cv::Vec3b vec = img_Gray.at<cv::Vec3b>(y, x);
            cv::Vec3b tone =img_Tone.at<cv::Vec3b>(y, x);
            int n = y%3;
            for(int ch=0; ch<3; ch++){
                if(vec[ch]< 63)
                    vec[ch] =   0;
                if(( 63<=vec[ch])&&(vec[ch]<128)){
                    vec[ch] =  85;
                    vec[n] = 127;   // 軽いトーン(\\\の向き)
                }
                if((128<=vec[ch])&&(vec[ch]<192)){
                    vec[ch] = 170;
                    vec[n] = 255;   // 軽いトーン(\\\の向き)
                }
                if(192<=vec[ch]){
                    vec[ch] = 255;
                    vec[2-n] = 205; // 軽いトーン(///の向き)
                    tone[ch] = 255; // 白色部のスクリーントーンの除去
                }
            }
            img_temp1.at<cv::Vec3b>(y, x) = vec;
            img_tone.at<cv::Vec3b>(y, x) = tone;
        }
    }
    // 輪郭をつけ、スクリーントーンを当てる
    img_temp1.copyTo(img_temp2, img_Edge);
    img_temp2.copyTo(img_Manga, img_tone);
   
    img_temp1.release();
    img_temp2.release();
    img_tone.release();
    return img_Manga;
}

///* カラー抽出(肌色) *///
// 閾値(抽出範囲)に課題
/******* 色範囲 ********
 USB-Cam(実験で用いたSANWAのWebCam) H:137~172 S:20~ V:60~
   ^目視より明るい(赤みが強い)・視野角狭い
 In-Cam(MacbookPro2012:720p FaceTimeHD-Cam)
        (太陽色の電球)H:167~243 S:30~60 V:30~155
        (白色の蛍光灯)H:125~175 S:30~60 V:30~155
   ^目視より青みが若干強い・明暗がくっきり・視野角は広い、照明に大きく左右
 ^いずれも背景が白っぽいと精度が落ちる
 *********************/
cv::Mat colorExtra(const cv::Mat img_Src, cv::Mat img_ColorEx)
{
    const int low1 = 132, top1 = 172;   // H:0~255
    const int low2 =  20, top2 = 255;   // S:0~255
    const int low3 =  60, top3 = 255;   // V:0~255
    cv::Scalar low(low1, low2, low3);
    cv::Scalar top(top1, top2, top3);
    
    cv::Mat img_hsv, img_mask;
    
    cv::cvtColor(img_Src, img_hsv, CV_RGB2HSV_FULL);
    
    // maskを作る : low~topの範囲外を黒く塗りつぶす
    cv::inRange(img_hsv, low, top, img_mask);
    // maskのちらつきを少なくする
    cv::GaussianBlur(img_mask, img_mask, cv::Size(1, 1), 1.0, 1.0);
    // srcにmaskを重ねる
    img_Src.copyTo(img_ColorEx, img_mask);
    
    img_hsv.release();
    img_mask.release();
    return img_ColorEx;
}

///* ネガポジ反転 *///
cv::Mat colorRef(const cv::Mat img_Src, cv::Mat img_ColorRef)
{
    img_ColorRef = ~img_Src;    // ビット否定
    return img_ColorRef;
}

//////* エフェクト画 終 *///////////////////////////
//////////* filter.cpp 終 */////////////////////////////////////////////













