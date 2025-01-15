#include <windows.h>
#include <iostream>
#include <string>
#include <filesystem>

// RAII-обёртка для HANDLE
class HandleWrapper
{
public:
    explicit HandleWrapper(HANDLE handle) : m_handle(handle)
    {}

    ~HandleWrapper()
    {
        if (m_handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_handle);
        }
    }

    // убрал копирование
    HandleWrapper(const HandleWrapper &) = delete;


    // не хватает перемещающего опертора присваивания
    HandleWrapper &operator=(const HandleWrapper &) = delete;

    // разрешил перемещение
    HandleWrapper(HandleWrapper &&other) noexcept: m_handle(other.m_handle)
    {
        other.m_handle = INVALID_HANDLE_VALUE;
    }

    HANDLE get() const
    {
        return m_handle;
    }

private:
    HANDLE m_handle;
};

void PrintEvent(const FILE_NOTIFY_INFORMATION *notifyInfo)
{
    std::wstring fileName(notifyInfo->FileName, notifyInfo->FileNameLength / sizeof(WCHAR));
    std::wcout << L"Changed file: " << fileName << L"\n";

    switch (notifyInfo->Action)
    {
        case FILE_ACTION_ADDED:
            std::wcout << L"Event: file/dir added\n";
            break;
        case FILE_ACTION_REMOVED:
            std::wcout << L"Event: file/dir removed\n";
            break;
        case FILE_ACTION_MODIFIED:
            std::wcout << L"Event: file change\n";
            break;
        case FILE_ACTION_RENAMED_OLD_NAME:
            std::wcout << L"Event: rename (old name)\n";
            break;
        case FILE_ACTION_RENAMED_NEW_NAME:
            std::wcout << L"Event: rename (new name)\n";
            break;
        default:
            std::wcout << L"Event: unknown\n";
            break;
    }
}

void MonitorDirectory(const std::string &path)
{
    HandleWrapper directoryHandle(CreateFile(
            path.c_str(),//путь к директории в виде строки.
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE |
            FILE_SHARE_DELETE, //позволяет другим процессам читать, писать или удалять файлы в директории.
            NULL, //стандартныйе атрибуты безопасности
            OPEN_EXISTING, //  объект должен уже существовать
            FILE_FLAG_BACKUP_SEMANTICS, //позволяет открыть директорию как файл (требуется для работы с директориями).
            NULL //шаблон файла не используется.
    ));

    if (directoryHandle.get() == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error: unable to open the directory to observe!\n";
        return;
    }
    // разобраться, 1024 почему
    // проверить поведение с маленькими значениями
    char buffer[1024];
    DWORD bytesReturned;

    while (true)
    {
        if (ReadDirectoryChangesW(
                directoryHandle.get(),
                buffer,
                sizeof(buffer),
                TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_ATTRIBUTES |
                FILE_NOTIFY_CHANGE_SIZE,
                &bytesReturned,
                NULL,
                NULL
        ))
        {
            //узнать почему
            const FILE_NOTIFY_INFORMATION *notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(buffer);

            while (true)
            {
                PrintEvent(notifyInfo);
                if (notifyInfo->NextEntryOffset == 0)
                    break;
                notifyInfo = reinterpret_cast<const FILE_NOTIFY_INFORMATION *>
                (
                        reinterpret_cast<const char *>(notifyInfo) + notifyInfo->NextEntryOffset
                );
            };
        } else
        {
            std::cerr << "Error: unable to read changes in the directory!\n";
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    std::string directoryPath;

    if (argc == 2)
    {
        directoryPath = argv[1];
    } else
    {
        directoryPath = std::filesystem::current_path().string();
    }

    std::cout << "Now observing changes in: " << directoryPath << "\n";

    MonitorDirectory(directoryPath);

    return 0;
}
