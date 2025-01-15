#include "StudentDatabase.h"
#include <iostream>

void PrintStudents(IStudentIterator &iterator)
{
    while (iterator.HasNext())
    {
        const Student &student = iterator.Next();
        std::cout << "ID: " << student.id
                  << ", Name: " << student.name
                  << ", Specialty: " << student.specialty
                  << ", Year: " << student.enrollmentYear << std::endl;
    }
}

int main()
{
    StudentDatabase db;

    // Add students
    db.addStudent({1, "Alice", "Computer Science", 2018});
    db.addStudent({2, "Bob", "Mechanical Engineering", 2019});
    db.addStudent({3, "Alice", "Electrical Engineering", 2020});

    // Iterate through all students
    auto allStudentsIterator = db.GetAllStudents();
    std::cout << "All Students:" << std::endl;
    PrintStudents(*allStudentsIterator);

    // Iterate through students named "Alice"
    auto aliceStudentsIterator = db.GetStudentsByName("Alice");
    std::cout << "\nStudents named Alice:" << std::endl;
    PrintStudents(*aliceStudentsIterator);

    return 0;
}