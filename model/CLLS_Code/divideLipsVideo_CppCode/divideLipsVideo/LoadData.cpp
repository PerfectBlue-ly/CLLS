// LoadData.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCtest1.h"
#include "LoadData.h"
#include "afxdialogex.h"
#include "process.h"
//#include "ResultShow.h"
#include "MFCtest1Dlg.h"


// LoadData 对话框

IMPLEMENT_DYNAMIC(LoadData, CDialogEx)

LoadData::LoadData(CWnd* pParent /*=NULL*/)
	: CDialogEx(LoadData::IDD, pParent)
{

}

LoadData::~LoadData()
{
}

void LoadData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LoadData, CDialogEx)
	ON_BN_CLICKED(IDC_VIDEO_BUTTON1, &LoadData::OnBnClickedVideoButton1)
	ON_BN_CLICKED(IDC_JSON_BUTTON2, &LoadData::OnBnClickedJsonButton2)
	ON_BN_CLICKED(IDC_PROCESS_BUTTON4, &LoadData::OnBnClickedProcessButton4)
	ON_BN_CLICKED(IDC_PROCESS_BUTTON5, &LoadData::OnBnClickedProcessButton5)
	ON_BN_CLICKED(IDC_MENU1_BUTTON1, &LoadData::OnBnClickedMenu1Button1)
	ON_BN_CLICKED(IDC_EXIT1_BUTTON2, &LoadData::OnBnClickedExit1Button2)
END_MESSAGE_MAP()



// LoadData 消息处理程序


void LoadData::OnBnClickedVideoButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	// 设置过滤器   
	TCHAR szFilter1[] = _T("视频文件(*.mp4)|*.mp4|所有文件(*.*)|*.*||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("mp4"), NULL, 0, szFilter1, this);
	CString strFilePath1, fileName;

	// 显示打开文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		// 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里  
		strFilePath1 = fileDlg.GetPathName();
		//SetDlgItemText(IDC_VIDEO_EDIT1, strFilePath1);
		//string videopath1 = CT2A(strFilePath1.GetBuffer());
		videopath = CT2A(strFilePath1.GetBuffer());
		fileName = fileDlg.GetFileName();
		videoname = CT2A(fileName.GetBuffer());
		SetDlgItemText(IDC_VIDEO_EDIT1, fileName);
		TCHAR Result1[] = _T("视频加载成功！");
		SetDlgItemText(IDC_RESULT_EDIT2, Result1); //显示处理结果
	}
}


void LoadData::OnBnClickedJsonButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	// 设置过滤器  
	TCHAR szFilter2[] = _T("语音转写文件(*.json)|*.json|所有文件(*.*)|*.*||");
	// 构造打开文件对话框   
	CFileDialog fileDlg2(TRUE, _T("json"), NULL, 0, szFilter2, this);
	CString strFilePath2;

	// 显示打开文件对话框
	if (IDOK == fileDlg2.DoModal())
	{
		// 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里  
		strFilePath2 = fileDlg2.GetPathName();
		//SetDlgItemText(IDC_JSON_EDIT2, strFilePath2);
		jsonpath = CT2A(strFilePath2.GetBuffer());
		TCHAR Result1[] = _T("语音转写json数据加载成功！");
		SetDlgItemText(IDC_RESULT_EDIT2, Result1); //显示处理结果
	}
}


void LoadData::OnBnClickedProcessButton4()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileFind finder;
	//CString path;
	//BOOL fileExist;

	// 获取特定文件夹的LPITEMIDLIST，可以将之理解为HANDLE  
	// 所谓的特定文件夹,你可以用CSIDL_XXX来检索之。  
	LPITEMIDLIST rootLoation;
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &rootLoation);
	if (rootLoation == NULL) {
		// unkown error  
		return;
	}
	// 配置对话框  
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.pidlRoot = rootLoation; // 文件夹对话框之根目录，不指定的话则为我的电脑  
	bi.lpszTitle = _T("请选择保存数据的文件夹"); // 提示选择文件保存路径  
	// bi.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;  

	// 打开对话框, 有点像DoModal  
	LPITEMIDLIST targetLocation = SHBrowseForFolder(&bi);
	if (targetLocation != NULL) {
		TCHAR targetPath[MAX_PATH];
		SHGetPathFromIDList(targetLocation, targetPath);
		//MessageBox( targetPath );  
		savepath = TCHAR2STRING(targetPath);  //将路径存放到savepath字符串
		//SetDlgItemText(IDC_SAVEPATH_EDIT1, targetPath); //将路径显示
		TCHAR Result1[] = _T("数据保存路径获取成功！");
		SetDlgItemText(IDC_RESULT_EDIT2, Result1); //显示处理结果
	}
}


//json解析、视频剪切以及唇形提取


long int string_to_int(string snum) //string类型转长整型
{
	stringstream ss;//声明流
	ss << snum;    //字符串输出流
	long int num;
	ss >> num;    //输入到数字
	return num;
}

string TCHAR2STRING(TCHAR *STR) //TCHAR类型转string类型
{

	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL); //宽字节转多字节
	std::string str(chRtn);
	return str;
}

/*视频切割与唇形分割函数*/
int video_cut(long int bg, long int ed, string filename, frontal_face_detector detector, shape_predictor pose_model)
{
	if (bg == ed) return 0;//去掉json文件解析结果中的标点符号

	//【1】读入视频
	string tss;
	tss = videopath;
	cv::VideoCapture capture(videopath);
	cv::Mat inframe;//定义一个Mat变量，用于存储每一帧的图像
	if (!capture.isOpened()){
		return 0;
	}
	int rate = capture.get(CV_CAP_PROP_FPS);
	int delay = 1000 / rate;
	//cout << "帧率：" << rate << endl;
	capture.read(inframe);
	//capture >> inframe;  //读取当前帧

	//创建词语文件夹
	char pathdir[80];
	char str1[80];
	char cfilename[80];
	strcpy_s(cfilename,filename.c_str());
	int len = strlen(cfilename); //判断词语是单字还是双字或长词语
	int num = 1; //将同名的词语有序创建文件夹
	strcpy_s(str1, savepath.c_str());
	if (len < 4){
		while (true)
		{
			if (num < 10)
				sprintf_s(pathdir, "%s%s%s%s%d", str1, "\\单字\\", filename, "_100", num);
			else if (num < 100)
				sprintf_s(pathdir, "%s%s%s%s%d", str1, "\\单字\\", filename, "_10", num);
			else
				sprintf_s(pathdir, "%s%s%s%s%d", str1, "\\单字\\", filename, "_1", num);
			if (_access(pathdir, 0) == -1){
				//int ts = 1;
				//cout << "新创建文件夹：" << filename << endl;
				_mkdir(pathdir); //创建文件夹
				break;
			}
			num++;
		}
	}
	else if (len > 4)
	{
		while (true)
		{
			if (num < 10)
				sprintf_s(pathdir, "%s%s%s%s%d", str1, "\\长词语\\", filename, "_100", num);
			else if(num < 100)
				sprintf_s(pathdir, "%s%s%s%s%d", str1, "\\长词语\\", filename, "_10", num);
			else
				sprintf_s(pathdir, "%s%s%s%s%d", str1, "\\长词语\\", filename, "_1", num);
			if (_access(pathdir, 0) == -1){
				//int ts = 1;
				//cout << "新创建文件夹：" << filename << endl;
				_mkdir(pathdir); //创建文件夹
				break;
			}
			num++;
		}
	}
	else
	{
		while (true)
		{
			if (num < 10)
				sprintf_s(pathdir, "%s%s%s%s%d", str1, "\\双字\\", filename, "_100", num);
			else if (num < 100)
				sprintf_s(pathdir, "%s%s%s%s%d", str1, "\\双字\\", filename, "_10", num);
			else
				sprintf_s(pathdir, "%s%s%s%s%d", str1, "\\双字\\", filename, "_1", num);
			if (_access(pathdir, 0) == -1){
				//int ts = 1;
				//cout << "新创建文件夹：" << filename << endl;
				_mkdir(pathdir); //创建文件夹
				break;
			}
			num++;
		}
	}
	//sprintf_s(pathdir, "%s%s", "C:\\Users\\Jordan23\\Desktop\\DataSet\\", filename);
	//string path; path.c_str是将string转为char *

	//创建唇形文件夹
	sprintf_s(str1, "%s%s", pathdir, "\\lip_images");
	if (_access(str1, 0) == -1){
		//cout << "创建唇形文件夹成功！" << endl;
		_mkdir(str1); //创建文件夹
	}

	//保存对应剪切视频的说明信息
	char name1[80];
	sprintf_s(name1, "%s%s%s%s", pathdir, "\\", filename, ".txt");
	//string videoname = "test1_1.mp4";
	char contents[80];
	char str2[80];
	strcpy_s(str2, videoname.c_str());
	sprintf_s(contents, "%s%s%c%s%d%s%d%c%s%d", "视频源：", str2, '\n', "时间戳(ms)：", bg, "-", ed, '\n', "帧率(帧/s)：", rate);
	FILE *fp;
	errno_t err;
	if ((err = fopen_s(&fp, name1, "w")) == 0){
		fputs(contents, fp);
		//cout << "说明信息写入成功！" << endl;
		fclose(fp);
	}

	//保存剪切视频
	char name2[80];
	sprintf_s(name2, "%s%s%s%s", pathdir, "\\", filename, ".avi"); //输出到指定文件下
	cv::VideoWriter writer(name2, CV_FOURCC('D', 'I', 'V', 'X'), rate, cv::Size(inframe.cols, inframe.rows), true); //注意此处视频的尺寸大小要与真实的一致
	if (!writer.isOpened()){
		return 0;
	}

	//【2】循环显示每一帧
	int i = 0;
	int k = 0;
	cv::Mat fimg;
	char imagepath[80];//保存唇形图像路径
	while (capture.read(inframe))//若视频播放完成，退出循环
	{
		//capture >> inframe;  //读取当前帧
		//若视频剪切完成，退出循环
		if (i*delay > ed)
		{
			break;
		}
		i++;
		if (i*delay >= bg && i*delay <= ed)
		{
			//sprintf(name, "C:\\Users\\Jordan23\\Desktop\\DataSet\\data1_1\\pictures\\%d.jpg", i);//输出到指定文件夹下
			//imwrite(name, inframe);//输出一张jpg图片到工程目录下
			writer << inframe;
			//sprintf(name, "%d", i);
			//int ts = 1;
			//putText(frame, name, Point(0, 20), FONT_HERSHEY_SIMPLEX,0.6, Scalar(0, 255, 0));
			//imshow("读取视频", inframe);  //显示当前帧
			//cout << "processing image " << k << endl;

			//唇形提取
			cv_image<bgr_pixel> cimg(inframe);
			// Detect faces   
			std::vector<rectangle> face = detector(cimg);
			//截取人脸检测图像
			cv::Rect rect(face[0].left(), face[0].top(), face[0].width(), face[0].height());
			fimg = inframe(rect);

			// Detect faces
			cv_image<bgr_pixel> fcimg(fimg);
			std::vector<rectangle> faces = detector(fcimg);

			// Find the pose of each face.  
			std::vector<full_object_detection> shapes;
			for (unsigned long j = 0; j < faces.size(); ++j){
				shapes.push_back(pose_model(fcimg, faces[j]));
			}
			if (!shapes.empty()) {
				for (int j = 48; j < 68; j++) {
					circle(fimg, cvPoint(shapes[0].part(j).x(), shapes[0].part(j).y()), 2, cv::Scalar(0, 0, 255), -1);
					//putText(inframe, to_string(j), cvPoint(shapes[0].part(j).x(), shapes[0].part(j).y()), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0), 1, 4);
					//  shapes[0].part(j).x();//68个  
				}
			}
			sprintf(imagepath, "%s%s%d%s", str1, "\\lip_", k++, ".jpg");   //指定保存路径
			imwrite(imagepath, fimg);
			//Display it all on the screen
			//imshow("Dlib特征点", fimg);
			//cv::waitKey(delay);  //延时40ms
		}
		/*if (!capture.grab())
		{
		break;
		}*/
	}
	writer.release();
	//cout << "Done!" << endl;
	//system("pause");
	return 0;
}

/*唇语数据采集处理函数*/
int lip_proceess()
{
	Json::Reader reader;
	Json::Value root;// Json::Value是一种很重要的类型，可以代表任意类型(如int, double, string, object, array)
	//从文件中读取  
	ifstream is;
	is.open(jsonpath, ios::binary);
	if (reader.parse(is, root))
	{
		//定义并加载人脸检测模型
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor pose_model;
		deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

		//读取根节点array信息
		for (unsigned int k = 0; k < root.size(); k++){
			//读取object节点信息
			string bg = root[k]["bg"].asString();
			string ed = root[k]["ed"].asString();
			string nc = root[k]["nc"].asString();
			string content = root[k]["onebest"].asString();
			string si = root[k]["si"].asString();
			//cout << "-----句子" << si << "：" << content << endl;
			//cout << "时间范围：" << bg << "-" << ed << "ms" << endl;
			//cout << "句子置信度(范围为[0,1])： " << nc << endl;

			//读取数组信息  
			//cout << "-----分词结果：" << endl;
			for (unsigned int i = 0; i < root[k]["wordsResultList"].size(); i++)
			{
				string wc = root[k]["wordsResultList"][i]["wc"].asString();
				string wordBg = root[k]["wordsResultList"][i]["wordBg"].asString();
				string wordEd = root[k]["wordsResultList"][i]["wordEd"].asString();
				string wordsName = root[k]["wordsResultList"][i]["wordsName"].asString();
				//cout << "词语内容：" << wordsName << endl;
				//cout << "时间(1帧/10ms)[帧数范围]：" << wordBg << "-" << wordEd << endl;
				//调用视频剪切和唇形提取函数
				video_cut(10 * string_to_int(wordBg), 10 * string_to_int(wordEd), wordsName, detector, pose_model);
				//cout << "词置信度：" << wc << endl << endl;
			}
			//cout << endl << endl;
		}
		//cout << "Parsing complete!" << endl;
	}
	is.close();
	//getchar();
	return 0;
}



void LoadData::OnBnClickedProcessButton5()
{
	// TODO:  在此添加控件通知处理程序代码
	lip_proceess();
	TCHAR Result1[] = _T("数据采集完成！包括视频剪切、唇形提取和说明信息文件");
	SetDlgItemText(IDC_RESULT_EDIT2, Result1); //显示处理结果
	//LoadData::OnCancel();
	//ResultShow showDialog;
	//showDialog.DoModal();
}


void LoadData::OnBnClickedMenu1Button1()
{
	// TODO:  在此添加控件通知处理程序代码
	LoadData::OnCancel();
	CMFCtest1Dlg Menu;
	Menu.DoModal();
}


void LoadData::OnBnClickedExit1Button2()
{
	// TODO:  在此添加控件通知处理程序代码
	LoadData::OnCancel();
}
