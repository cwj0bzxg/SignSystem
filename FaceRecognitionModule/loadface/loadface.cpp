#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <stdlib.h>

using namespace std;
using namespace cv;
using namespace cv::face;

void GetFileNames(string path,vector<string>& filenames)
{
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<endl;
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
            filenames.push_back(ptr->d_name);
    }
    }
    closedir(pDir);
}

int main(int argc, char** argv)
{
    if(argc != 2){
        cout<<" 你需要输入一个参数 \n"
            <<" ./loadface <path>\n"
            <<" <path> 图片目录(该目录中只能包含文件夹,且名称都只能为有数字组成的字符串)\n"
            <<" ( 存储训练好的模型的xml文件会存储到 ../../faces_models.xml ) "
            <<endl;
        return 2;
    }
    vector<string> student_dir;
    
    string filename = argv[1];

    string output_filename = "../../faces_model.xml";
    GetFileNames(filename, student_dir);  // 获取filename文件夹下的所有目录路径

    Ptr<LBPHFaceRecognizer> model = LBPHFaceRecognizer::create();  // 声明一个分类器
    
    // 遍历filename目录下的每个目录，即遍历每个学生的照片
    for(auto it = student_dir.cbegin(); it!=student_dir.cend();it++){
        vector<string> imageNames;
        int index = atoi((*it).c_str());

        string img_dir = filename + "/" + *it;
        GetFileNames(img_dir, imageNames);     // 获取当前目录中所有图片的图片名

        int imgNum = imageNames.size();
        vector<int> label;                 // 标签数组label，值全为index
        vector<Mat> images;                // 图片数组，值为当前文件夹中的图片

        for(int i=0;i<imgNum;i++){         // 遍历当前学生的所有图片，构造label和images数组

            label.push_back(index);
            images.push_back(imread(img_dir + "/" + imageNames[i],IMREAD_GRAYSCALE));
            cout<<"image: "<<img_dir<<"/"<<imageNames[i]<<" label: "<<index<<endl;  // 输出一些信息
        }
        model->update(images,label);       // 更新模型
    }
    model->write(output_filename);
    cout<<"模型保存至："<<output_filename<<endl;
    return 0;
}