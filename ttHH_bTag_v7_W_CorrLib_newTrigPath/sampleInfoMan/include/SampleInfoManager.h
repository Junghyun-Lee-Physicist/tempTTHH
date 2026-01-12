#ifndef SAMPLEINFOMANAGER_H
#define SAMPLEINFOMANAGER_H

#include <string>
#include <unordered_map>
#include <vector>

/// 샘플에 대한 정보를 저장할 구조체  
struct SampleInfo {
    bool isData;      // "Data"이면 true, "MC"이면 false
    double weight;    // MC인 경우 지정된 weight, Data는 무조건 1.0 사용
};

class SampleInfoManager {
public:
    /// 싱글턴 패턴: 전역에서 하나의 인스턴스로 관리  
    static SampleInfoManager& instance();

    /// CSV 파일을 읽어 파싱합니다.  
    /// 만약 인자로 파일명을 전달하지 않으면 내부의 csvFileName을 사용합니다.
    bool loadFromCSV(const std::string &filename = "");

    /// CSV 파일명을 설정합니다. (기본값은 실행 파일과 같은 위치의 "sampleConfig.csv")
    void setCsvFileName(const std::string &filename);

    /// 설정된 CSV 파일명을 반환합니다.
    std::string getCsvFileName() const;

    /// 샘플 이름을 입력받아 해당 SampleInfo를 반환합니다.  
    /// 찾지 못하면 nullptr를 반환합니다.
    const SampleInfo* getSampleInfo(const std::string &sampleName) const;

private:
    SampleInfoManager();
    SampleInfoManager(const SampleInfoManager&) = delete;
    SampleInfoManager& operator=(const SampleInfoManager&) = delete;

    std::unordered_map<std::string, SampleInfo> sampleMap;
    std::string csvFileName;  // CSV 파일명을 저장 (기본: "sampleConfig.csv")

    // 문자열의 앞뒤 공백을 제거하는 헬퍼 함수
    std::string trim(const std::string &s) const;
};

#endif // SAMPLEINFOMANAGER_H
