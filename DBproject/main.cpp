#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <cctype>
#include <vector>
#include <thread>

using namespace std;
void splitFile(int threadNum, int startLine, int endLine, string filename);


// 파일을 주어진 크기에 따라 분할하는 함수
void SplitFileIntoParts(const string& filename, int part_size) {
    ifstream file(filename, std::ios::binary);
    vector<string> parts;

    if (!file) {
        cerr << "파일을 열 수 없습니다: " << filename << std::endl;
        exit(-1);
    }

    // 파일의 끝에 도달할 때까지 읽기
    while (file.good()) {
        vector<char> buffer(part_size);
        file.read(buffer.data(), part_size);
        if (file.gcount() > 0) {
            parts.emplace_back(buffer.data(), file.gcount());
        }
    }

}


void splitFileByThread(int totalThreads, string filename) {
    // 파일을 스레드 수에 따라 분할
    int totalLines = 0;

    ifstream input("./folder/" + filename + ".txt", ios::binary);
    if (!input.is_open()) {
        cout << "[ERROR] file is not open <Split::splitFileByThread>" << endl;
        exit(-1);
    }

    string line;
    while (getline(input, line)) {
        totalLines++;
    }

    input.close();

    int linesPerThread = totalLines / totalThreads;

    for (int threadNum = 0; threadNum < totalThreads; threadNum++) {
        int startLine = threadNum * linesPerThread;
        int endLine = (threadNum == totalThreads - 1) ? totalLines : (threadNum + 1) * linesPerThread;

        splitFile(threadNum, startLine, endLine, filename);
    }
}

void splitFile(int threadNum, int startLine, int endLine, string filename) {
    ifstream input("./folder/" + filename + ".txt", ios::binary);
    if (!input.is_open()) {
        cout << "[ERROR] file is not open <Split::splitFile>" << endl;
        exit(-1);
    }

    string outputFileName = "./folder/_thread" + to_string(threadNum) + ".txt";
    ofstream output(outputFileName);

    if (!output.is_open()) {
        cout << "[ERROR] file is not open <Split::splitFile>" << endl;
        exit(-1);
    }

    string line;
    int currentLine = 0;

    while (getline(input, line)) {
        if (currentLine >= startLine && currentLine < endLine) {
            output << line << endl;
        }
        currentLine++;

        if (currentLine >= endLine) {
            break;
        }
    }

    input.close();
    output.close();
}

bool isPunct(int c) {// 사용자 정의 함수로 특수 문자를 확인합니다.

    return ispunct(static_cast<unsigned char>(c));
}


// 단어를 분류하고 파일에 쓰는 함수
void ClassifyWords(const string& inputFileName, const string& outputFileName) {
    ifstream inFile(inputFileName, ios::in);
    ofstream outFile(outputFileName, ios::out);
    string word;
    map<string, int> wordCounts;

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Unable to open file." << endl;
        return;
    }

    // 단어를 읽고 카운트
    while (inFile >> word) {
        // 특수 문자 제거
        if (!word.empty()) {
            word.erase(remove_if(word.begin(), word.end(), isPunct), word.end());
        }
        transform(word.begin(), word.end(), word.begin(), ::tolower); // 소문자로 변환
        ++wordCounts[word];
    }

    // 결과를 파일에 쓰기
    for (const auto& pair : wordCounts) {
        for (int i = 0; i < pair.second; i++) {//중복된 단어 처리
            outFile << "(" << pair.first << "," << 1 << ")" << endl;
        }
       
    }

    inFile.close();
    outFile.close();
}

int main() {
    string filename;
    int threadCount;

    cout << "filename : ";
    cin >> filename;
    cout << "total thread number" << endl;
    cin >> threadCount;
    splitFileByThread(threadCount, filename);
    // 스레드 생성 및 실행
    vector<thread> threads;
    for (int i = 0; i < threadCount; ++i) {
        string inputFileName = "./folder/_thread" + to_string(i) + ".txt";
        string outputFileName = "./folder/_wordcount_thread" + to_string(i) + ".txt";
        threads.emplace_back(ClassifyWords, inputFileName, outputFileName);
    }

    // 모든 스레드가 완료될 때까지 기다림
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }


    return 0;
}
