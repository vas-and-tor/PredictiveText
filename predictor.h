#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <qvector.h>
#include <qstring.h>
#include <qpair.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>

class Predictor
{
private:
    QVector<QPair<QString, int> > dictionary;
    QVector<QPair<QString, int> > low_dictionary;
    static const int MAX_DISTANCE = 10;

    int dist(const QString&, const QString&);
    int getd(int (*)[3], int, int);
public:
    Predictor();
    QVector<QString> predict(QString str);
    QVector<QString> correct(QString str);
};

#endif // PREDICTOR_H
