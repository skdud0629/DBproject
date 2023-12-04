#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>

// ������ �־��� ũ�⿡ ���� �����ϴ� �Լ�
std::vector<std::string> SplitFileIntoParts(const std::string& filename, size_t part_size) {
    std::ifstream file(filename);
    std::vector<std::string> parts;
    std::string part;
    part.reserve(part_size);

    // ������ ���� ������ ������ �б�
    while (file.good()) {
        char buffer[part_size];
        file.read(buffer, part_size);
        parts.emplace_back(buffer, file.gcount());
    }

    return parts;
}

// Map �Լ��� �ؽ�Ʈ ������ �Է����� �޾� �ܾ�� ���� Ƚ���� �ʿ� �߰��մϴ�.
void Map(const std::string& text, std::map<std::string, int>& M) {
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        M[word]++;
    }
}

// Reduce �Լ��� ���� �Է����� �޾� �ܾ�� ���� Ƚ���� ���͸� �����մϴ�.
void Reduce(const std::map<std::string, int>& map, std::map<std::string, int>& reduced) {
    for (const auto& kv : map) {
        reduced[kv.first] += kv.second;
    }
}

int main() {
    std::string filename = "input.txt"; // �Է� ���� �̸�
    size_t part_size = 1024; // ������ ������ ũ��

    // ������ ����
    std::vector<std::string> parts = SplitFileIntoParts(filename, part_size);

    // �� �κп� ���� MapReduce ����
    std::vector<std::map<std::string, int>> intermediate_maps(parts.size());
    for (size_t i = 0; i < parts.size(); ++i) {
        Map(parts[i], intermediate_maps[i]);
    }

    // ��� �߰� ����� Reduce
    std::map<std::string, int> final_result;
    for (const auto& imap : intermediate_maps) {
        Reduce(imap, final_result);
    }

    // ���� ��� ���
    for (const auto& kv : final_result) {
        std::cout << kv.first << " -> " << kv.second << std::endl;
    }

    return 0;
}
