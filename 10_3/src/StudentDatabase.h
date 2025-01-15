#ifndef STUDENT_DATABASE_H
#define STUDENT_DATABASE_H

#include <string>
#include <unordered_map>
#include <memory>

struct Student
{
    unsigned int id;
    std::string name;
    std::string specialty;
    int enrollmentYear;
};

class IStudentIterator
{
public:
    virtual ~IStudentIterator() = default;

    virtual bool HasNext() const = 0;

    virtual const Student &Next() = 0;

    virtual std::unique_ptr<IStudentIterator> Clone() const = 0;
};

class StudentDatabase
{
public:
    void addStudent(const Student &student);

    void removeStudent(unsigned int id);

    // Iterator methods
    std::unique_ptr<IStudentIterator> GetAllStudents() const;

    std::unique_ptr<IStudentIterator> GetStudentsByName(const std::string &name) const;

private:
    std::unordered_map<unsigned int, Student> studentsById;
    std::unordered_map<std::string, std::unordered_map<unsigned int, Student *>> studentsByName;
};

#endif // STUDENT_DATABASE_H