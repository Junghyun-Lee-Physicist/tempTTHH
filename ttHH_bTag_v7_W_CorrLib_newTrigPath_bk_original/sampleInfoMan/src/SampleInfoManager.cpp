#include "SampleInfoManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cctype>

SampleInfoManager& SampleInfoManager::instance() {
    static SampleInfoManager instance;
    return instance;
}

SampleInfoManager::SampleInfoManager() : csvFileName("sampleConfig.csv") {}

std::string SampleInfoManager::trim(const std::string &s) const {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start))
        start++;
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

void SampleInfoManager::setCsvFileName(const std::string &filename) {
    csvFileName = filename;
}

std::string SampleInfoManager::getCsvFileName() const {
    return csvFileName;
}

bool SampleInfoManager::loadFromCSV(const std::string &filename) {
    std::string fileToLoad = filename.empty() ? csvFileName : filename;
    std::ifstream infile(fileToLoad);
    if (!infile.is_open()) {
        std::cerr << "Cannot open config file: " << fileToLoad << std::endl;
        return false;
    }
    
    sampleMap.clear();
    std::string line;
    while (std::getline(infile, line)) {
        if(line.empty())
            continue;
        std::string trimmedLine = trim(line);
        // '#'로 시작하면 주석 처리
        if(trimmedLine[0] == '#')
            continue;
        // SampleName 뒤에 콜론(:)을 기준으로 분리
        auto colonPos = trimmedLine.find(':');
        if (colonPos == std::string::npos) {
            std::cerr << "Invalid format (missing ':') in line: " << trimmedLine << std::endl;
            std::exit(1);
        }
        std::string sampleName = trim(trimmedLine.substr(0, colonPos));
        std::string rest = trimmedLine.substr(colonPos + 1);
        // 콤마(,)로 항목 분리 (여기서는 2개 항목: Data/MC, weight)
        std::vector<std::string> tokens;
        std::istringstream iss(rest);
        std::string token;
        while (std::getline(iss, token, ',')) {
            tokens.push_back(trim(token));
        }
        if (tokens.size() < 2) {
            std::cerr << "Not enough tokens in line: " << trimmedLine << std::endl;
            std::exit(1);
        }
        SampleInfo info;
        std::string type = tokens[0];
        if (type == "MC")
            info.isData = false;
        else if (type == "Data")
            info.isData = true;
        else {
            std::cerr << "Invalid sample type: '" << type << "' in line: " << trimmedLine << std::endl;
            std::exit(1);
        }
        try {
            info.weight = std::stod(tokens[1]);
        } catch (const std::exception &e) {
            std::cerr << "Error parsing weight in line: " << trimmedLine << "\n" << e.what() << std::endl;
            std::exit(1);
        }
        sampleMap[sampleName] = info;
    }
    infile.close();
    return true;
}

const SampleInfo* SampleInfoManager::getSampleInfo(const std::string &sampleName) const {
    auto it = sampleMap.find(sampleName);
    if (it != sampleMap.end())
        return &(it->second);
    return nullptr;
}
