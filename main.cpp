/*������Ϊ����ʱ���������ͼƬ��д��������ͼƬ*/

#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
using namespace cv;
using namespace std;

 vector<Rect> redrects;
 vector<Rect> greenrects;
 vector<Rect> bluerects;
void  screenMVRect(vector<Rect> rects, int flag){//Ѱ����Ѿ��Σ���һ������
	for (int i = 0; i < rects.size(); i++){
		int y0 = rects[i].y;//�϶�������
		int y1 = rects[i].height + y0;//�¶�������
		double scale = double(rects[i].height) / rects[i].width;//����ı���
		if (y0>5 && y1 < 475){//����þ��β������±߽�Ӵ�������Ϊ��Ϊ����ԲͰ����������Գ���ı���Ҫ��Ϊ>1.6������Ҫ��>0.6,����ֻ��һ����Ͱ������

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


void drawRect(Mat srcImg){//���ƾ���
	for (int i = 0; i < greenrects.size(); i++){//������ɫ����ɫ�����ص������Զ���ɫ�е���ɫ����ȥ��
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
		CV_RETR_TREE, CV_CHAIN_APPROX_NONE);		//��������
	vector<Rect> all_bound;
	for (vector<vector<Point> >::iterator i = all_contours.begin();
		i != all_contours.end(); ++i) {
		Rect temp = boundingRect(Mat(*i));	//��������ȡ����
		if (temp.width / temp.height < 1.2){//����ȹ���
			if (temp.area()>3000){//�������
				all_bound.push_back(temp);
				//rectangle(srcImg, temp, Scalar(0, 255, 255), 2);
				screenMVRect(all_bound,flag);
			}
		
		}
	
	}

}
void screenRect(Mat srcImage,string str){
	morphologyEx(srcImage, srcImage, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(5, 5)));	//����������
	Mat redSrc = srcImage.clone();

	//��������������Ϊ��ɫ��������ͨ���Զ�ֵ���ĸ���
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
	split(colorImg, channels);//����ͨ����˳��ΪBGR

	Mat redthres;
	Mat gred = channels[2].clone();
	Mat bred = channels[2].clone();
	threshold(channels[2], redthres, 29, 255, THRESH_BINARY);//�Ժ�ɫ���ж�ֵ����Ϊɸѡ��ɫͰ
	morphologyEx(redthres, redthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(3, 3)));	//����������
	morphologyEx(redthres, redthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(3, 3)));	//�ղ�������
	getRect(redthres,3);

	/*ʹ�ú�ɫͨ����ֵ�����������ɫɸѡ*/
	Mat bluethres;
	threshold(channels[0], bluethres, 35, 255, THRESH_BINARY);
	morphologyEx(bluethres, bluethres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(5, 5)));	//����������
	morphologyEx(bluethres, bluethres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(5, 5)));	//�ղ�������

	Mat bredthres;//�����洢�˴�Ҫ����ɫ����ѡȡ�ĺ�ɫ�Ķ�ֵ�����
	threshold(gred, bredthres, 15, 255, THRESH_BINARY);
	morphologyEx(bredthres, bredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//����������
	morphologyEx(bredthres, bredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//�ղ�������


	Mat opsbred;
	bitwise_not(bredthres, opsbred);//����ɫ��ֵ����������ɫ���


	Mat finalblue = opsbred.mul(bluethres);//��ɫ����ɫ���
	morphologyEx(finalblue, finalblue, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//����������
	morphologyEx(finalblue, finalblue, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//�ղ�������
	getRect(finalblue,  1);
	/*ʹ�ú�ɫͨ����ֵ��������Ա���ɸѡ*/
	Mat gredthres;
	threshold(gred, gredthres, 9, 255, THRESH_BINARY);
	morphologyEx(gredthres, gredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//����������
	morphologyEx(gredthres, gredthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//�ղ�������

	Mat opsgred;
	bitwise_not(gredthres, opsgred);//����ֵ
	

	Mat greenthres;
	threshold(channels[1], greenthres, 34, 255, THRESH_BINARY);
	morphologyEx(greenthres, greenthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10,10)));	//����������
	morphologyEx(greenthres, greenthres, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//�ղ�������


	Mat finalgreen=opsgred.mul(greenthres);//��ɫ����ɫ���
	morphologyEx(finalgreen, finalgreen, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//����������
	morphologyEx(finalgreen, finalgreen, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//�ղ�������
	getRect(finalgreen,2);

	drawRect(redSrc);
	string name = "G:/Robot/all/output3/jpg1/" + str+ ".jpg";
	imwrite(name, redSrc);

	cout << "�����" <<str << "��ͼƬ" << endl;
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