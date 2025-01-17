#ifndef STUDENT_DATABASE_C_API_H
#define STUDENT_DATABASE_C_API_H

#include <stddef.h> // Добавлено для size_t

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned int id;
    const char *name;
    const char *specialty;
    int enrollmentYear;
} StudentC;

EXPORT void *createStudentDatabase();
EXPORT void destroyStudentDatabase(void *db);

EXPORT int addStudentC(void *db, const StudentC *student);
EXPORT int removeStudentC(void *db, unsigned int id);

EXPORT int getAllStudentsC(void *db, StudentC *results, size_t *count);
EXPORT int getStudentsByNameC(void *db, const char *name, StudentC *results, size_t *count);

#ifdef __cplusplus
}
#endif

#endif // STUDENT_DATABASE_C_API_H