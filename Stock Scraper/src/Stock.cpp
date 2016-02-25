#include "Stock.h"

Stock::Stock(string symbol) {
    SetSymbol(symbol);
    this->baseURL = "http://finance.yahoo.com/q/hp?s=+Historical+Prices";
    this->chunk.memory = new string;
    this->chunk.size = 0;
}

Stock::~Stock() {
    if (chunk.memory) {
        chunk.size = 0;
        chunk.memory->clear();
        delete chunk.memory;
    }
}

//Returns the base URL - default is Yahoo
string Stock::GetBaseURL() {
    return this->baseURL;
}

//Returns the ticker symbol for the stock
string Stock::GetSymbol() {
    return this->symbol;
}

//Returns the URL used to download quotes
string Stock::GetURL() {
    return this->url;
}

//Returns the vector of vectors of stock data filled by DownloadQuotes()
vector<vector<string> > Stock::GetQuotes() {
    return this->quotes;
}

/*
* Sets the base URL - default is Yahoo
* http://finance.yahoo.com/q/hp?s=+Historical+Prices
*
* The ticker symbol should be inserted between "s=" and
* "+Historical" in order to goto the corresponding
* stock's historical prices web page.
*/
void Stock::SetBaseURL(string baseURL) {
    this->baseURL = baseURL;
}

//Sets the ticker symbol for the stock
void Stock::SetSymbol(string symbol) {
    this->symbol = symbol;
}

void Stock::SetURL(string url) {
    this->url = url;
}

/*
* Downloads the HTML web page from the specified URL, then stores
* the dates and quotes into the vectors.
*/
void Stock::DownloadQuotes(string url) {
    CURL *easyHandle; //handle of curl calls
    CURLcode res; //response from curl
    string table, row;
    string value;
    size_t tablePos1, tablePos2, rowPos1, rowPos2;

    if (url.empty()) {
        this->UpdateURL();
        url = this->url;
    }

    //Clear and initialize the vector
    this->quotes.clear();
    this->quotes.resize(7, vector<string>(0));

    easyHandle = curl_easy_init(); //initialize handle
    if (easyHandle) {
        /*** Set options for the handle: ***/
        //sets the URL:
        curl_easy_setopt(easyHandle, CURLOPT_URL, url.c_str());
        //sets the write function:
        curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        /*sets the data to get out of the write function,
        * chunk.memory is a pointer to the string containing the web page.
        * chunk.size is the size in bytes of the string
        */
        curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, (void*)&this->chunk);
        /* some servers don't like requests that are made without a user-agent
        field, so I provide one */
        curl_easy_setopt(easyHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(easyHandle);
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        }
        else if (chunk.size < MIN_PAGE_SIZE) {
            cout << symbol << ": " << (long)this->chunk.size << " bytes...";
            cout << " Skipped." << endl;
        }
        else {
            cout << symbol << ": " << (long)this->chunk.size << " bytes" << endl;

            table = *chunk.memory;
            row = " ";

            //Find table:
            tablePos1 = table.find("<table class=\"yfnc_datamodoutline1\"");
            tablePos2 = table.find("</table>", tablePos1);
            table = table.substr(tablePos1, tablePos2 - tablePos1);

            tablePos1 = table.find("</tr>") + 5; //Skip header row

            //loop that gets all data from the table
            while (row.find("colspan=\"7\"") == string::npos) {
                tablePos2 = table.find("</tr>", tablePos1) + 5;

                row = table.substr(tablePos1, tablePos2 - tablePos1); //<tr>...</tr>

                //If there is more than one column:
                if (row.find("colspan=\"7\"") == string::npos) {
                    //Append date:
                    rowPos1 = row.find("right\">") + 7;
                    rowPos2 = row.find("</td>", rowPos1);
                    value = row.substr(rowPos1, rowPos2 - rowPos1);
                    this->quotes.at(0).push_back(value);
                    //If there are more than 2 columns:
                    if (row.find("colspan=\"6\"") == string::npos) {
                        //Iterate through the rest of the row
                        for (unsigned int i = 1; i < this->quotes.size(); ++i) {
                            rowPos1 = row.find("right\">", rowPos2) + 7;
                            rowPos2 = row.find("</td>", rowPos1);
                            value = row.substr(rowPos1, rowPos2 - rowPos1);
                            //Erase \n and large white spaces:
                            value.erase(remove(value.begin(), value.end(), '\n'), value.end());
                            value.erase(remove(value.begin(), value.end(), '\t'), value.end());
                            while (value.find("  ") != string::npos) {
                                value.erase(value.find("  "), 2);
                            }
                            this->quotes.at(i).push_back(value);
                        }
                    }
                    else {
                        //Dividend, Stock Split, etc:
                        rowPos1 = row.find("colspan=\"6\">") + 12;
                        rowPos2 = row.find("<", rowPos1);
                        value = row.substr(rowPos1, rowPos2 - rowPos1);
                        //Append to every row (except dates):
                        for (unsigned int i = 1; i < this->quotes.size(); i++) {
                            this->quotes.at(i).push_back(value);
                        }
                    }

                    tablePos1 = tablePos2;
                }
            }
        }

        curl_easy_cleanup(easyHandle);
    }
}

//Inserts the ticker symbol into the default string
void Stock::UpdateURL() {
    if (this->baseURL == "http://finance.yahoo.com/q/hp?s=+Historical+Prices") {
        this->url = this->baseURL;
        this->url.insert(32, this->symbol);
    }
}

/*
* libcurl needs a write function in order to write the web page data that it scrapes.
* This is the function that the Stock class uses. It stores the HTML as well as the
* size (in bytes) of the web page. libcurl is written in C.
*/
size_t Stock::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        size_t realsize = size * nmemb;
        struct MemoryStruct *mem = (struct MemoryStruct *)userp;
        char *sptr = (char *)contents;

        for (unsigned int i = 0; i < realsize; ++i) {
            *mem->memory += sptr[i];
        }

        mem->size += realsize;

        return realsize;
    }

/*
* Will write vectors of dates and quotes to specified file.
* Call DownloadQuotes() first.
*/
void Stock::WriteToFile(string filename) {
    size_t i = this->quotes.at(0).size();
    ofstream file;
    file.open(filename.c_str());

    file << "Ticker Symbol: " << this->symbol << '\n';
    file << "Date Range: " << this->quotes.at(0).back() << " - " << this->quotes.at(0).front() << '\n';
    file << '\n';
    file << setw(16) << left << "Date" << setw(16) << left << "Open" << setw(16) << left << "High" << setw(16) << left << "Low" << setw(16) << left << "Close" << setw(16) << left << "Volume" << setw(16) << left << "Adj. Close" << '\n';
    while (i > 0) {
        --i;
        for (unsigned int j = 0; j < this->quotes.size(); j++) {
            file << setw(16) << left << quotes.at(j).at(i);
        }
        file << '\n';
    }

    file.close();
}