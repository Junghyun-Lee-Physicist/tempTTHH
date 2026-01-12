
# SampleInfoManager 라이브러리 및 Trigger Efficiency/SF 적용 코드 사용법

## 1. 개요
이 라이브러리는 CSV 파일을 읽어 샘플별로 **Sample Name**, **Data/MC 여부**, **weight** 정보를 중앙에서 관리합니다.  
CSV 파일은 다음과 같은 형식으로 작성합니다:

SampleName : Data or MC, weight

ttJets      : MC   , 3.4410
Data_EraB   : Data , 1.0

- **샘플 타입**은 반드시 `Data` 또는 `MC`여야 하며, 이외의 값이면 에러를 출력하고 프로그램을 종료합니다.
- **weight** 항목은 필수입니다.

## 2. CSV 파일 위치
CSV 파일은 기본적으로 실행 파일과 같은 위치의 `sampleConfig.csv`를 사용합니다.  
필요한 경우, 라이브러리 사용 시 아래와 같이 CSV 파일명을 변경할 수 있습니다:
```cpp
SampleInfoManager::instance().setCsvFileName("csvInfo/sampleConfig.csv");

이 경우 CSV 파일은 지정한 위치(예: 프로젝트 내 csvInfo 디렉토리)에 두면 됩니다.

3. 디렉토리 구조 예시

project/
├── include/
│   └── SampleInfoManager.h
├── src/
│   ├── SampleInfoManager.cpp
│   └── EventLooperWithCorrections.cpp
├── csvInfo/        (원하는 경우 CSV 파일을 이곳에 저장)
│   └── sampleConfig.csv
├── lib/            (Makefile 빌드시 자동 생성)
├── obj/            (빌드시 생성되며, 최종 빌드시 삭제)
├── Makefile
└── main.cpp

4. 빌드 방법

4.1 Makefile을 이용한 빌드

터미널에서 프로젝트 루트 디렉토리로 이동 후 다음 명령어를 실행합니다:

make

그러면 lib/libSampleInfoManager.a 정적 라이브러리가 생성되며, 빌드 후 중간 오브젝트 파일은 삭제되어 용량을 줄입니다.

라이브러리를 깨끗하게 삭제하려면:

make clean

4.2 g++ 명령줄에서 컴파일하는 방법

예를 들어, trigger.cpp 파일에서 이 라이브러리를 사용하는 경우:

g++ -I/path/to/project/include trigger.cpp -L/path/to/project/lib -lSampleInfoManager -o trigger

옵션 설명:
	•	-I/path/to/project/include: 헤더 파일 위치 지정
	•	-L/path/to/project/lib: 정적 라이브러리 파일 위치 지정
	•	-lSampleInfoManager: libSampleInfoManager.a 라이브러리를 링크 (앞의 lib와 확장자 .a는 생략)

4.3 CMake를 이용한 빌드

예시 CMakeLists.txt:

cmake_minimum_required(VERSION 3.10)
project(TriggerApp)

set(CMAKE_CXX_STANDARD 11)

# 헤더 파일 경로 설정
include_directories("/path/to/project/include")

# 실행 파일 생성
add_executable(trigger trigger.cpp)

# 정적 라이브러리 링크 (전체 경로 지정)
target_link_libraries(trigger "/path/to/project/lib/libSampleInfoManager.a")

빌드 방법:

mkdir build && cd build
cmake ..
make

5. 라이브러리 사용 방법

다른 코드(예, trigger.cpp 또는 보정 코드)에서는 아래와 같이 사용합니다:

#include "EventLooperWithCorrections.hh"
#include "SampleInfoManager.h"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: ./exe_TrigStudy [SampleName]" << std::endl;
        return 1;
    }
    TString sampleName = argv[1];

    // 기본 CSV 파일은 "sampleConfig.csv" (실행 파일과 같은 위치)
    // 필요시 setCsvFileName()으로 경로를 변경할 수 있음
    if (!SampleInfoManager::instance().loadFromCSV()) {
        std::cerr << "Failed to load sample configuration from " 
                  << SampleInfoManager::instance().getCsvFileName() << std::endl;
        return 1;
    }

    // CSV에서 sampleName과 일치하는 설정 정보를 가져옴
    const SampleInfo* config = SampleInfoManager::instance().getSampleInfo(sampleName.Data());
    if (config == nullptr) {
        std::cerr << "No configuration found for sample: " << sampleName << std::endl;
        return 1;
    }

    // EventLooperWithCorrections를 사용하여 trigger efficiency 및 SF 계산
    EventLooperWithCorrections* looper = new EventLooperWithCorrections();
    looper->setNtupleName(sampleName);
    looper->Init();
    looper->setSampleConfig(*config);
    looper->Loop();
    delete looper;

    return 0;
}

6. Trigger Efficiency 및 Scale Factor 저장

Trigger efficiency 및 scale factor는 기존과 같이 ROOT 파일에 2D 히스토그램 (예: number of b-jet vs HT) 형태로 저장됩니다.

7. 환경 변수 설정 (동적 라이브러리 사용 시)

동적 라이브러리를 사용할 경우, 실행 전에 아래 스크립트를 실행하여 환경 변수 LD_LIBRARY_PATH (Linux) 또는 DYLD_LIBRARY_PATH (macOS)를 설정합니다.

#!/bin/bash
# setenv.sh 예시: 라이브러리 경로를 설정합니다.
export LD_LIBRARY_PATH="/path/to/project/lib:$LD_LIBRARY_PATH"
echo "LD_LIBRARY_PATH set to: $LD_LIBRARY_PATH"

실행 전에 다음 명령어로 실행합니다:

source setenv.sh

8. 요약
	•	CSV 파일은 Sample Name, Data/MC 여부, weight 만 저장합니다.
	•	CSV 파일명은 기본적으로 실행 파일과 같은 위치의 sampleConfig.csv를 사용하며, 필요시 setCsvFileName()으로 변경할 수 있습니다.
	•	Trigger efficiency 및 scale factor는 ROOT 파일에 2D 히스토그램으로 저장합니다.
	•	빌드 및 링크는 Makefile, g++, 또는 CMake를 통해 수행합니다.
	•	다른 코드에서는 #include "SampleInfoManager.h" 및 EventLooperWithCorrections.hh를 통해 중앙 관리되는 설정 정보를 불러와 사용합니다.

---

이상으로 수정된 코드와 README 예시를 제공합니다.  
코드는 코드 블록으로 제공되었으므로 복사하여 붙여넣을 때 형식이 그대로 유지됩니다.
