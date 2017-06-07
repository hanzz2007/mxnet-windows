
#include <stdio.h>

// Path for c_predict_api
#include <mxnet/c_predict_api.h>

#include <opencv2/opencv.hpp>

#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace cv;


// Read file to buffer
class BufferFile {
 public :
    std::string file_path_;
    int length_;
    char* buffer_;

    explicit BufferFile(std::string file_path)
    :file_path_(file_path) {

        std::ifstream ifs(file_path.c_str(), std::ios::in | std::ios::binary);
        if (!ifs) {
            std::cerr << "Can't open the file. Please check " << file_path << ". \n";
            assert(false);
        }

        ifs.seekg(0, std::ios::end);
        length_ = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
       // std::cout << file_path.c_str() << " ... "<< length_ << " bytes\n";

        buffer_ = new char[sizeof(char) * length_];
        ifs.read(buffer_, length_);
        ifs.close();
    }

    int GetLength() {
        return length_;
    }
    char* GetBuffer() {
        return buffer_;
    }

    ~BufferFile() {
        delete[] buffer_;
        buffer_ = NULL;
    }
};

void GetMeanFile(const cv::Mat im, mx_float* image_data){
                //const int channels, const cv::Size resize_size) {

    //int size = im.rows * im.cols * channels;

    mx_float* ptr_image = image_data;

    for (int i = 0; i < im.cols; i++) {
        for (int j = 0; j < im.rows; j++) {
            mx_float pix = im.at<unsigned char>(i,j);
            *ptr_image++ = pix/255;
        }
    }
}


void AttributeDetection(const Mat img, std::vector<float> &data_cp) {


    BufferFile json_data("lightened_moon_fuse-symbol.json");
    BufferFile param_data("lightened_moon_fuse-0082.params");

    // Parameters
    int dev_type = 1;  // 1: cpu, 2: gpu
    int dev_id = 0;  // arbitrary.
    mx_uint num_input_nodes = 1;  // 1 for feedforward
    const char* input_key[1] = {"data"};
    const char** input_keys = input_key;

    // Image size and channels
    int width = 128;
    int height = 128;
    int channels = 1;

    const mx_uint input_shape_indptr[2] = { 0, 4 };
    // ( trained_width, trained_height, channel, num)
    const mx_uint input_shape_data[4] = { 1,
                                        static_cast<mx_uint>(channels),
                                        static_cast<mx_uint>(width),
                                        static_cast<mx_uint>(height) };
    PredictorHandle out = 0;  // alias for void *

    //-- Create Predictor000
    MXPredCreate((const char*)json_data.GetBuffer(),
                 (const char*)param_data.GetBuffer(),
                 static_cast<size_t>(param_data.GetLength()),
                 dev_type,
                 dev_id,
                 num_input_nodes,
                 input_keys,
                 input_shape_indptr,
                 input_shape_data,
                 &out);

    // Just a big enough memory 1000x1000x3
    int image_size = width * height * channels;
    std::vector<mx_float> image_data = std::vector<mx_float>(image_size);
    
	
    GetMeanFile(img, image_data.data());//, channels, cv::Size(width, height));

    //-- Set Input Image
    MXPredSetInput(out, "data", image_data.data(), image_size);

    //-- Do Predict Forward
    MXPredForward(out);

    mx_uint output_index = 0;

    mx_uint *shape = 0;
    mx_uint shape_len;

    //-- Get Output Result
    MXPredGetOutputShape(out, output_index, &shape, &shape_len);
    size_t size = 1;
    for (mx_uint i = 0; i < shape_len; ++i) size *= shape[i];
    std::vector<float> data(size);
    MXPredGetOutput(out, output_index, &(data[0]), size);
    data_cp = data;
    // Release Predictor
    MXPredFree(out);
  
}




