/* TITLE
*
* Author:
*
* Description:
*/

//#include "stdafx.h"

#include "Stock.h"

mutex mtx; //For keeping one thread handling data at a time
unsigned short NUM_THREADS;
const unsigned short MAX_DIR_LEN = 255;
char OUT_DIR[MAX_DIR_LEN], FILE_TYPE_OUT[100], TICKER_FILE[MAX_DIR_LEN], BASE_URL[MAX_DIR_LEN];

void ReadSettingsINI() {
    LPSTR iniPath = ".//resource/settings.ini";
    NUM_THREADS = GetPrivateProfileInt("global", "iThreads", 1, iniPath);
    GetPrivateProfileString("global", "sOutputDirectory", "data/", OUT_DIR, sizeof(OUT_DIR) / sizeof(OUT_DIR[0]), iniPath);
    GetPrivateProfileString("global", "sOutputFileType", ".txt", FILE_TYPE_OUT, sizeof(FILE_TYPE_OUT) / sizeof(FILE_TYPE_OUT[0]), iniPath);
    GetPrivateProfileString("global", "sTickerFile", "resource/TickerSymbols.txt", TICKER_FILE, sizeof(TICKER_FILE) / sizeof(TICKER_FILE[0]), iniPath);
    GetPrivateProfileString("global", "sBaseURL", "http://finance.yahoo.com/q/hp?s=+Historical+Prices", BASE_URL, sizeof(BASE_URL) / sizeof(BASE_URL[0]), iniPath);
    cout << NUM_THREADS << " " << OUT_DIR << " " << FILE_TYPE_OUT << " " << TICKER_FILE << endl;
}
int CountNumSymbols(ifstream &inFile);
void FetchStocks(vector<string> &symbolList);

int main(int argc, char const *argv[]) {
    ReadSettingsINI();
    int numSymbols, listLength, remainder;
    bool remainderFlag = false;
    unsigned int i = 1;
    string symbol;
    vector<string> list; //vector of a few symbols
    vector<vector<string> > symbols; //vector of vectors to include all symbols
    vector<thread> threads; //vector of all threads used
    ifstream inFile(TICKER_FILE); //opens list of desired companies
    if (inFile.fail()) {
        cerr << "Failed to open file: " << TICKER_FILE << endl;
        exit(1);
    }

    /*** Separate symbols for multi-threading: ***/
    //Count the number of symbols in the file:
    numSymbols = CountNumSymbols(inFile);
    cout << "Number of symbols: " << numSymbols << endl;
    listLength = numSymbols / NUM_THREADS; //Length of list for each thread to use
    remainder = numSymbols % NUM_THREADS; //Never greater than NUM_THREADS
    //Distribute symbols as evenly as possible, especially if we don't need all threads (listLength=0)
    if (remainder > 0) {
        listLength++;
        remainderFlag = true;
    }
    cout << "Remainder: " << remainder << endl; //FIXME
    //Create lists for threads:
    while (getline(inFile, symbol)) {
        //Adjust the proceeding lists for even distribution:
        if (remainderFlag == true && remainder == 0) {
            listLength--;
            remainderFlag = false;
        }

        list.push_back(symbol);
        if (i < (unsigned int)listLength) i++;
        else {
            symbols.push_back(list);
            list.clear();
            i = 1;
            if (remainder > 0) remainder--;
        }
    }
    inFile.close();

    /*** Create threads that download data for each of the symbols ***/
    _mkdir(OUT_DIR); //makes directory in .exe path
    curl_global_init(CURL_GLOBAL_ALL); //init curl, call only once
    if (NUM_THREADS > symbols.size()) NUM_THREADS = (unsigned int)symbols.size();
    cout << "Threads: " << NUM_THREADS << endl;
    //Start threads:
    for (i = 0; i < NUM_THREADS; ++i) {
        threads.push_back(thread(FetchStocks, symbols.at(i)));
    }
    //Join threads to main process (wait for completion):
    for (i = 0; i < threads.size(); ++i) {
        threads.at(i).join();
    }
    curl_global_cleanup(); //cleanup curl, call only once

    return EXIT_SUCCESS;
}

//Returns the integer number of symbols in the ticker file
int CountNumSymbols(ifstream &inFile) {
    int i = 0;
    string line;
    while (getline(inFile, line)) {
        i++;
    }

    //Return to beginning of stream
    inFile.clear();
    inFile.seekg(0, inFile.beg);

    return i;
}

void FetchStocks(vector<string> &symbolList) {
    string filename;

    //Lock so that only one thread prints out at a time.
    mtx.lock();
    for (unsigned int i = 0; i < symbolList.size(); ++i) {
        cout << symbolList.at(i) << ", ";
    }
    cout << endl;
    mtx.unlock();

    //loops through the ticker file
    for (unsigned int i = 0; i < symbolList.size(); ++i) {
        Stock stock(symbolList.at(i));
        stock.SetBaseURL(BASE_URL);
        stock.DownloadQuotes(); //populates vectors
        filename = OUT_DIR + stock.GetSymbol() + FILE_TYPE_OUT;
        stock.WriteToFile(filename);
    }
}