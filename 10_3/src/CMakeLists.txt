add_library(StudentDatabase SHARED
        StudentDatabase.cpp
        StudentDatabase.h
        StudentDatabaseCApi.cpp
        StudentDatabaseCApi.h
        StudentDatabaseWrapper.cpp
        StudentDatabaseWrapper.h
)

target_include_directories(StudentDatabase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Указываем флаг для C-совместимости
target_compile_definitions(StudentDatabase PUBLIC STUDENT_DATABASE_C_API)
