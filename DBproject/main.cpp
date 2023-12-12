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


// ÆÄÀÏÀ» ÁÖ¾îÁø Å©±â¿¡ µû¶ó ºÐÇÒÇÏ´Â ÇÔ¼ö
void SplitFileIntoParts(const string& filename, int part_size) {
    ifstream file(filename, std::ios::binary);
    vector<string> parts;

    if (!file) {
        cerr << "ÆÄÀÏÀ» ¿­ ¼ö ¾ø½À´Ï´Ù: " << filename << std::endl;
        exit(-1);
    }

    // ÆÄÀÏÀÇ ³¡¿¡ µµ´ÞÇÒ ¶§±îÁö ÀÐ±â
    while (file.good()) {
        vector<char> buffer(part_size);
        file.read(buffer.data(), part_size);
        if (file.gcount() > 0) {
            parts.emplace_back(buffer.data(), file.gcount());
        }
    }

}


void splitFileByThread(int totalThreads, string filename) {
    // ÆÄÀÏÀ» ½º·¹µå ¼ö¿¡ µû¶ó ºÐÇÒ
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

bool isPunct(int c) {// »ç¿ëÀÚ Á¤ÀÇ ÇÔ¼ö·Î Æ¯¼ö ¹®ÀÚ¸¦ È®ÀÎÇÕ´Ï´Ù.

    return ispunct(static_cast<unsigned char>(c));
}


// ´Ü¾î¸¦ ºÐ·ùÇÏ°í ÆÄÀÏ¿¡ ¾²´Â ÇÔ¼ö
void ClassifyWords(const string& inputFileName, const string& outputFileName) {
    ifstream inFile(inputFileName, ios::in);
    ofstream outFile(outputFileName, ios::out);
    string word;
    map<string, int> wordCounts;

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Unable to open file." << endl;
        return;
    }

    // ´Ü¾î¸¦ ÀÐ°í Ä«¿îÆ®
    while (inFile >> word) {
        // Æ¯¼ö ¹®ÀÚ Á¦°Å
        if (!word.empty()) {
            word.erase(remove_if(word.begin(), word.end(), isPunct), word.end());
        }
        transform(word.begin(), word.end(), word.begin(), ::tolower); // ¼Ò¹®ÀÚ·Î º¯È¯
        ++wordCounts[word];
    }

    // °á°ú¸¦ ÆÄÀÏ¿¡ ¾²±â
    for (const auto& pair : wordCounts) {
        if (pair.first == "")continue; //°ø¹é¹®ÀÚ ¿¹¿ÜÃ³¸®
        for (int i = 0; i < pair.second; i++) {//Áßº¹µÈ ´Ü¾î Ã³¸®
            outFile << "(" << pair.first << "," << 1 << ")" << endl;
        }
       
    }

    inFile.close();
    outFile.close();
}

int merge(int threadNum, const string& inputFileName) {
    string filename;


    //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½->ï¿½Ê¹Ý¿ï¿½ ï¿½Ú¸ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ 
    //ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ù¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Â½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ï´Ù°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 1ï¿½ï¿½ï¿½ï¿½ ï¿½É¶ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½.
    return 0;
}

int main() {
    clock_t full_start, full_end;
    clock_t split_start, split_end;
    clock_t merge_start, merge_end;
    double duration;
    string filename;
    int threadCount;

    cout << "filename : ";
    cin >> filename;
    cout << "total thread number" << endl;
    cin >> threadCount;
    splitFileByThread(threadCount, filename);
    // ½º·¹µå »ý¼º ¹× ½ÇÇà
    vector<thread> threads;

    full_start = clock();
    split_start = clock();

    for (int i = 0; i < threadCount; ++i) {
        string inputFileName = "./folder/_thread" + to_string(i) + ".txt";
        string outputFileName = "./folder/_wordcount_thread" + to_string(i) + ".txt";
        threads.emplace_back(ClassifyWords, inputFileName, outputFileName);
    }

    // ¸ðµç ½º·¹µå°¡ ¿Ï·áµÉ ¶§±îÁö ±â´Ù¸²
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    split_end = clock();

    cout << endl;
    duration = (double)(split_end - split_start) / CLOCKS_PER_SEC;
    cout << "split_time : " << duration << endl;
    cout << endl;


    return 0;
}
