#ifndef STUDENT_DATABASE_C_H
#define STUDENT_DATABASE_C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct
{
    unsigned int id;
    const char *name;
    const char *specialty;
    int enrollmentYear;
} StudentC;

void *createStudentDatabase();
void destroyStudentDatabase(void *db);

int addStudentC(void *db, const StudentC *student);
int removeStudentC(void *db, unsigned int id);
int findStudentByIdC(void *db, unsigned int id, StudentC *result);
int findStudentsByNameC(void *db, const char *name, StudentC *results, size_t *count);

#ifdef __cplusplus
}
#endif

#endif // STUDENT_DATABASE_C_H
