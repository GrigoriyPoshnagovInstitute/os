import ctypes
from ctypes import POINTER, Structure, c_char_p, c_int, c_uint

class StudentC(Structure):
    _fields_ = [
        ("id", c_uint),
        ("name", c_char_p),
        ("specialty", c_char_p),
        ("enrollmentYear", c_int),
    ]

# Load the shared library
lib = ctypes.CDLL("./libStudentDatabase.dll")

# Create and destroy database
lib.createStudentDatabase.restype = ctypes.c_void_p
lib.destroyStudentDatabase.argtypes = [ctypes.c_void_p]

# Add and remove students
lib.addStudentC.argtypes = [ctypes.c_void_p, StudentC]
lib.addStudentC.restype = ctypes.c_int

lib.removeStudentC.argtypes = [ctypes.c_void_p, c_uint]
lib.removeStudentC.restype = ctypes.c_int

# Find students
lib.findStudentsByNameC.argtypes = [ctypes.c_void_p, c_char_p, POINTER(StudentC), POINTER(c_int)]
lib.findStudentsByNameC.restype = ctypes.c_int

lib.findStudentByIdC.argtypes = [ctypes.c_void_p, c_uint]
lib.findStudentByIdC.restype = POINTER(StudentC)

# Example usage
if __name__ == "__main__":
    db = lib.createStudentDatabase()

    try:
        # Add students
        lib.addStudentC(db, StudentC(1, b"John Doe", b"Computer Science", 2020))
        lib.addStudentC(db, StudentC(2, b"Jane Smith", b"Mathematics", 2021))
        lib.addStudentC(db, StudentC(3, b"John Doe", b"Physics", 2019))

        # Find students by name
        print("Students named 'John Doe':")
        count = c_int(0)
        students = (StudentC * 10)()
        if lib.findStudentsByNameC(db, b"John Doe", students, ctypes.byref(count)) == 0:
            for i in range(count.value):
                print(f"ID: {students[i].id}, Name: {students[i].name.decode()}, Specialty: {students[i].specialty.decode()}, Year: {students[i].enrollmentYear}")

        # Find student by ID
        student = lib.findStudentByIdC(db, 2)
        if student:
            print(f"\nStudent with ID 2: Name: {student.contents.name.decode()}, Specialty: {student.contents.specialty.decode()}, Year: {student.contents.enrollmentYear}")

        # Remove a student
        lib.removeStudentC(db, 1)
        print("\nRemoved student with ID 1.")
    finally:
        lib.destroyStudentDatabase(db)
