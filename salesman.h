#ifndef SALESMAN_H
#define SALESMAN_H
#include <string>
#include <QString>

using namespace std;

class salesman
{
public:
    salesman();
    salesman(long long num,QString ac,QString ps)
    {
      sID=num;
      account=ac;
      password=ps;
    };
    QString getaccount() const;
    QString getpassword() const;
    int getsID() const;
    void setaccount(QString ac);
    void setpassword(QString pw);
    void setsID(int num);
private:
    QString account;
    QString password;
    long long sID;//TODO
};

#endif // SALESMAN_H
