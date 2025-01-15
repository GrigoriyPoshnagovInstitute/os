#include <iostream>
#include <windows.h>

typedef bool (*SolveQuadraticFunc)(double, double, double, double*, double*);

int main()
{
    HMODULE hLib = LoadLibrary("lib10_1_lib.dll");
    if (!hLib)
    {
        std::cerr << "Failed to load DLL.\n";
        return 1;
    }

    auto SolveQuadratic = (SolveQuadraticFunc)GetProcAddress(hLib, "SolveQuadratic");
    if (!SolveQuadratic)
    {
        std::cerr << "Failed to locate function in DLL.\n";
        FreeLibrary(hLib);
        return 1;
    }

    double a, b, c;
    std::cout << "Enter coefficients a, b, c: ";
    std::cin >> a >> b >> c;

    double root1, root2;

    bool hasRoots = SolveQuadratic(a, b, c, &root1, &root2);

    if (hasRoots)
    {
        std::cout << "Roots: " << root1 << " and " << root2 << "\n";
    }
    else
    {
        std::cout << "No real roots.\n";
    }

    FreeLibrary(hLib);
    return 0;
}
