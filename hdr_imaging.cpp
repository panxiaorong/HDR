#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>
//#include <string>

using namespace cv;
using namespace std;

void loadExposureSeq(String, vector<Mat>&, vector<float>&);

int main(int, char**argv)
{
    vector<Mat> images;
    vector<float> times;
	argv[1] = "./test1";  // ./Memorial_SourceImages   Images
	double cost, start, end;

	
    loadExposureSeq(argv[1], images, times);
	start = clock();
	
    Mat response;
    Ptr<CalibrateDebevec> calibrate = createCalibrateDebevec();
    calibrate->process(images, response, times);

    Mat hdr;
    Ptr<MergeDebevec> merge_debevec = createMergeDebevec();
    merge_debevec->process(images, hdr, times, response);

    Mat ldr;
    Ptr<TonemapDurand> tonemap = createTonemapDurand(2.2f);
    tonemap->process(hdr, ldr);
    
    Mat fusion;
    Ptr<MergeMertens> merge_mertens = createMergeMertens();
    merge_mertens->process(images, fusion);

	imshow("dst", fusion);
	//imshow("dstl", ldr);
	end = clock();
	cost = difftime(end, start);

	FILE *fp = fopen("result.txt", "a+");
	char sx[300];
	sprintf(sx, "%lf\n", cost);
	fwrite(&sx,strlen(sx),1,fp);
	fclose(fp);

	fusion = fusion * 255;
	//ldr = ldr * 255;
    imwrite("fusion.png", fusion);
    //imwrite("ldr.png", ldr);
    //imwrite("hdr.hdr", hdr);

	waitKey(0);
    return 0;
}

void loadExposureSeq(String path, vector<Mat>& images, vector<float>& times)
{
    path = path + std::string("/");
    ifstream list_file((path + "list-3.txt").c_str());
    string name;
    float val;
	int count = 0;
    while(list_file >> name >> val) {
        Mat img = imread(path + name);
		resize(img, img, Size(1920, 1080));
        images.push_back(img);
        times.push_back(1 / val);
		count++;
    }
    list_file.close();
}
