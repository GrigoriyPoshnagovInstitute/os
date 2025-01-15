#include <cmath>
#include <stdexcept>

extern "C" __declspec(dllexport) bool SolveQuadratic(double a, double b, double c, double *root1, double *root2)
{
    if (a == 0.0)
    {
        throw std::invalid_argument("Coefficient 'a' cannot be zero.");
    }

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0.0)
    {
        return false;
    }

    double sqrtDiscriminant = std::sqrt(discriminant);
    *root1 = (-b + sqrtDiscriminant) / (2 * a);
    *root2 = (-b - sqrtDiscriminant) / (2 * a);
    return true;
}
