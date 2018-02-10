/*本程序为测验时导入待处理图片和写出处理结果图片*/

#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
using namespace cv;
using namespace std;

 vector<Rect> redrects;
 vector<Rect> greenrects;
 vector<Rect> bluerects;
void  screenMVRect(vector<Rect> rects, int flag){//寻找最佳矩形，进一步过滤
	for (int i = 0; i < rects.size(); i++){
		int y0 = rects[i].y;//上顶点坐标
		int y1 = rects[i].height + y0;//下顶点坐标
		double scale = double(rects[i].height) / rects[i].width;//长宽的比例
		if (y0>5 && y1 < 475){//如果该矩形不与上下边界接触，则视为它为整个圆桶的轮廓，则对长宽的比例要求为>1.6，否则要求>0.6,考虑只有一部分桶的区域

			if (scale>1.6){
				if (flag == 3){
					redrects.push_back(rects[i]);
				}
				else if (flag == 2){
					greenrects.push_back(rects[i]);
				}
				else{
					bluerects.push_back(rects[i]);
				}
				//rectangle(srcImg, rects[i], Scalar(0, 255, 255), 2);
			}
		}
		else{
			if (scale > 0.6){
			//	rectangle(srcImg, rects[i], Scalar(0, 255, 255), 2);
				if (flag == 3){
					redrects.push_back(rects[i]);
				}
				else if (flag == 2){
					greenrects.push_back(rects[i]);
				}
				else{
					bluerects.push_back(rects[i]);
				}
			}
		}
	}
}


void drawRect(Mat srcImg){//绘制矩形
	for (int i = 0; i < greenrects.size(); i++){//由于绿色和蓝色会有重叠，所以对蓝色中的绿色进行去除
		for (int j = 0; j < bluerects.size(); j++){
			Rect rect1 = greenrects[i];
			Rect rect2 = bluerects[j];
			if (rect2.x<rect1.x){
				if (abs(rect2.x - rect1.x)<rect2.width){
					bluerects.erase(remove(bluerects.begin(), bluerects.end(), bluerects[i]), bluerects.end());

				}

			}
			else{
				if (abs(rect1.x - rect2.x)<rect1.width){
					bluerects.erase(remove(bluerects.begin(), bluerects.end(), bluerects[i]), bluerects.end());
				}

			}
		}

	}
	
	for (int i = 0; i < greenrects.size(); i++){
		rectangle(srcImg, greenrects[i], Scalar(0, 255, 255), 2);
	}
	for (int i = 0; i < bluerects.size(); i++){
		rectangle(srcImg, bluerects[i], Scalar(0, 255, 255), 2);
	}
	for (int i = 0; i < redrects.size(); i++){
		rectangle(srcImg, redrects[i], Scalar(0, 255, 255), 2);
	}
	greenrects.clear();
	redrects.clear();
	bluerects.clear();
}
void getRect(Mat thresImg,int flag){
	vector<vector<Point> > all_contours;
	vector<Vec4i> hierarchy;
	Mat contours_image = thresImg.clone();
	findContours(contours_image, all_contours, hierarchy,
		CV_RETR_TREE, CV_CHAIN_APPROX_NONE);		//查找轮廓
	vector<Rect> all_bound;
	for (vector<vector<Point> >::iterator i = all_contours.begin();
		i != all_contours.end(); ++i) {
		Rect temp = boundingRect(Mat(*i));	//从轮廓获取矩形
		if (temp.width / temp.height < 1.2){//长宽比过滤
			if (temp.area()>3000){//面积过滤
				all_bound.push_back(temp);
				//rectangle(srcImg, temp, Scalar(0, 255, 255), 2);
				screenMVRect(all_bound,flag);
			}
		
		}
	
	}

}
void screenRect(Mat srcImage,string str){
	morphologyEx(srcImage, srcImage, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(5, 5)));	//开操作降噪
	Mat redSrc = srcImage.clone();

	//将白线区域设置为黑色，减少三通道对二值化的干扰
	for (int x = 0; x < srcImage.cols; x++){
	for (int y = 0; y < srcImage.rows; y++){

			int b = srcImage.at<Vec3b>(y, x)[0];
			int g = srcImage.at<Vec3b>(y, x)[1];
			int r = srcImage.at<Vec3b>(y, x)[2];
			if (b>30 && g>30 && r > 30)
				srcImage.at<Vec3b>(y, x) = 0;
		}
	}
	Mat colorImg = srcImage.clone();
	vector<Mat> channels(3);
	split(colorImg, channels);//分离通道，顺序为BGR

	Mat redthres;
	Mat gred = channels[2].clone();
	Mat bred = channels[2].clone();
	threshold(channels[2], redthres, 29, 255, THRESH_BINARY);//对红色进行二值化，为筛选红色桶
	morphologyEx(redthres, redthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(3, 3)));	//开操作降噪
	morphologyEx(redthres, redthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(3, 3)));	//闭操作降噪
	getRect(redthres,3);

	/*使用红色通道二值化的区域对蓝色筛选*/
	Mat bluethres;
	threshold(channels[0], bluethres, 35, 255, THRESH_BINARY);
	morphologyEx(bluethres, bluethres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(5, 5)));	//开操作降噪
	morphologyEx(bluethres, bluethres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(5, 5)));	//闭操作降噪

	Mat bredthres;//用来存储此处要对蓝色进行选取的红色的二值化结果
	threshold(gred, bredthres, 15, 255, THRESH_BINARY);
	morphologyEx(bredthres, bredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//开操作降噪
	morphologyEx(bredthres, bredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//闭操作降噪


	Mat opsbred;
	bitwise_not(bredthres, opsbred);//反红色二值，用来与蓝色点乘


	Mat finalblue = opsbred.mul(bluethres);//红色与蓝色点乘
	morphologyEx(finalblue, finalblue, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//开操作降噪
	morphologyEx(finalblue, finalblue, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//闭操作降噪
	getRect(finalblue,  1);
	/*使用红色通道二值化的区域对背景筛选*/
	Mat gredthres;
	threshold(gred, gredthres, 9, 255, THRESH_BINARY);
	morphologyEx(gredthres, gredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//开操作降噪
	morphologyEx(gredthres, gredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//闭操作降噪

	Mat opsgred;
	bitwise_not(gredthres, opsgred);//反二值
	

	Mat greenthres;
	threshold(channels[1], greenthres, 34, 255, THRESH_BINARY);
	morphologyEx(greenthres, greenthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10,10)));	//开操作降噪
	morphologyEx(greenthres, greenthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//闭操作降噪


	Mat finalgreen=opsgred.mul(greenthres);//红色与绿色点乘
	morphologyEx(finalgreen, finalgreen, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//开操作降噪
	morphologyEx(finalgreen, finalgreen, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//闭操作降噪
	getRect(finalgreen,2);

	drawRect(redSrc);
	string name = "G:/Robot/all/output3/jpg1/" + str+ ".jpg";
	imwrite(name, redSrc);

	cout << "输出第" <<str << "张图片" << endl;
}



void main(){
	
	string name1 = "G:/Robot/jpg/1810";
	string name2 = ".jpg";
	for (int i = 19; i < 44; i++){
		std::stringstream ss;
		std::string str;
		ss << i;
		ss >> str;
		string name = name1 + str + name2;
		Mat srcImg = imread(name, CV_LOAD_IMAGE_COLOR);
		screenRect(srcImg, str);
	}

	waitKey(0);
}