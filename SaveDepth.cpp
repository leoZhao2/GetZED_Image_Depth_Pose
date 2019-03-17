#include "SaveDepth.hpp"

using namespace sl;
using namespace std;

int count_save = 0;

DEPTH_FORMAT Depth_format;
string temp;

void processKeyEvent(Camera& zed, char &key) {
    switch (key) {
        case 's': // Save side by side image
			saveSbSImage(zed,std::to_string(count_save) + std::string(".png"));
			saveDepth(zed, to_string(count_save));
			count_save++;
        break;
		case 'd': // Save Pose.txt
			savePose();
			break;
    }
    
}

void saveDepth(Camera& zed, std::string filename) {
    float max_value = std::numeric_limits<unsigned short int>::max();
    float scale_factor = max_value / zed.getDepthMaxRangeValue();

    std::cout << "Saving Depth Map... " << flush;
	Depth_format = static_cast<DEPTH_FORMAT> (2);
    bool saved = saveDepthAs(zed, Depth_format, filename.c_str(), scale_factor);
    if (saved)
        std::cout << "Done" << endl;
    else
       std::cout << "Failed... Please check that you have permissions to write on disk" << endl;
}

void saveSbSImage(Camera& zed, std::string filename) {
	char tr[200];
	char tl[200];
    Resolution image_size = zed.getResolution();
	Pose zed_pose;
	zed.getPosition(zed_pose, REFERENCE_FRAME_WORLD); 
	
	sprintf(tr,"%.5f %.5f %.5f ", zed_pose.getTranslation().tx,
		zed_pose.getTranslation().ty, zed_pose.getTranslation().tz, zed_pose.timestamp);
	sprintf(tl,"%.5f %.5f %.5f %.5f\n", zed_pose.getOrientation().ox,
		zed_pose.getOrientation().oy, zed_pose.getOrientation().oz, zed_pose.getOrientation().ow);
	printf("=====\n");
	string trr = tr;
	string tll = tl;
	temp += trr;
	temp += tll;
    cv::Mat sbs_image(image_size.height, image_size.width * 2, CV_8UC4);
    cv::Mat left_image(sbs_image, cv::Rect(0, 0, image_size.width, image_size.height));
    cv::Mat right_image(sbs_image, cv::Rect(image_size.width, 0, image_size.width, image_size.height));

    Mat buffer_sl;
    cv::Mat buffer_cv;

    zed.retrieveImage(buffer_sl, VIEW_LEFT);
    buffer_cv = cv::Mat(buffer_sl.getHeight(), buffer_sl.getWidth(), CV_8UC4, buffer_sl.getPtr<sl::uchar1>(MEM_CPU));
    buffer_cv.copyTo(left_image);
    zed.retrieveImage(buffer_sl, VIEW_RIGHT);
    buffer_cv = cv::Mat(buffer_sl.getHeight(), buffer_sl.getWidth(), CV_8UC4, buffer_sl.getPtr<sl::uchar1>(MEM_CPU));
    buffer_cv.copyTo(right_image);

    //cv::cvtColor(sbs_image, sbs_image, CV_RGBA2RGB);
	cv::cvtColor(left_image, left_image, CV_RGBA2RGB);
   // cv::imwrite(filename, sbs_image);
	cv::imwrite(filename, left_image);
}
void savePose()
{
	FILE *fp = fopen("./pose.txt", "w");
	const char *p = temp.data();
	fprintf(fp, p);
	fclose(fp);
	printf("Pose.txt saved!\n");
}