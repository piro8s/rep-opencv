//
//  main.cpp
//  Opencv++
//
//  Created by 川副 泰広 on 2015/08/17.
//  Copyright (c) Yasuhiro Kawazoe 2015年 MytestApps. All rights reserved.
//
//  動作: 画像加工->センサに反応->表示切り替え
//
//////////////////////////////////////////////////////////////////

#include "filter.h"
#include <iostream>
#include <chrono>

int main(int argc, char *argv[])
{
    /* 定数の宣言 */
    cv::Mat img_org, img_org_in;
    cv::Mat img_hsv;
    cv::Mat img_gray, img_edge, img_lowColor, img_screenTone;
    cv::Mat img_dst, img_edited;
    int num=0, counter=0;  // 表示画像番号
    
    /* 初期化：Start */
    /*  キャプチャ優先順位
     動作デバイス: MacBook Pro
        引数の大きさ : LeftUSB >> inSight >> RightUSB    
     */
    cv::VideoCapture cap1(0);
//    cv::VideoCapture cap2(1);
    // 成功したかどうかをチェック
    if(!cap1.isOpened()){ std::cout<<"Camera[0] cannot open!\n"<<std::endl; return -1; }
//    if(!cap2.isOpened()){ std::cout<<"Camera[1] cannot open!\n"<<std::endl; return -1; }
    
    // キャプチャの初期設定
    // USB-Cam
    cap1.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH_WINDOW);
    cap1.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT_WINDOW);
    // in-Cam
//    cap2.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH_WINDOW);
//    cap2.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT_WINDOW);
    
    /* 初期化：End */

    /* フレーム処理：Start */
    for(;;){
        /* カメラから新しいフレームを取得 */
        cap1 >> img_org;
//        cap2 >> img_org_in;

        /* 表示切り替えセンサ処理：Start */
//        const int thre_plt = WIDTH_WINDOW*HEIGHT_WINDOW*0.05;   // 画面の10％の領域で判断
//        const int threH_low = 58, threH_top = 118;   // ターゲットの色範囲(HSVのH)
//        const int threS = 60, threV = 60;   // ターゲット閾値のSとV
//        int flag=0;     // 0->1の時にウィンドウを切り替える
//        // ターゲットの画面占有量
//        cv::cvtColor(img_org_in, img_hsv, CV_RGB2HSV_FULL);
//        int plot=0;
//        for(int y=0; y<HEIGHT_WINDOW; y++){
//            for(int x=0; x<WIDTH_WINDOW; x++){
//                cv::Vec3b hsv = img_hsv.at<cv::Vec3b>(y, x);
//                if((threH_low<hsv[0])&&(hsv[0]<threH_top)&&(threS<hsv[1])&&(threV<hsv[2])){
//                    plot++;
//                }
//                else{
//                    hsv[2]=0;
//                }
//                img_hsv.at<cv::Vec3b>(y, x) = hsv;
//            }
//        }
//        
//        // センサの反応
//        if(plot<thre_plt){// 占有率未満ならリセット
//            flag=0;
//        }
//        else if(plot>=thre_plt){// 占有率以上で反応
//            flag++;
//        }
//        
//        // センサが初めて反応した時
//        if(flag==1){
//            counter++;      // 次の加工画像に移る
//            cv::destroyAllWindows();  // 前のウィンドウを消す
//        }

        int waitKey = cv::waitKey(1);
        if (waitKey==113) break;    // pressed "q"
        else if (waitKey==32) {     // pressed "space"
            counter++;                  // 次の加工画像に移る
            cv::destroyAllWindows();    // 前のウィンドウを消す
        }
        /* 表示切り替えセンサ処理：End */

        /* センサに反応して加工画像を表示：Start */
        num = counter%N;        // 表示画像番号
        
        switch (num) {
            default:
            case 0:
                img_gray = setGray(img_org, img_dst);
                img_edge = setEdge(img_gray, img_dst);
                img_lowColor  = unifQuant(img_org, 6, img_dst);// anime用　RGBを各5分割(5^3階調)
                img_edited = anime(img_edge, img_lowColor, img_dst);
                
                cv::imshow(name0, img_edited);
                cvMoveWindow(name0, 720-(WIDTH_WINDOW/2), 417-(HEIGHT_WINDOW/2));  // 画面の中央に移動
                break;
                
            case 1:
                img_lowColor = unifQuant(img_org,  4, img_dst);// paint用 RGBを各5分割(5^3階調)
                img_edited = paint(img_lowColor, img_dst, 5);
                
                cv::imshow(name1, img_edited);
                cvMoveWindow(name1, 720-(WIDTH_WINDOW/2), 417-(HEIGHT_WINDOW/2));
                break;
                
            case 2:
                img_gray = setGray(img_org, img_dst);
                img_edge = setEdge(img_gray, img_dst);
                img_screenTone = makeScreenTone(WIDTH_WINDOW, HEIGHT_WINDOW, img_dst);
                img_edited = manga(img_gray, img_edge, img_screenTone, img_dst);
                
                cv::imshow(name2, img_edited);
                cvMoveWindow(name2, 720-(WIDTH_WINDOW/2), 417-(HEIGHT_WINDOW/2));
                break;
                
            case 3:
                img_edited = colorExtra(img_org, img_dst);
                
                cv::imshow(name3, img_edited);
                cvMoveWindow(name3, 720-(WIDTH_WINDOW/2), 417-(HEIGHT_WINDOW/2));
                break;
                
            case 4:
                img_edited = colorRef(img_org, img_dst);
                
                cv::imshow(name4, img_edited);
                cvMoveWindow(name4, 720-(WIDTH_WINDOW/2), 417-(HEIGHT_WINDOW/2));
                break;
                
        }

        /* 画像データのリセット */
        img_org.release();
        img_org_in.release();
        img_hsv.release();
        img_gray.release();
        img_edge.release();
        img_lowColor.release();
        img_screenTone.release();
        img_dst.release();
        img_edited.release();
        img_org.release();

    }
    /* センサに反応して加工画像を表示：End */

    cv::destroyAllWindows();
    cap1.release();
//    cap2.release();
    
    return 0;
}
