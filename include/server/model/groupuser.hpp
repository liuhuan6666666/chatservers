#ifndef GROUPUSER_H
#define GROUPUSER_H
#include "user.hpp"
#include <string>
class GroupUser : public User
{
public:
    void setRole(string role) { this->_role = role; }
    string getRole() { return this->_role; }

private:
    string _role;
};

#endif