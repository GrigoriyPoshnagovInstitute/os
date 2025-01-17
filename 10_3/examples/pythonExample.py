import ctypes
from ctypes import POINTER, Structure, c_char_p, c_int, c_uint, byref

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
lib.addStudentC.argtypes = [ctypes.c_void_p, POINTER(StudentC)]
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
        students_to_add = [
            StudentC(1, b"John Doe", b"Computer Science", 2020),
            StudentC(2, b"Jane Smith", b"Mathematics", 2021),
            StudentC(3, b"John Doe", b"Physics", 2019),
        ]

        for student in students_to_add:
            result = lib.addStudentC(db, byref(student))
            if result != 0:
                print(f"Failed to add student with ID {student.id}")

        # Find students by name
        print("Students named 'John Doe':")
        count = c_int(0)
        students = (StudentC * 10)()  # Buffer for up to 10 students
        if lib.findStudentsByNameC(db, b"John Doe", students, byref(count)) == 0:
            for i in range(count.value):
                print(f"ID: {students[i].id}, Name: {students[i].name.decode()}, Specialty: {students[i].specialty.decode()}, Year: {students[i].enrollmentYear}")
        else:
            print("Failed to find students by name.")

        # Find student by ID
        student_id_to_find = 2
        student_ptr = lib.findStudentByIdC(db, student_id_to_find)
        if student_ptr:
            student = student_ptr.contents
            print(f"\nStudent with ID {student_id_to_find}: Name: {student.name.decode()}, Specialty: {student.specialty.decode()}, Year: {student.enrollmentYear}")
        else:
            print(f"\nStudent with ID {student_id_to_find} not found.")

        # Remove a student
        student_id_to_remove = 1
        if lib.removeStudentC(db, student_id_to_remove) == 0:
            print(f"\nRemoved student with ID {student_id_to_remove}.")
        else:
            print(f"\nFailed to remove student with ID {student_id_to_remove}.")

    finally:
        lib.destroyStudentDatabase(db)