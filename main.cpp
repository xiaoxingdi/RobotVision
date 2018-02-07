#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
using namespace cv;
using namespace std;

 vector<Rect> redrects;
 vector<Rect> greenrects;
 vector<Rect> bluerects;
void  screenMVRect(vector<Rect> rects, Mat srcImg,int flag){//寻找最佳矩形
	for (int i = 0; i < rects.size(); i++){
		int y0 = rects[i].y;//上顶点坐标
		int y1 = rects[i].height + y0;//下顶点坐标
		double scale = double(rects[i].height) / rects[i].width;
		if (y0>5 && y1 < 475){

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


void drawRect(Mat srcImg){
	for (int i = 0; i < greenrects.size(); i++){
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
void getRect(Mat thresImg, Mat srcImg,int flag){
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
				screenMVRect(all_bound, srcImg,flag);
			}

			
		}

		
	}
	//imshow("绘制矩形", srcImg);


}
void screenRect(Mat srcImage,string str){
	morphologyEx(srcImage, srcImage, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(5, 5)));	//开操作降噪
	Mat redSrc = srcImage.clone();
	Mat blueSrc = srcImage.clone();
	Mat greenSrc = srcImage.clone();

	
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
	split(colorImg, channels);
	/*imshow("蓝色通道", channels[0]);
	imshow("绿色通道", channels[1]);
	imshow("红色通道", channels[2]);*/
	Mat redthres;
	Mat gred = channels[2].clone();
	threshold(channels[2], redthres, 29, 255, THRESH_BINARY);
	morphologyEx(redthres, redthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(3, 3)));	//开操作降噪
	morphologyEx(redthres, redthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(3, 3)));	//闭操作降噪
	//imshow("二值红色", redthres);
	getRect(redthres, redSrc,3);

	Mat bluethres;
	Mat gblue = channels[0].clone();
	threshold(channels[0], bluethres, 43, 255, THRESH_BINARY);
	morphologyEx(bluethres, bluethres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(5, 5)));	//开操作降噪
	morphologyEx(bluethres, bluethres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(5, 5)));	//闭操作降噪
	//imshow("二值蓝色", bluethres);
	getRect(bluethres, blueSrc,1);

	
	//Mat gbluethres;
	//threshold(gblue, gbluethres, 30, 255, THRESH_BINARY);
	//morphologyEx(gbluethres, gbluethres, CV_MOP_OPEN,
	//	getStructuringElement(MORPH_RECT, Size(10, 10)));	//开操作降噪
	//morphologyEx(gbluethres, gbluethres, CV_MOP_OPEN,
	//	getStructuringElement(MORPH_RECT, Size(10, 10)));	//闭操作降噪
	//imshow("二值绿蓝色", gbluethres);
	/*使用红色通道二值化的区域对背景筛选*/
	Mat gredthres;
	threshold(gred, gredthres, 9, 255, THRESH_BINARY);
	morphologyEx(gredthres, gredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//开操作降噪
	morphologyEx(gredthres, gredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//闭操作降噪
	//imshow("二值绿红色", gredthres);

	Mat opsgred;
	bitwise_not(gredthres, opsgred);//反二值
	
	//imshow("二值反绿红色", opsgred);
	Mat greenthres;
	threshold(channels[1], greenthres, 34, 255, THRESH_BINARY);
	morphologyEx(greenthres, greenthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10,10)));	//开操作降噪
	morphologyEx(greenthres, greenthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//闭操作降噪
	//imshow("二值绿色", greenthres);

	Mat finalgreen=opsgred.mul(greenthres);//红色与绿色点乘
	//imshow("二值最终绿色", finalgreen);
	getRect(finalgreen, greenSrc,2);

	drawRect(redSrc);
	string name = "G:/Robot/all/output2/jpg3/" + str+ ".jpg";
	imwrite(name, redSrc);
	/*string name2 = "G:/Robot/jpg/blue/" + str+ ".jpg";
	imwrite(name2, blueSrc);
	string name3 = "G:/Robot/jpg/green/" +str+ ".jpg";
	imwrite(name3, greenSrc);*/
	cout << "输出第" <<str << "张图片" << endl;
}



void main(){
	/*Mat srcImg1 = imread("G:/Robot/jpg/181021.jpg",CV_LOAD_IMAGE_COLOR);
	screenRect(srcImg1, "41");*/
	string name1 = "G:/Robot/jpg3/1836";
	string name2 = ".jpg";
	for (int i = 26; i < 52; i++){
		std::stringstream ss;
		std::string str;
		ss << i;
		ss >> str;
		string name = name1 + str + name2;
		Mat srcImg = imread(name, CV_LOAD_IMAGE_COLOR);
		screenRect(srcImg, str);
	}
	/*Mat srcImg2 = imread("G:/Robot/jpg/181020.jpg");
	Mat dst;
	subtract(srcImg2, srcImg1, dst);
	
	imshow("1", srcImg1);
	imshow("2", srcImg2);
	imshow("dst", dst);*/
	//useRGB(img);
	//EqualizeHistColorImage(srcImg1);
	waitKey(0);
}