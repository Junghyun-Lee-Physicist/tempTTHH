# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(ttHH_bTagStudy)

# Set C++ standard to C++17 to match ROOT requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the ROOT package
find_package(ROOT REQUIRED)

# Include ROOT headers
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(include)
include_directories("/Users/jhlee/Desktop/Work/sampleInfoMan/include")

# Add executable
add_executable(exe_bTagStudy main_bTag.cpp src/BTagCalibrationStandalone.cpp src/makeBTagWeight.cpp src/makeBTagWeight_reweight.cpp src/makeBTagWeight_Final.cpp)

# Link against ROOT libraries
target_link_libraries(exe_bTagStudy ${ROOT_LIBRARIES} "/Users/jhlee/Desktop/Work/sampleInfoMan/lib/libSampleInfoManager.a")
