#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

class PhrasesFile // obiekt typu RAII -> Rescource Aquisition Is Initialization
{
public:
    PhrasesFile()
    {
        std::lock_guard<std::mutex> lock(PhrasesFile::mu);
        OpenFile("phrases.txt");
    }

    PhrasesFile(std::string filename)
    {
        std::lock_guard<std::mutex> lock(PhrasesFile::mu);
        OpenFile(filename);
    }

    void ReadPhrases(std::vector<std::string>& Phrases)
    {
        Phrases.clear();
        if (file)
        {
            std::string line;
            while (!file.eof())
            {
                getline(file, line);
                Phrases.push_back(line);
            }
            file.clear();
            file.seekg(0, std::ios::beg);
        }
    }

    ~PhrasesFile()
    {
        std::lock_guard<std::mutex> lock(PhrasesFile::mu);
        file.close();
    }

private:
    std::mutex mu;
    std::ifstream file;

    void OpenFile(std::string filename)
    {
        file.open(filename, std::ios::in);
        if (!file)
        {
            std::cerr << "Could not open file for reading." << std::endl;
        }
    }
};