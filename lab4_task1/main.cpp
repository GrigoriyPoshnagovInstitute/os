#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <filesystem>
#include <thread>

char MyToUpper(char ch)
{
    return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
}

char MyToLower(char ch)
{
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}


//ввыяснить если функция потока выкинет исключение которое не будет аерехвачено
//программа завершится с вызовом std::terminate()
void FlipCase(const std::string &inputFilename)
{
    try
    {
        std::string outputFilename = inputFilename + ".out";

        std::ifstream inputFile(inputFilename, std::ios::in);
        std::ofstream outputFile(outputFilename, std::ios::out);
        //прочитать одокументацию и сделать по феншую
        char ch;
        while (inputFile.get(ch))
        {
            if (std::islower(static_cast<unsigned char>(ch)))
            {
                ch = MyToUpper(ch);
            } else if (std::isupper(static_cast<unsigned char>(ch)))
            {
                ch = MyToLower(ch);
            }
            outputFile.put(ch);
        }
        std::cout << "File: '" << inputFilename << "' successfully flipped!" << std::endl;
    } catch (const std::exception &e)
    {
        std::cerr << "Error processing file '" << inputFilename << "': " << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: flip-case-mt <input_file1> <input_file2> ..." << std::endl;
        return 1;
    }
    //узнать как именно выдделяютяс ресурсы в рамказ одного процнсса
// нормально объяснить где выполняются поткои
// Потоки (в рамках одного процесса) делят одну и ту же область памяти и ресурсы (например, открытые файлы)
// В C++ потоки (или "threads") выполняются в контексте процесса, который может содержать один или несколько потоков.

    std::vector<std::jthread> threads;

    for (int i = 1; i < argc; ++i)
    {
        //выяснить почему этот код работаетесли Алексей Николаевич привел аргументы, почему оно не должно работать
        std::string inputFilename = argv[i];
        threads.emplace_back(FlipCase, inputFilename);
    }

    return 0;
}
