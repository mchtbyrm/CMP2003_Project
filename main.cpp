#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cassert>
using namespace std;

clock_t startTime, endTime;

class hashUnigram {
private:
    string* listUnigrams;
    string stopWords[571];
    int* totalCount;
    int* indexStatus;
public:
    hashUnigram()
    {
        listUnigrams = new string[1000000];
        assert(listUnigrams != nullptr);

        totalCount = new int[1000000];
        assert(totalCount != nullptr);

        indexStatus = new int[1000000];
        assert(indexStatus != nullptr);
    }
    hashUnigram(const hashUnigram& otherList)
    {
        listUnigrams = new string[1000000];
        assert(listUnigrams != nullptr);
        for (int j = 0; j < 1000000; j++) {
            listUnigrams[j] = otherList.listUnigrams[j];
        }
        for (int i = 0; i < 571; i++) {
            stopWords[i] = otherList.stopWords[i];
        }
        totalCount = new int[1000000];
        assert(totalCount != nullptr);
        for (int i = 0; i < 1000000; ++i) {
            totalCount[i] = otherList.totalCount[i];
        }
        indexStatus = new int[1000000];
        assert(indexStatus != nullptr);
        for (int i = 0; i < 1000000; ++i) {
            indexStatus[i] = otherList.indexStatus[i];
        }
    }
    ~hashUnigram() {
        delete[] listUnigrams;
        delete[] indexStatus;
        delete[] totalCount;
    }
    void insertUnigram(string unigram, int count) {
        int total = 0;
        for (char i : unigram) {
            total += (int)i;
        }
        int key = (total * unigram[0] * 20) % 1000000;
        while (indexStatus[key] == 1 && listUnigrams[key] != unigram) {
            key = (key + 1) % 1000000;
        }

        if (listUnigrams[key] == unigram) {
            totalCount[key] += count;
        }

        if (indexStatus[key] != 1) {
            listUnigrams[key] = unigram;
            totalCount[key] += count;
            indexStatus[key] = 1;
        }
    }
    void insertStop() {
        ifstream in;
        in.open("stopwords.txt");
        string stop;
        int i = 0;
        while (!in.eof()) {
            in >> stop;
            stopWords[i] = stop;
            i++;
        }
    }
    bool isStopWord(const string& s) {
        bool found = false;
        for (auto & stopWord : stopWords) {
            if (s == stopWord) {
                found = true;
            }
            if (found) {
                break;
            }
        }
        return found;
    }

    std::string* sortTop10() {

        auto* TempList = new std::string[20];
        auto* tempUnigrams = new std::string[1000000];
        int* temptotalCount = new int[1000000];

        for (int i = 0; i < 1000000; i++) {

            tempUnigrams[i] = listUnigrams[i];
            temptotalCount[i] = totalCount[i];
        }

        for (int i = 0; i < 20; i++) {
            TempList[i] = "";
        }

        int max;
        int maxPosition;

        for (int i = 0; i < 10; i++) {
            max = 0;
            maxPosition = 0;
            for (int j = 0; j < 1000000; j++) {
                if (temptotalCount[j] > max) {
                    max = temptotalCount[j];
                    maxPosition = j;
                }
            }
            TempList[i * 2] = tempUnigrams[maxPosition];
            TempList[(i * 2) + 1] = std::to_string(temptotalCount[maxPosition]);
            temptotalCount[maxPosition] = -1;
            tempUnigrams[maxPosition] = "";
        }
        return TempList;
    }
    void printTop10() {
        string* PrintList = sortTop10();

        for (int i = 0; i < 10; i++) {
            std::cout << PrintList[i * 2] << " " << PrintList[(i * 2) + 1] << std::endl;
        }
    }
};

int main() {
    startTime = clock();

    hashUnigram h;
    h.insertStop();

    ifstream inFile;
    inFile.open("PublicationsDataSet.txt");

    auto* document = new string;

    while (!inFile.eof()) {
        getline(inFile, *document);
        size_t pos = document->rfind("\":{\"");
        if (pos != string::npos) {
            *document = document->substr(pos + 3);
        }
        document->insert(0, 1, ',');
        document->pop_back();
        document->pop_back();
        *document += ",\"\":";
        while (document->length() > 0) {
            size_t first = document->find(",\"");
            if (first != string::npos) {
                *document = document->substr(first + 2);
            }
            size_t last = document->find("\":");
            string unigram = document->substr(0, last + 0);

            if (last != string::npos) {
                *document = document->substr(last + 2);
            }
            size_t newPos = document->find(",\"");
            string countString = document->substr(0, newPos + 0);

            int digit = 1;
            int count = 0;
            for (int i = countString.length() - 1; i >= 0; i--) {
                count += (countString[i] - 48) * digit;
                digit *= 10;
            }

            int numOfUnwantedChar = 0;

            while (!((unigram[0] >= 'a' && unigram[0] <= 'z') ||
                (unigram[0] >= '0' && unigram[0] <= '9') ||
                unigram[0] >= 'A' && unigram[0] <= 'Z')
                && unigram.length() > 0) {
                unigram = unigram.substr(1);
            }

            while (!((unigram[unigram.length() - 1] >= 'a' && unigram[unigram.length() - 1] <= 'z') ||
                (unigram[unigram.length() - 1] >= '0' && unigram[unigram.length() - 1] <= '9') ||
                unigram[unigram.length() - 1] >= 'A' && unigram[unigram.length() - 1] <= 'Z') &&
                unigram.length() > 0)
            {
                unigram.pop_back();
            }

            if (unigram.length() > 0) {

                for (char i : unigram) {
                    if (!(i >= 'a' && i <= 'z') && i != '\'' && !(i >= 'A' && i <= 'Z')) {
                        numOfUnwantedChar++;
                        break;
                    }
                }

                if (numOfUnwantedChar == 0) {
                    for (char & i : unigram) {
                        if (i >= 'A' && i <= 'Z') {
                            i = char(i + 32);
                        }
                    }

                    if (!h.isStopWord(unigram)) {
                        h.insertUnigram(unigram, count);
                    }
                }
            }
        }
    }
    inFile.close();

    h.printTop10();

    endTime = clock();
    cout << "Time elapsed : " << ((double)(endTime - startTime)) / CLOCKS_PER_SEC << " seconds" << endl;
    return 0;
}