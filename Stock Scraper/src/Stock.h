#pragma once

//#include "stdafx.h"

class Stock {
    public:
        Stock(string symbol = "GOOG");
        ~Stock();
        //if a web page is smaller than this, it's ignored
        static const size_t MIN_PAGE_SIZE = 5000;
        string GetBaseURL();
        string GetSymbol();
        string GetURL();
        vector<vector<string> > GetQuotes();
        void SetSymbol(string symbol);
        void SetBaseURL(string baseURL);
        void SetURL(string url);
        void DownloadQuotes(string url = "");
        void WriteToFile(string filename);
    private:
        string symbol; //ticker symbol
        string baseURL; //base URL
        string url; //URL used to download quotes
        //Vector of vectors storing stock data.
        //[0] dates, [1] open prices, [2] high prices, [3] low prices, [4] close prices, [5] volumes, [6] adjusted close prices
        vector<vector<string> > quotes;
        struct MemoryStruct {
            string *memory; //pointer to chunk of memory that will transfer a web page to file
            size_t size; //size of the chunk of memory that stores the web page
        };
        struct MemoryStruct chunk;
        void UpdateURL();
        static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
};