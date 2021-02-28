#include <iostream>  
#include <fstream>  
#include "json/json.h"
#include <opencv2\opencv.hpp>  
#include <direct.h>
#include <io.h>
#include <stdio.h>

#include <dlib/opencv.h>  
#include <opencv2/opencv.hpp>  
#include <dlib/image_processing/frontal_face_detector.h>  
#include <dlib/image_processing/render_face_detections.h>  
#include <dlib/image_processing.h>  
#include <dlib/gui_widgets.h>  

using namespace dlib;
using namespace std;

long int string_to_int(string snum); //数字字符串转换成整数
string TCHAR2STRING(TCHAR *STR); //TCHAR类型转string类型
int video_cut(long int bg, long int ed, string filename, frontal_face_detector detector, shape_predictor pose_model); //视频剪切函数
int lip_process(); //唇语数据采集处理函数

string videopath; //视频打开路径
string videoname; //视频名
string jsonpath; //语音转写json数据打开路径
string savepath; //数据保存路径