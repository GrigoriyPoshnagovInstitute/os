#include "StudentDatabaseWrapper.h"
#include <cstring>

StudentDatabaseWrapper::StudentDatabaseWrapper() {
    db = createStudentDatabase();
    if (!db) {
        throw std::runtime_error("Failed to create student database.");
    }
}

StudentDatabaseWrapper::~StudentDatabaseWrapper() {
    if (db) {
        destroyStudentDatabase(db);
    }
}

void StudentDatabaseWrapper::addStudent(unsigned int id, const std::string& name, const std::string& specialty, int enrollmentYear) {
    StudentC student = {id, name.c_str(), specialty.c_str(), enrollmentYear};
    if (addStudentC(db, &student) != 0) {
        throw std::runtime_error("Failed to add student.");
    }
}

void StudentDatabaseWrapper::removeStudent(unsigned int id) {
    if (removeStudentC(db, id) != 0) {
        throw std::runtime_error("Failed to remove student.");
    }
}

std::vector<StudentC> StudentDatabaseWrapper::getAllStudents() const {
    std::vector<StudentC> students;
    StudentC buffer[100]; // Буфер для студентов
    size_t count = 100;

    if (getAllStudentsC(db, buffer, &count) == 0) {
        students.assign(buffer, buffer + count);
    } else {
        throw std::runtime_error("Failed to get all students.");
    }

    return students;
}

std::vector<StudentC> StudentDatabaseWrapper::getStudentsByName(const std::string& name) const {
    std::vector<StudentC> students;
    StudentC buffer[100];
    size_t count = 100;

    if (getStudentsByNameC(db, name.c_str(), buffer, &count) == 0) {
        students.assign(buffer, buffer + count);
    } else {
        throw std::runtime_error("Failed to get students by name.");
    }

    return students;
}