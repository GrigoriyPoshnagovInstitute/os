#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

//разбить на поменьще
//вертикальной палочеи не должно быть, если нет директорий
void PrintFileTree(const fs::path &dir, const std::string &prefix = "")
{
    if (!fs::exists(dir) || !fs::is_directory(dir))
    {
        std::cerr << "Error: Path does not exist or is not a directory: " << dir << '\n';
        return;
    }
    // как имя путь и название получить конечный путь
    // посмотреть вопросы в конце лабораторной
    // отделяю файлы от директорий
    std::vector<fs::directory_entry> files;
    std::vector<fs::directory_entry> directories;

    for (const auto &entry: fs::directory_iterator(dir))
    {
        // Символические ссылки: is_symlink() — объект может быть ссылкой на другой файл или директорию.
        // Файлы устройств: Например, для UNIX-систем это специальные файлы устройств (char/block device).
        // Сокеты: is_socket() — используются для межпроцессного взаимодействия.
        // FIFO/каналы: is_fifo() — для создания очередей между процессами.
        if (entry.is_regular_file())
        {
            files.push_back(entry);
        } else if (entry.is_directory())
        {
            directories.push_back(entry);
        }
    }

    // сортирую элементы по алфавиту
    auto comparator = [](const fs::directory_entry &a, const fs::directory_entry &b)
    {
        return a.path().filename() < b.path().filename();
    };
    std::sort(files.begin(), files.end(), comparator);
    std::sort(directories.begin(), directories.end(), comparator);

    // Вывожу файлы
    for (size_t i = 0; i < files.size(); ++i)
    {
        std::cout << prefix << (i == files.size() - 1 && directories.empty() ? "\\---[F] " : "|   [F] ")
                  << files[i].path().filename().string() << '\n';
    }
    //после списка файлов должна быть пустая строка
    // Вывожу директории
    for (size_t i = 0; i < directories.size(); ++i)
    {
        bool isLast = (i == directories.size() - 1);
        std::cout << prefix << (isLast ? "\\---[D] " : "+---[D] ")
                  << directories[i].path().filename().string() << '\n';
        PrintFileTree(directories[i].path(), prefix + (isLast ? "    " : "|   "));
    }
}

int main(int argc, char *argv[])
{
    try
    {
        fs::path start_dir = (argc > 1)
             ? fs::path(argv[1])
             : fs::current_path();

        std::cout << start_dir.filename().string() << '\n';
        PrintFileTree(start_dir);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
