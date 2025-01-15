#include <iostream>
#include <dlfcn.h>

typedef bool (*SolveQuadraticFunc)(double, double, double, double *, double *);

int main()
{
    void *handle = dlopen("./lib10_2_lib.so", RTLD_LAZY);
    if (!handle)
    {
        std::cerr << "Failed to load shared library: " << dlerror() << "\n";
        return 1;
    }

    auto SolveQuadratic = (SolveQuadraticFunc) dlsym(handle, "SolveQuadratic");
    if (!SolveQuadratic)
    {
        std::cerr << "Failed to locate function in shared library: " << dlerror() << "\n";
        dlclose(handle);
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

    // Закрываем библиотеку
    dlclose(handle);
    return 0;
}
