#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>

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


/*분할한 파일에 있는 단어를 분류하는 함수*/


int main() {
    string filename;
    int ThreadNum;

    cout << "filename : ";
    cin >> filename;
    cout << "total thread number" << endl;
    cin >> ThreadNum;

    splitFileByThread(ThreadNum, filename);


    return 0;
}
