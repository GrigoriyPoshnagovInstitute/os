#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include <iostream>
#include <thread>

const int FPS = 60;
const long FRAME_TIME = 1000000.l / FPS;
const int SNOWFLAKE_COUNT = 100;
const int MIN_SPEED = 20;
const int MAX_SPEED = 150;
const int MIN_RADIUS = 2;
const int MAX_RADIUS = 8;
// горизонтальное движение
struct Snowflake
{
    float x;
    float y;
    float radius;
    float speed;
};

void DrawSnowflake(Display *display, Window window, GC gc, const Snowflake &snowflake)
{
    XDrawArc(display, window, gc, snowflake.x - snowflake.radius, snowflake.y - snowflake.radius,
             snowflake.radius * 2, snowflake.radius * 2, 0, 360 * 64);
}

int main()
{
    //Устанавливает соединение с X сервером
    Display *display = XOpenDisplay(nullptr);
    if (!display)
    {
        std::cerr << "Failed to open X display\n";
        return 1;
    }
    //Получает номер экрана по умолчанию (если подключено несколько мониторов).
    int screen = DefaultScreen(display);
    //Получает "корневое окно" экрана, которое является базовым слоем для всех окон на экране. Все окна создаются как "дочерние" этого корневого окна.
    Window root = RootWindow(display, screen);

    int width = 1920, height = 1080;
    //3. Создание нового окна
    Window window = XCreateSimpleWindow(display, root, 0, 0, width, height, 1,
                                        BlackPixel(display, screen), BlackPixel(display, screen));
    //XMapWindow: Делает окно видимым. До вызова этой функции окно существует, но не отображается на экране.
    XMapWindow(display, window);

    //XCreateGC: Создает графический контекст (GC, Graphic Context) для окна.
    //Графический контекст определяет параметры отрисовки (цвета, шрифты, линии и т.д.).
    //Здесь создается контекст, связанный с окном, который будет использоваться для рисования.
    GC gc = XCreateGC(display, window, 0, nullptr);
    // gc нужно грохнуть

    //Инициализирует генератор случайных чисел на основе текущего времени. Это нужно для последующего создания снежинок с произвольными координатами.
    srand(time(nullptr));
    std::vector<Snowflake> snowflakes;

    for (int i = 0; i < SNOWFLAKE_COUNT; ++i)
    {
        snowflakes.push_back
        ({
            static_cast<float>(rand() % width),
            static_cast<float>(rand() % height),
            static_cast<float>(rand() % (MAX_RADIUS - MIN_RADIUS) + MIN_RADIUS),
            (float)((rand() % (MAX_SPEED - MIN_SPEED) + MAX_SPEED))
        });
    }

    auto lastTime = std::chrono::steady_clock::now();

    // разбить на подфункции
    bool running = true;
    while (running)
    {
        auto currentTime = std::chrono::steady_clock::now();

        // исопльзозвать не count а еденицы времени
        const long timeBetweenFrames = (currentTime - lastTime).count();

        if (timeBetweenFrames < FRAME_TIME)
        {
            std::this_thread::sleep_for(std::chrono::microseconds (FRAME_TIME - timeBetweenFrames));
            currentTime = std::chrono::steady_clock::now();
        }
        std::chrono::duration<float> deltaTime = currentTime - lastTime;

        lastTime = currentTime;

        // Очистка окна
        XSetForeground(display, gc, BlackPixel(display, screen));
        XFillRectangle(display, window, gc, 0, 0, width, height);

        // Рисование снежинок
        XSetForeground(display, gc, WhitePixel(display, screen));
        for (auto &snowflake : snowflakes)
        {
            DrawSnowflake(display, window, gc, snowflake);
            snowflake.y += snowflake.speed * deltaTime.count();

            if (snowflake.y > height)
            {
                snowflake.y = 0;
                snowflake.x = rand() % width;
                snowflake.radius = rand() % (MAX_RADIUS - MIN_RADIUS) + MIN_RADIUS;
                snowflake.speed = (float)((rand() % (MAX_SPEED - MIN_SPEED) + MAX_SPEED)); // Скорость в диапазоне 20-60
            }
        }
        //XFlush(display) используется в Xlib для отправки всех накопленных запросов от программы к X серверу
        XFlush(display);
    }
}
