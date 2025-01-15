#include "StudentDatabaseCApi.h"
#include "StudentDatabase.h"
#include <cstring>
#include <vector>

extern "C" {

void *createStudentDatabase()
{
    return new StudentDatabase();
}

void destroyStudentDatabase(void *db)
{
    delete static_cast<StudentDatabase *>(db);
}

int addStudentC(void *db, const StudentC *student)
{
    try
    {
        auto *database = static_cast<StudentDatabase *>(db);
        database->addStudent({
                                     student->id,
                                     student->name,
                                     student->specialty,
                                     student->enrollmentYear
                             });
        return 0; // Success
    } catch (...)
    {
        return -1; // Error
    }
}

int removeStudentC(void *db, unsigned int id)
{
    try
    {
        auto *database = static_cast<StudentDatabase *>(db);
        database->removeStudent(id);
        return 0; // Success
    } catch (...)
    {
        return -1; // Error
    }
}

int getAllStudentsC(void *db, StudentC *results, size_t *count)
{
    try
    {
        auto *database = static_cast<StudentDatabase *>(db);
        auto iterator = database->GetAllStudents();

        size_t i = 0;
        while (iterator->HasNext() && i < *count)
        {
            const Student &student = iterator->Next();
            results[i].id = student.id;
            results[i].name = student.name.c_str();
            results[i].specialty = student.specialty.c_str();
            results[i].enrollmentYear = student.enrollmentYear;
            ++i;
        }

        *count = i;
        return 0; // Success
    } catch (...)
    {
        return -1; // Error
    }
}

int getStudentsByNameC(void *db, const char *name, StudentC *results, size_t *count)
{
    try
    {
        auto *database = static_cast<StudentDatabase *>(db);
        auto iterator = database->GetStudentsByName(name);

        size_t i = 0;
        while (iterator->HasNext() && i < *count)
        {
            const Student &student = iterator->Next();
            results[i].id = student.id;
            results[i].name = student.name.c_str();
            results[i].specialty = student.specialty.c_str();
            results[i].enrollmentYear = student.enrollmentYear;
            ++i;
        }

        *count = i;
        return 0; // Success
    } catch (...)
    {
        return -1; // Error
    }
}

} // extern "C"