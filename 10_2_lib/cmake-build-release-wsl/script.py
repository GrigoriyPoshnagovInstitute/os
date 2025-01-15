import ctypes
from ctypes import *

dll = cdll.LoadLibrary("lib10_2_lib.so")

dll.SolveQuadratic.argtypes = [c_double, c_double, c_double, POINTER(c_double), POINTER(c_double)]
dll.SolveQuadratic.restype = c_bool

def solve_quadratic(a, b, c):
    root1 = c_double(0.0)
    root2 = c_double(0.0)

    has_roots = dll.SolveQuadratic(a, b, c, ctypes.byref(root1), ctypes.byref(root2))

    return has_roots, root1.value, root2.value

a, b, c = map(float, input("Enter coefficients a, b, c: ").split())
has_roots, root1, root2 = solve_quadratic(a, b, c)

if has_roots:
    print(f"Roots: {root1} and {root2}")
else:
    print("No real roots.")
