#include <iostream>
#include <TString.h>
#include <TTree.h>

#include "EventLooper.hh"
#include "EventLooperWithCorrections.hh"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: ./exe_TrigStudy [SampleName] [UseCorrections (0 or 1)]" << std::endl;
        return 1;
    }

    TString sampleName = argv[1];
////    bool useEfficiencyDrawing = false;
    bool applySF = false;
    if (argc >= 3) {
       applySF = (std::stoi(argv[2]) != 0);
    }

    if (applySF) {
	std::cout<<"Step 02 mode, apply SF is TRUE"<<std::endl;
        // EventLooperWithCorrections 사용
        EventLooperWithCorrections* looper = new EventLooperWithCorrections();
        looper->setNtupleName(sampleName);
        looper->Init();
        looper->Loop();
        delete looper;
    } else {
        // 기존의 EventLooper 사용
	std::cout<<"Step 01 mode, get Trigger Eff"<<std::endl;
        EventLooper* looper = new EventLooper();
        looper->setNtupleName(sampleName);
        looper->Init();
        looper->Loop();
        delete looper;
    }

    return 0;
}

