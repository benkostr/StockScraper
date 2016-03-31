# Stock Scraper #
- - - 
**Author:** Benjamin Kostreva (benkostr@gmail.com)

### What is this repository for?
This application uses web scraping to download historical
stock data from Yahoo! Finance.  
[resources/TickerSymbols.txt](resources/TickerSymbols.txt) contains a list of
ticker symbols that the application will use. Downloaded data for each company
is currently stored in individual files within the [data](data) directory.

### How do I get set up?
* Compile x86/x64 ```libcurl.dll``` (Release) &
  ```libcurld.dll``` (Debug) builds (no ssl, no ssh)

  - Read the [wiki](https://gitlab.com/Quijanza/stock-scraper/wikis/home)
  for a step-by-step process on compiling libcurl in
  Visual Studio or Unix
  
* For Visual Studio, open StockScraper.sln

* You can compile to 32bit or 64bit, just make sure you use the
  correct cURL library

* Edit the post-build events to your specific needs. The
  post-build event simply copies the MSVC and libcurl ```.dll```
  libraries to the output (build) folder

##### Dependencies:
* [cURL](https://curl.haxx.se/)

### Contribution Guidleines:
Try not to upload machine-specific changes to the
 ```.sln``` file or the ```.vcxproj``` file.

- - - 

### To Do:
* Download entire set of historical prices, instead of just the
  first page
* Scrape as many ticker symbols as you can from Yahoo! Finance
    - Navigate through pages?
* Exception for invalid ticker symbol
* Check for maximum number of threads on computer
* Implement CMake file

### Completed:
* Scrapes the first page of historical data for a stock
  from Yahoo! Finance.
* Reads a .txt file in order to select ticker symbols
* Saves data to individual files
* Implements multi-threading