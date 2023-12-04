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


// ������ �־��� ũ�⿡ ���� �����ϴ� �Լ�
void SplitFileIntoParts(const string& filename, int part_size) {
    ifstream file(filename, std::ios::binary);
    vector<string> parts;

    if (!file) {
        cerr << "������ �� �� �����ϴ�: " << filename << std::endl;
        exit(-1);
    }

    // ������ ���� ������ ������ �б�
    while (file.good()) {
        vector<char> buffer(part_size);
        file.read(buffer.data(), part_size);
        if (file.gcount() > 0) {
            parts.emplace_back(buffer.data(), file.gcount());
        }
    }

}


void splitFileByThread(int totalThreads, string filename) {
    // ������ ������ ���� ���� ����
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

bool isPunct(int c) {// ����� ���� �Լ��� Ư�� ���ڸ� Ȯ���մϴ�.

    return ispunct(static_cast<unsigned char>(c));
}


// �ܾ �з��ϰ� ���Ͽ� ���� �Լ�
void ClassifyWords(const string& inputFileName, const string& outputFileName) {
    ifstream inFile(inputFileName, ios::in);
    ofstream outFile(outputFileName, ios::out);
    string word;
    map<string, int> wordCounts;

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Unable to open file." << endl;
        return;
    }

    // �ܾ �а� ī��Ʈ
    while (inFile >> word) {
        // Ư�� ���� ����
        if (!word.empty()) {
            word.erase(remove_if(word.begin(), word.end(), isPunct), word.end());
        }
        transform(word.begin(), word.end(), word.begin(), ::tolower); // �ҹ��ڷ� ��ȯ
        ++wordCounts[word];
    }

    // ����� ���Ͽ� ����
    for (const auto& pair : wordCounts) {
        for (int i = 0; i < pair.second; i++) {//�ߺ��� �ܾ� ó��
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
    // ������ ���� �� ����
    vector<thread> threads;
    for (int i = 0; i < threadCount; ++i) {
        string inputFileName = "./folder/_thread" + to_string(i) + ".txt";
        string outputFileName = "./folder/_wordcount_thread" + to_string(i) + ".txt";
        threads.emplace_back(ClassifyWords, inputFileName, outputFileName);
    }

    // ��� �����尡 �Ϸ�� ������ ��ٸ�
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }


    return 0;
}
