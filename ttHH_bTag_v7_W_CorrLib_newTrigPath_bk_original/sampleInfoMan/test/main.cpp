#include "SampleInfoManager.h"
#include <iostream>

int main() {
    // CSV 파일은 기본적으로 "sampleConfig.csv" (test 디렉토리 내에 위치)
    // 만약 다른 파일명을 사용하고 싶으면 아래와 같이 설정할 수 있습니다.
    // SampleInfoManager::instance().setCsvFileName("csvInfo/mySampleConfig.csv");
    
    if (!SampleInfoManager::instance().loadFromCSV()) {
        std::cerr << "Failed to load CSV configuration from: "
                  << SampleInfoManager::instance().getCsvFileName() << std::endl;
        return 1;
    }

    // "ttJets" 샘플 정보를 가져와 출력합니다.
    const SampleInfo* info = SampleInfoManager::instance().getSampleInfo("ttJets");
    if (info != nullptr) {
        std::cout << "Sample: ttJets" << std::endl;
        std::cout << "Type: " << (info->isData ? "Data" : "MC") << std::endl;
        std::cout << "Weight: " << info->weight << std::endl;
    } else {
        std::cerr << "Sample 'ttJets' not found in CSV configuration." << std::endl;
        return 1;
    }

    return 0;
}
