#ifndef FORMATEXCEPTION_H
#define FORMATEXCEPTION_H

#include <exception>
#include <QString>

using namespace std;

class FormatException
{
public:
    FormatException();
    FormatException(int r,QString m):row(r),msg(m){};
    const char * what() const throw (){
        return msg.toStdString().c_str();
    }

    int getRow(){return row;}
    QString getMsg(){return msg;}

private:
    int row;
    QString msg;
};

#endif // FORMATEXCEPTION_H
