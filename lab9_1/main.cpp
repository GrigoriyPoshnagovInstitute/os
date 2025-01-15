#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <thread>
#include <mutex>
#include <Ws2tcpip.h>

#define BUFFER_SIZE 1024 // Определение размера буфера для передачи данных

void ErrorHandler(const std::string &message) {
    std::cerr << message << " Error Code: " << WSAGetLastError()
              << std::endl; // Выводим сообщение об ошибке и код ошибки
    //Никких EXIT ов
    exit(EXIT_FAILURE); // Завершаем программу с кодом ошибки
}

// Функция для парсинга чисел из строки
std::vector<int> ParseNumbers(const std::string &input) {
    std::istringstream iss(input);
    std::vector<int> numbers;
    int num;
    while (iss >> num) {
        numbers.push_back(num);
    }
    return numbers;
}

// Функция для обработки команды, полученной от клиента
std::string ProcessCommand(const std::string &command) {
    if (command.empty()) return "Error: Empty command";

    std::istringstream iss(command);
    char operation;
    iss >> operation;

    std::string numbersStr;
    std::getline(iss, numbersStr);

    auto numbers = ParseNumbers(numbersStr);
    if (numbers.empty()) return "Error: Invalid or missing numbers";

    int result = numbers[0];
    if (operation == '+') {
        for (size_t i = 1; i < numbers.size(); ++i) {
            result += numbers[i];
        }
    } else if (operation == '-') {
        for (size_t i = 1; i < numbers.size(); ++i) {
            result -= numbers[i];
        }
    } else {
        return "Error: Unsupported operation";
    }
    return std::to_string(result);
}

// Функция для обработки клиента
void HandleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];  // Буфер для получения данных от клиента
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);  // Получаем данные от клиента
        if (bytesReceived <= 0) break;  // Если получено 0 или меньше байт, завершаем соединение

        buffer[bytesReceived] = '\0';  // Завершаем строку нулевым символом
        std::string response = ProcessCommand(buffer);  // Обрабатываем команду клиента

        send(clientSocket, response.c_str(), response.size(), 0);  // Отправляем результат обратно клиенту
        //метод c_str() используется для получения указателя на C-строку (массив символов) из объекта типа std::string.
    }
    closesocket(clientSocket);  // Закрываем соединение с клиентом
}

// Функция для старта сервера
void StartServer(int port) {
    std::cout << "Starting server\n";
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {  // Инициализация библиотеки Winsock
        ErrorHandler("WSAStartup failed");  // Если ошибка при инициализации, вызываем обработчик ошибок
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);  // Создаем сокет для сервера
    //AF_INET: Указывает на семейство адресов IPv4
    //SOCK_STREAM: Это тип сокета, который используется для TCP-соединений

    if (serverSocket == INVALID_SOCKET) {  // Если сокет не удалось создать, вызываем обработчик ошибок
        ErrorHandler("Socket creation failed");
    }

    sockaddr_in serverAddr{};  // Структура для хранения адреса сервера
    serverAddr.sin_family = AF_INET;  // Используем IPv4
    // почему нельлзя просто устанвоить, зачем htons
    //htnos не устанвливает порт
    serverAddr.sin_port = htons(port);  // Устанавливаем порт
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Принимаем подключения с любого IP

    if (bind(serverSocket, (sockaddr *) &serverAddr, sizeof(serverAddr)) ==
        SOCKET_ERROR) {  // Привязываем сокет к адресу
        ErrorHandler("Bind failed");
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {  // Начинаем слушать порт
        ErrorHandler("Listen failed");
    }

    std::cout << "Server listening on port " << port << std::endl;  // Сообщаем, что сервер слушает указанный порт

    while (true) {
        sockaddr_in clientAddr;  // Структура для хранения адреса клиента
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr *) &clientAddr,
                                     &clientSize);  // Принимаем входящее соединение

        if (clientSocket == INVALID_SOCKET) {  // Если соединение не удалось установить
            std::cerr << "Accept failed" << std::endl;  // Сообщаем об ошибке
            continue;  // Пытаемся снова принять соединение
        }

        std::thread clientThread(HandleClient, clientSocket);  // Создаем новый поток для обработки клиента
        clientThread.detach();  // Отключаем поток, чтобы он работал независимо
    }
}

// Функция для старта клиента
void StartClient(const std::string &address, int port) {
    std::cout << "Starting client\n";  // Выводим сообщение о старте клиента
    //использовать RAII обертки
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {  // Инициализация библиотеки Winsock
        ErrorHandler("WSAStartup failed");
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);  // Создаем сокет для клиента
    if (clientSocket == INVALID_SOCKET) {  // Если сокет не удалось создать, вызываем обработчик ошибок
        ErrorHandler("Socket creation failed");
    }

    sockaddr_in serverAddr = {};  // Структура для хранения адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);  // Устанавливаем порт
    inet_pton(AF_INET, address.c_str(),
              &serverAddr.sin_addr);  // Преобразуем строку с адресом в формат, который понимает сокет

    if (connect(clientSocket, (sockaddr *) &serverAddr, sizeof(serverAddr)) ==
        SOCKET_ERROR) {  // Подключаемся к серверу
        ErrorHandler("Connect failed");
    }

    std::cout << "Connected to server at " << address << ":" << port << std::endl;  // Сообщаем о успешном подключении

    std::string input;
    // что произойдет если данных будет больше чем BUFFER_SIZE
    char buffer[BUFFER_SIZE];
    while (true) {
        std::cout << "> ";  // Запрашиваем ввод у пользователя
        std::getline(std::cin, input);  // Читаем команду пользователя

        if (input.empty()) continue;  // Если команда пустая, пропускаем итерацию

        send(clientSocket, input.c_str(), input.size(), 0);  // Отправляем команду на сервер

        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);  // Получаем ответ от сервера
        if (bytesReceived <= 0) break;  // Если данные не получены, выходим из цикла

        buffer[bytesReceived] = '\0';  // Завершаем строку нулевым символом
        std::cout << "Server response: " << buffer << std::endl;  // Выводим ответ сервера
    }

    closesocket(clientSocket);  // Закрываем соединение с сервером
    WSACleanup();  // Завершаем работу с библиотекой Winsock
}

// решить проблему
int main() {
    int choice;
    std::cout << "Enter 1 to start server or 2 to start client: ";
    std::cin >> choice;

    if (choice == 1) {
        int port;
        std::cout << "Enter port for server: ";
        std::cin >> port;
        StartServer(port);
    } else if (choice == 2) {
        std::string address;
        int port;
        std::cout << "Enter server address: ";
        std::cin >> address;
        std::cout << "Enter port: ";
        std::cin >> port;
        StartClient(address, port);
    }

    return 0;  // Завершаем программу
}
