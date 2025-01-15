#include <windows.h>
#include <cstdlib>
#include <ctime>

// Глобальные переменные
// избавится
COLORREF ellipseColor = RGB(0, 0, 255); // Начальный цвет эллипса

// Прототипы функций
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Генерация случайного цвета
COLORREF GetRandomColor()
{
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

//hInstance — уникальный идентификатор экземпляра приложения. Используется для загрузки ресурсов (иконок, курсоров и т.д.).
//nCmdShow — флаг отображения окна (свернутое, развернутое и т.д.).
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    // что такое оконная процедура
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc; // Указываем функцию обработки сообщений
    wc.hInstance = hInstance; // Идентификатор приложения
    wc.lpszClassName = TEXT("EllipseApp"); // Имя класса окна
    wc.hbrBackground = (HBRUSH)(COLOR_BTNSHADOW + 1); // Цвет фона окна
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    // Регистрируем класс окна в системе
    if (!RegisterClass(&wc))
    {
        MessageBox(nullptr, TEXT("Error when registration window class!"), TEXT("Error"), MB_OK);
        return 1;
    }

    // Создаём окно
    HWND hWnd = CreateWindow
        (
            wc.lpszClassName,   // Имя класса окна
            TEXT("My cool app!!!"), // Заголовок окна
            WS_OVERLAPPEDWINDOW, // Стиль окна (с кнопками управления)
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // Размер и положение окна
            nullptr,
            nullptr,
            hInstance,
            nullptr
        );

    if (!hWnd)
    {
        MessageBox(nullptr, TEXT("Error when creating window!"), TEXT("Error"), MB_OK);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {};

    //связь с конкретным окном обеспечивается через дескриптор окна (HWND), который является частью механизма обработки сообщений.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        //вернуть translate
        //После получения сообщения функция DispatchMessage(&msg) вызывает оконную процедуру (WndProc) для конкретного окна, указанного в msg.hwnd
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HMENU hMenu;
    static RECT clientRect;

    switch (message)
    {
        case WM_CREATE:
        {
            // Создаём меню
            hMenu = CreateMenu();

            // Меню File
            HMENU hFileMenu = CreateMenu();
            AppendMenu(hFileMenu, MF_STRING, 1, TEXT("Exit"));
            AppendMenu(hFileMenu, MF_STRING, 2, TEXT("Say HI!!"));
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, TEXT("File"));

            // Меню Help
            HMENU hHelpMenu = CreateMenu();
            AppendMenu(hHelpMenu, MF_STRING, 3, TEXT("About"));
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, TEXT("Help"));

            // Устанавливаем меню
            SetMenu(hWnd, hMenu);
            break;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                //не использовать магические числа
                case 1: // Пункт меню Exit
                //какое сообщение окну когда нажимаем крестик
                    PostQuitMessage(0);
                    break;

                case 2: // Пункт меню Say HI!!
                    MessageBox(nullptr, TEXT("It's me Mario!"), TEXT("hello!"), MB_OK);
                    break;

                case 3: // Пункт меню About
                    MessageBox(nullptr, TEXT("Grisha's app"), TEXT("About"), MB_OK);
                    break;
            }
            break;
        }

        case WM_SIZE:
        {
            GetClientRect(hWnd, &clientRect);
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        }

        case WM_LBUTTONDOWN:
        {
            int x = LOWORD(lParam); // Координата X клика
            int y = HIWORD(lParam); // Координата Y клика

            // Центр и размеры эллипса
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;

            int centerX = clientRect.left + width / 2;
            int centerY = clientRect.top + height / 2;

            int a = width / 2; // Горизонтальный радиус
            int b = height / 2; // Вертикальный радиус

            // Проверка попадания в эллипс
            double normalizedX = static_cast<double>(x - centerX) / a;
            double normalizedY = static_cast<double>(y - centerY) / b;

            if (normalizedX * normalizedX + normalizedY * normalizedY <= 1.0)
            {
                ellipseColor = GetRandomColor();
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            HBRUSH hBrush = CreateSolidBrush(ellipseColor);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

            // Рисуем эллипс
            Ellipse(hdc, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);

            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);
            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY:
            //вопрос, почему если не вызвать  PostQuitMessage программа не завершит работу
            // какое сообщение штется окну когда нажимается крестик
            // алерт при закрытии приложения
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
