#include "StudentDatabase.h"
#include <stdexcept>
#include <vector>

void StudentDatabase::addStudent(const Student &student)
{
    if (studentsById.find(student.id) != studentsById.end())
    {
        throw std::runtime_error("Student with this ID already exists.");
    }
    studentsById[student.id] = student;
    studentsByName[student.name][student.id] = &studentsById[student.id];
}

void StudentDatabase::removeStudent(unsigned int id)
{
    auto it = studentsById.find(id);
    if (it == studentsById.end())
    {
        throw std::runtime_error("Student with this ID does not exist.");
    }
    auto name = it->second.name;
    studentsByName[name].erase(id);
    if (studentsByName[name].empty())
    {
        studentsByName.erase(name);
    }
    studentsById.erase(it);
}

class ConcreteStudentIterator : public IStudentIterator
{
public:
    // конструктор для all students
    ConcreteStudentIterator(const std::unordered_map<unsigned int, Student> &map)
            : students(), current(0)
    {
        for (const auto &pair: map)
        {
            students.push_back(&pair.second);
        }
        end = students.size();
    }

    // конструктор для students by name
    ConcreteStudentIterator(const std::unordered_map<unsigned int, Student *> &map)
            : students(), current(0)
    {
        for (const auto &pair: map)
        {
            students.push_back(pair.second);
        }
        end = students.size();
    }

    bool HasNext() const override
    {
        return current != end;
    }

    const Student &Next() override
    {
        if (!HasNext())
        {
            throw std::out_of_range("No more students.");
        }
        return *students[current++];
    }

private:
    std::vector<const Student *> students;
    size_t current;
    size_t end;
};

std::unique_ptr<IStudentIterator> StudentDatabase::GetAllStudents() const
{
    return std::make_unique<ConcreteStudentIterator>(studentsById);
}

std::unique_ptr<IStudentIterator> StudentDatabase::GetStudentsByName(const std::string &name) const
{
    auto it = studentsByName.find(name);
    if (it == studentsByName.end())
    {
        return std::make_unique<ConcreteStudentIterator>(std::unordered_map<unsigned int, Student *>());
    }
    return std::make_unique<ConcreteStudentIterator>(it->second);
}