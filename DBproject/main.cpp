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
#include <future>
#include <queue>

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

/*
// �ܾ �з��ϰ� ���Ͽ� ���� �Լ�
void ClassifyWords(const string& inputFileName, const string& outputFileName) {
    ifstream inFile(inputFileName, ios::in);
    ofstream outFile(outputFileName, ios::out);
    string word;
    map<string, vector<int>> wordCounts;

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
        wordCounts[word].push_back(1);
    }

    // ����� ���Ͽ� ����
    for (const auto& pair : wordCounts) {
        if (pair.first == "") continue; //���鹮�� ����ó��
        outFile << "(" << pair.first << ", [";
        for (int i = 0; i < pair.second.size(); i++) {
            outFile << pair.second[i];
            if (i < pair.second.size() - 1) {
                outFile << ", ";
            }
        }
        outFile << "])" << endl;
    }

    inFile.close();
    outFile.close();
}*/

future<void> ClassifyWordsAsync(const string& inputFileName, const string& outputFileName) {
    return async(launch::async, [inputFileName, outputFileName] {
        ifstream inFile(inputFileName, ios::in);
        ofstream outFile(outputFileName, ios::out);
        string word;
        map<string, vector<int>> wordCounts;

        if (!inFile.is_open() || !outFile.is_open()) {
            cerr << "Unable to open file." << endl;
            return;
        }

        while (inFile >> word) {
            if (!word.empty()) {
                word.erase(remove_if(word.begin(), word.end(), isPunct), word.end());
            }
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            wordCounts[word].push_back(1);
        }

        for (const auto& pair : wordCounts) {
            if (pair.first == "") continue;
            outFile << "(" << pair.first << ", [";
            for (int i = 0; i < pair.second.size(); i++) {
                outFile << pair.second[i];
                if (i < pair.second.size() - 1) {
                    outFile << ", ";
                }
            }
            outFile << "])" << endl;
        }

        inFile.close();
        outFile.close();

        remove(inputFileName.c_str());
        });
}

pair<string, vector<int>> parseLine(const string& line) {
    stringstream ss(line);
    string key;
    vector<int> values;
    // '(' ����
    getline(ss, key, ',');
    key = key.substr(1);
    // ',' ������ ' [' ����
    ss.ignore(2);
    // �迭 �Ľ�
    int value;
    while (ss >> value) {
        values.push_back(value);
        // ',' �Ǵ� ']' �ǳʶٱ�
        ss.ignore(2);
    }
    return { key, values };
}

void printResult(ofstream& out, const string& key, const vector<int>& values) {
    out << "(" << key << ", [";
    for (size_t i = 0; i < values.size(); ++i) {
        out << values[i];
        if (i < values.size() - 1) {
            out << ", ";
        }
    }
    out << "])\n";
}

void merge(int threadCount) {
    vector<ifstream> files(threadCount);
    for (int i = 0; i < threadCount; ++i) {
        string inputFileName = "./folder/_wordcount_thread" + to_string(i) + ".txt";
        files[i].open(inputFileName);
        if (!files[i]) {
            cerr << "Cannot open file: " << inputFileName << endl;
            return;
        }
    }

    ofstream out("./folder/output.txt");
    if (!out) {
        cerr << "Cannot open file: output.txt" << endl;
        return;
    }

    auto comp = [](pair<int, pair<string, vector<int>>> a, pair<int, pair<string, vector<int>>> b) {
        return a.second.first > b.second.first;
    };
    priority_queue<pair<int, pair<string, vector<int>>>, vector<pair<int, pair<string, vector<int>>>>, decltype(comp)> pq(comp);

    string line;
    for (int i = 0; i < threadCount; ++i) {
        if (getline(files[i], line)) {
            pair<string, vector<int>> parsed = parseLine(line);
            string key = parsed.first;
            vector<int> values = parsed.second;
            pq.push({ i, {key, values} });
        }
    }

    while (!pq.empty()) {
        pair<int, pair<string, vector<int>>> top = pq.top();
        int fileIndex = top.first;
        pair<string, vector<int>> keyValue = top.second;
        pq.pop();

        vector<int> totalValues = keyValue.second;
        while (!pq.empty() && pq.top().second.first == keyValue.first) {
            pair<int, pair<string, vector<int>>> nextTop = pq.top();
            int nextFileIndex = nextTop.first;
            pair<string, vector<int>> nextKeyValue = nextTop.second;
            pq.pop();
            totalValues.insert(totalValues.end(), nextKeyValue.second.begin(), nextKeyValue.second.end());
            if (getline(files[nextFileIndex], line)) {
                pair<string, vector<int>> parsed = parseLine(line);
                string key = parsed.first;
                vector<int> values = parsed.second;
                pq.push({ nextFileIndex, {key, values} });
            }
        }

        printResult(out, keyValue.first, totalValues);

        if (getline(files[fileIndex], line)) {
            pair<string, vector<int>> parsed = parseLine(line);
            string key = parsed.first;
            vector<int> values = parsed.second;
            pq.push({ fileIndex, {key, values} });
        }
    }

    for (int i = 0; i < threadCount; ++i) {
        files[i].close(); // ������ �ݽ��ϴ�.

        string inputFileName = "./folder/_wordcount_thread" + to_string(i) + ".txt";
        if (remove(inputFileName.c_str()) != 0) { // ������ �����մϴ�.
            cerr << "Error deleting file: " << inputFileName << endl;
        }
        else {}
    }

    out.close();
}

void reduce() {
    ifstream in("./folder/output.txt");
    if (!in) {
        cerr << "Cannot open file: output.txt" << endl;
        return;
    }

    ofstream out("./folder/result.txt");
    if (!out) {
        cerr << "Cannot open file: reduced_output.txt" << endl;
        return;
    }

    string line;
    while (getline(in, line)) {
        pair<string, vector<int>> parsed = parseLine(line);
        string key = parsed.first;
        vector<int> values = parsed.second;
        out << "(" << key << ", " << values.size() << ")\n";
    }

    in.close();
    remove("./folder/output.txt");
}


int main() {
    clock_t full_start, full_end;
    clock_t split_start, split_end;
    clock_t merge_start, merge_end;
    double duration;
    string filename;
    int threadCount;
    /*
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <thread count>\n";
        return 1;
    }

    string filename = argv[1];
    int threadCount = std::stoi(argv[2]);
    */
    
    cout << "filename : ";
    cin >> filename;
    cout << "total thread number : ";
    cin >> threadCount;
    
    cout << "file split start" << endl;

    splitFileByThread(threadCount, filename);
    
    cout << "file split end" << endl;

    // ������ ���� �� ����
    vector<thread> threads;

    full_start = clock();
    split_start = clock();
    /*
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
    }*/

    vector<std::future<void>> futures;

    cout << "word split start" << endl;

    for (int i = 0; i < threadCount; ++i) {
        string inputFileName = "./folder/_thread" + to_string(i) + ".txt";
        string outputFileName = "./folder/_wordcount_thread" + to_string(i) + ".txt";
        futures.push_back(ClassifyWordsAsync(inputFileName, outputFileName));
    }

    for (auto& future : futures) {
        future.get();
    }

    cout << "word split end" << endl;

    split_end = clock();

    cout << endl;
    duration = (double)(split_end - split_start) / CLOCKS_PER_SEC;
    cout << "split_time : " << duration << "s" << endl;
    cout << endl;


    // merge �κ�
    cout << "merge start" << endl;
    merge_start = clock();
    merge(threadCount);
    merge_end = clock();
    cout << "merge end" << endl << endl;

    duration = (double)(merge_end - merge_start) / CLOCKS_PER_SEC;
    cout << "merge_time : " << duration << "s" << endl;
    cout << endl;


    cout << "reduce start" << endl;
    reduce();
    cout << "reduce end" << endl << endl;

    full_end = clock();
    duration = (double)(full_end - full_start) / CLOCKS_PER_SEC;
    cout << "full_time : " << duration << "s" << endl;
    cout << endl;

    return 0;
    
}
