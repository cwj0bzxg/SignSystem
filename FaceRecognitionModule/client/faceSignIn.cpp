#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <time.h>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace cv;
using namespace cv::face;

boost::asio::io_service service;
boost::asio::ip::address_v4 add;
boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"),8001);

static string registerOntheServer(string className,string descript, int label){
    boost::asio::ip::tcp::socket sock(service);
    sock.connect(ep);

    string msg = to_string(label);  // 将三个参数转成json
    boost::property_tree::ptree pt_item;
    pt_item.put("className",className);
    pt_item.put("descript",descript);
    pt_item.put("label",msg);
    std::stringstream ss;
    boost::property_tree::write_json(ss,pt_item);
    string strContent = ss.str();
    cout<<" strContent:"<<strContent<<endl;
    
    boost::asio::streambuf b;   // 将json传给服务器
    std::ostream os(&b);
    os<<strContent;
    std::size_t n = sock.send(b.data());
    b.consume(n);

    boost::asio::streambuf a;      // 接收服务器返回的结果
    boost::asio::streambuf::mutable_buffers_type bufs = a.prepare(512);
    size_t m = sock.receive(bufs);
    a.commit(m);
    std::istream is(&a);
    std::string res;
    is >> res;

    sock.close();
    return res;
}

string getTime()
{
	struct tm t;   //tm结构指针
	time_t now;  //声明time_t类型变量
	time(&now);      //获取系统日期和时间
	localtime_r(&now,&t);   //获取当地日期和时间(linux)
    // localtime_s(&t, &now);  //window系统下使用这个函数
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",&t);
	return tmp;
}

int main(int argc, char** argv)
{
    if(argc != 2){
        cout<<" 您应该传入一个参数 "<<endl;
        cout<<" ./faceSignIn <classID>\n"
            <<endl;
        return 2;
    }
    string className = argv[1];
    string descript = getTime();  // 获取当前时间

    VideoCapture cap;       // 视频流
    cap.open(0);            // 打开0摄像头
    if(!cap.isOpened()){    // 判断是否正常打开
        cout<<"can't no open video stream <camera>"<<endl;
        return 2;
    }

    cout<<" 摄像头已打开 "<<endl;

    // 人脸分类器
    Ptr<LBPHFaceRecognizer> model = LBPHFaceRecognizer::create();
    model->read("../../faces_model.xml");  // 全部成员的脸部特征模型
    cout<<" 成功加载人脸分类器 "<<endl;
    // 人脸检测器
    CascadeClassifier cascade;
    cascade.load("../haarcascade_frontalface_default.xml");  // 采用opencv提供的人脸模型
    cout<<" 成功加载人脸检测器 "<<endl;

    Mat frame;  // 当前帧
    // int lastFaceNum=0;
    int currentFaceNum=0;

    time_t lasttime = time(0);
    time_t nowtime = time(0);

    vector<Rect> faces;  // 记录检测到的人脸

    string notice;       // 提示字符串
    // 用一个map存储近期检测过的label
    // 程序只对第一次被检测到的label请求服务器
    unordered_map<int,string> label2stringMap;

    cout<<" 开始检测 "<<endl;
    while(true)
    {
        cap>>frame;       // 读取一帧
        if (frame.empty()){
            cout << "Finished reading: empty frame" << endl;
            break;
        }
        // 检测当前帧中的人脸
        cascade.detectMultiScale( frame, faces,
            1.1, 2, 0
            //|CASCADE_FIND_BIGGEST_OBJECT
            //|CASCADE_DO_ROUGH_SEARCH
            |CASCADE_SCALE_IMAGE,
            Size(30, 30) );
        
        currentFaceNum=faces.size();  // 记录当前帧的人脸数量

        nowtime = time(0);            // 记录当前时间
        // 如果5秒内没有检测到人脸，且当前帧人脸数为0，这阻塞一段时间
        cout<<" time:"<<nowtime<<" 检测到人脸数："<<currentFaceNum<<endl;
        if(nowtime-lasttime>5&&currentFaceNum==0){
            cout<<" 近期未识别到人脸，进入待机状态 "<<endl;
            notice = "";
            waitKey(500);
            continue;
        }
        else{
            if(currentFaceNum!=0){    // 检测到人脸
                nowtime=lasttime=time(0);
                if(currentFaceNum>1){  // 当前帧有多张人脸
                    // 请确保画面中只有一张人脸
                    // 啥也不做
                }
                else if(currentFaceNum == 1){  // 当前帧只有一张人脸
                    int label = -1;
                    double confidence = -1;
                    Mat dst;
                    Rect& nr = faces[0];
                    Mat minimg(frame,Range(nr.y,nr.y+nr.height),Range(nr.x,nr.x+nr.width));
                    cvtColor(minimg,dst,COLOR_RGB2GRAY);  // 将图片转成灰度图

                    model->predict(dst,label,confidence);  // 预测

                    if(confidence>=0&&confidence<100){   // 设置阈值
                        if(label2stringMap.count(label)==0){  // 当前人脸未被标识过的话
                            // 向服务器返回信息
                            cout<<"正在向服务器发送请求"<<endl;
                            label2stringMap[label]=registerOntheServer(className, descript ,label);
                        }
                        string tmp = label2stringMap[label];
                        if(tmp!="NULL"){
                            notice = tmp + "Sign in successfully. "+ to_string(confidence);
                        }
                        else{
                            notice = "not a member of the class";
                        }
                        
                    }
                    else{
                        notice = "not a member of the class";
                    }
                }
                // 在人脸上画框并在图片左上角显示相应的notice
                // show the window
                {
                    ostringstream buf;
                    buf << notice;
                    putText(frame, buf.str(), Point(10, 30), FONT_HERSHEY_PLAIN, 2.0, Scalar(0, 0, 255), 2, LINE_AA);
                }
                for (vector<Rect>::iterator i = faces.begin(); i != faces.end(); ++i)
                {
                    Rect &r = *i;
                    rectangle(frame, r.tl(), r.br(), cv::Scalar(0, 255, 0), 2);
                }
                cout<<" notice: "<<notice<<endl;
                imshow("检测中",frame);
            }
            else{
                // 5秒内检测到过人脸
                imshow("检测中",frame);
            }
        }
        // 两帧之间至少相隔20ms
        waitKey(20);
    }
    return 0;
}