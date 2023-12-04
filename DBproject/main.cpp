#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>

// 파일을 주어진 크기에 따라 분할하는 함수
std::vector<std::string> SplitFileIntoParts(const std::string& filename, size_t part_size) {
    std::ifstream file(filename);
    std::vector<std::string> parts;
    std::string part;
    part.reserve(part_size);

    // 파일의 끝에 도달할 때까지 읽기
    while (file.good()) {
        char buffer[part_size];
        file.read(buffer, part_size);
        parts.emplace_back(buffer, file.gcount());
    }

    return parts;
}

// Map 함수는 텍스트 조각을 입력으로 받아 단어별로 출현 횟수를 맵에 추가합니다.
void Map(const std::string& text, std::map<std::string, int>& M) {
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        M[word]++;
    }
}

// Reduce 함수는 맵을 입력으로 받아 단어와 출현 횟수의 벡터를 생성합니다.
void Reduce(const std::map<std::string, int>& map, std::map<std::string, int>& reduced) {
    for (const auto& kv : map) {
        reduced[kv.first] += kv.second;
    }
}

int main() {
    std::string filename = "input.txt"; // 입력 파일 이름
    size_t part_size = 1024; // 파일을 분할할 크기

    // 파일을 분할
    std::vector<std::string> parts = SplitFileIntoParts(filename, part_size);

    // 각 부분에 대해 MapReduce 수행
    std::vector<std::map<std::string, int>> intermediate_maps(parts.size());
    for (size_t i = 0; i < parts.size(); ++i) {
        Map(parts[i], intermediate_maps[i]);
    }

    // 모든 중간 결과를 Reduce
    std::map<std::string, int> final_result;
    for (const auto& imap : intermediate_maps) {
        Reduce(imap, final_result);
    }

    // 최종 결과 출력
    for (const auto& kv : final_result) {
        std::cout << kv.first << " -> " << kv.second << std::endl;
    }

    return 0;
}
