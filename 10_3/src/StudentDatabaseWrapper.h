#ifndef STUDENT_DATABASE_WRAPPER_H
#define STUDENT_DATABASE_WRAPPER_H

#include "StudentDatabaseCApi.h"
#include "StudentDatabaseWrapper.h"
#include <string>
#include <vector>
#include <stdexcept>

class StudentDatabaseWrapper
{
public:
    StudentDatabaseWrapper();
    ~StudentDatabaseWrapper();

    void addStudent(unsigned int id, const std::string &name, const std::string &specialty, int enrollmentYear);

    void removeStudent(unsigned int id);

    std::vector<StudentC> getAllStudents() const;

    std::vector<StudentC> getStudentsByName(const std::string &name) const;

private:
    void *db;
};

#endif // STUDENT_DATABASE_WRAPPER_H