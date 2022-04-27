#include "salesman.h"

salesman::salesman()
{

}
QString salesman::getaccount() const
{
    return this->account;
}
QString salesman::getpassword() const
{
    return this->password;
}
int salesman::getsID() const
{
    return this->sID;
}
void salesman::setaccount(QString ac)
{
    this->account=ac;
}
void salesman::setpassword(QString pw)
{
    this->password=pw;
}
void salesman::setsID(int num)
{
    this->sID=num;
}
