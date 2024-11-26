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
        file.open("phrases.txt", std::ios::in);
    }

    PhrasesFile(string filename)
    {
        std::lock_guard<std::mutex> lock(PhrasesFile::mu);
        file.open(filename, std::ios::in);
    }

    void ReadPhrases(std::vector<std::string>& Phrases)
    {

    }

    ~PhrasesFile()
    {
        std::lock_guard<std::mutex> lock(PhrasesFile::mu);
        file.close();
    }

private:
    static std::mutex mu;
    std::fstream file;
}