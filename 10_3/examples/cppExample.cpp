#include "StudentDatabaseWrapper.h"
#include <iostream>

void PrintStudents(const std::vector<StudentC>& students) {
    for (const auto& student : students) {
        std::cout << "ID: " << student.id
                  << ", Name: " << student.name
                  << ", Specialty: " << student.specialty
                  << ", Year: " << student.enrollmentYear << std::endl;
    }
}

int main() {
    try {
        StudentDatabaseWrapper db;

        db.addStudent(1, "Alice", "Computer Science", 2018);
        db.addStudent(2, "Bob", "Mechanical Engineering", 2019);
        db.addStudent(3, "Alice", "Electrical Engineering", 2020);

        std::cout << "All Students:" << std::endl;
        PrintStudents(db.getAllStudents());

        std::cout << "\nStudents named Alice:" << std::endl;
        PrintStudents(db.getStudentsByName("Alice"));

        db.removeStudent(1);
        std::cout << "\nRemoved student with ID 1." << std::endl;

        std::cout << "\nAll students after removal:" << std::endl;
        PrintStudents(db.getAllStudents());

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}