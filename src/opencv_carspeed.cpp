#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <math.h>

#include "include/misc.h"


/* the function 'hough_tranformation' compute the hough transformation of the blob.
 * INPUT  -> blob : an object containing all the blob information: coordinates of points and size.
 * OUTPUT  -> lines : list of the (r,theta) couple more frequents.
 */
void hough_tranformation(Blob blob, std::vector<cv::Vec2f> &lines) {
	double precision = 100;
	int diagonal = 0;
	int mat_rows = int(M_PI * precision) + 1;

	int val = 0;
	int i_max  = 0;
	int r, angle;

	std::vector<cv::Point2i> points = blob.get_points();

	// find the most distant point from the origin.
	for(std::vector<cv::Point2i>::iterator point = points.begin(); point!=points.end();point++) {
		val = std::sqrt(std::pow((*point).x,2) + std::pow((*point).y,2)) + 1;
		if (val > diagonal)
			diagonal = val;
	}

	val = 0;

	// use the distance to create the vector of pair (r, theta).
	int *occs = (int *)(malloc(sizeof(int) * mat_rows * diagonal * 2));
	memset(occs, 1, sizeof(int) * mat_rows * diagonal * 2);

	// it calculates all the pairs (r, theta) and finds the most frequent one.
	for(std::vector<cv::Point2i>::iterator point = points.begin(); point!=points.end();point++) {
		for(int i=0;i<=180;i++) {
			r = int((*point).y * cos(i*M_PI/180.0) + (*point).x * sin(i*M_PI/180.0));
			angle = int(i*M_PI/180.0 * precision);

			val = mat_rows * (r + diagonal) + angle;
			occs[val]++;

			// update the most frequent pais.
			if (occs[val] > occs[i_max]) {
				i_max = val;
			}
		}	
	}

	lines.push_back(cv::Vec2f(double(i_max/mat_rows)-diagonal,i_max%mat_rows/precision));
	free(occs);
}

/* the function 'plot_line' draw the line with theta angle and distance r on the frame image.
 * INPUT  -> frame : the image on which to draw the line
 * INPUT  -> lines : list of (r,theta) couple.
 * INPUT  -> x_start : position in the original image wrt x.
 * INPUT  -> y_start : position in the original image wrt y.
 * OUTPUT  -> frame : the line is drawn on the input image
 */
void plot_line(cv::Mat frame, std::vector<cv::Vec2f> lines, int x_start, int y_start) {
	for(size_t i = 0; i < lines.size(); i++) {
		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000*(-b)) + x_start;
		pt1.y = cvRound(y0 + 1000*(a)) + y_start;
		pt2.x = cvRound(x0 - 1000*(-b)) + x_start;
		pt2.y = cvRound(y0 - 1000*(a)) + y_start;
		cv::line( frame, pt1, pt2, cv::Scalar(0,0,255), 3, cv::LINE_AA);
	}
}

int flag = 0;

/* the function 'compute_speed' given the angle of the line calculates the speed.
 * INPUT  -> value : the angle of the line.
 * OUTPUT  -> speed : the speed of the car.
 */
double compute_speed(double angle) {
	double speed;

	/* if to manage the change of angle when
	   the car goes to about 175 km/h.
	*/
	if (angle==0)
		flag = 180;
	else if (angle>3.10669)
		flag = 0;

	/* 1.4 is the proportion between the angle and the speed. This value is compute with speed/degree, 315/225
	 * where 315 is the maximum speed of the car and 225 si the degree of the speed indicator - 60 degree when the
	 * car is at 315 km/h (-60 since when the car starts the speed indicator is at 60 degree).
	 */
	speed = 1.4 * ((((180*angle)/M_PI)-60)+flag);
	return (speed < 0)? 0 : speed;
}

int main(int argc, char* argv[])
{
	int x_start = 120;
	int y_start = 270;
	int x_end = 330;
	int y_end = 240;

	int iter = 1;
	int n_frame = 0;

	cv::Mat frame;
	cv::Mat cropped_frame;
	cv::Mat out;

	cv::Rect crop(x_start, y_start, x_end, y_end);

	std::vector<Blob> blobs;

	std::ofstream myfile;
	
	myfile.open("data.csv");
	myfile << "speed,time,\n";

	// input video pointer.
	cv::VideoCapture cap;

	if(!cap.open("carspeed.mp4")) {
		std::cerr << "error in the opening of input file.";
		return -1;
	}

	// setup output video
	cv::VideoWriter output_cap("out_carspeed.mp4",
								cap.get(CV_CAP_PROP_FOURCC),
								cap.get(CV_CAP_PROP_FPS),
								cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH),
								cap.get(CV_CAP_PROP_FRAME_HEIGHT)));

	if (!output_cap.isOpened()) {
		std::cout << "error in the opening of output file." << std::endl;
		return -2;
	}
		
	n_frame = cap.get(CV_CAP_PROP_FPS);

	while(1) {
		cap >> frame;

		if (frame.rows==0) {
			myfile.close();
			return 0;
		}

		// isolate the pixels
		cropped_frame = frame.clone()(crop);
		out = cv::Mat(cropped_frame.rows,cropped_frame.cols,CV_8UC3);
		out = 0;
		
		// find the contours of the speed indicator.
		blobs = find_contours(cropped_frame,out,270,850);

		std::vector<cv::Vec2f> lines;

		if(blobs.size()>0){
			hough_tranformation(blobs[blobs.size()-1], lines);
			printf("angle: %f - time: %f - speed: %f\n",lines[0][1],iter * 1.0/n_frame,compute_speed(lines[0][1]));		
	 		myfile << compute_speed(lines[0][1]) << "," << iter * 1.0/n_frame << ",\n";
		}

		plot_line(frame,lines,x_start,y_start);
		cv::imshow("boundaries",out);
		cv::imshow("original",frame);

		output_cap.write(frame);

		int keypressed = cv::waitKey(1);
		if (keypressed == 'q')
			break;

		iter++;
	}

	return 0;
}