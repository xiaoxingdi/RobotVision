# RobotVision

## 程序的设计思路
1.首先为什么会采用通道？  

  通过三个桶的颜色为红绿蓝，可分别对应各自的通道，分别提取RGB
  三色通道，并将其阈值化，得到一个二值图，在其中筛选出桶的位置。
  
2.与其他方法比较有什么优缺点？  

  在使用边缘检测方法，由于在边缘检测之后仍然要根据颜色特征对矩
  形进行筛选，并且筛选边缘也必较耗费时间。也可采用差分法进行位
  置确定，但需要对之前机器人的行走的要求比较高。
  
   
## 详细思路
（本程序所有参数均在当前环境下的选取，若环境改变，可能需要微调）
首先，由于白色在的RGB值为（255,255,255），即考虑到阈值时由于白
色跑道会干扰到RGB三色阈值的结果，所以在图片处理开始，将图片当
中RGB均大于30的像素点设置为全0，即黑色。

红色桶定位：将红色通道进行二值化，此处参数的选取由环境决定，二
值化之后进行开闭操作。由于红色较为稳定，所以目前仅此方法。

蓝色桶定位：在实验过程中，发现蓝色桶的定位会受到相机噪声而产生
一定影响，将非蓝桶区域也会标记出，所以首先进行红色筛选，此处红
色的阈值较低，是因为其他干扰区域带有较低的红色像素值，所以进行
二值化之后，蓝色桶部分为黑色，干扰区域为白色，将干扰区域和蓝色
区域就分离开了。然后对蓝色通道的二值化选取值会处理比实际低一点，
这样所得到的蓝色通道二值化的结果就会很好的兼容了噪声。将红色二
值化的结果进行反二值化操作，即白色变黑色，黑色变白色。最后将蓝
色与上面得到的红色结果进行点乘，得出最后的蓝色桶定位。

绿色桶定位：实验当中，发现绿色背景草坪会对绿色桶的选取产生了一
定的干扰，经过测试发现，草坪的R值要大于绿色桶的R值。利用此特点，
首先对红色通道进行二值化，然后将此二值化结果进行反二值化操作，
与绿色的二值化结果进行点乘运算，得到最终的绿色桶的定位。

如果你以为到这里就结束了，you are to young to simple
