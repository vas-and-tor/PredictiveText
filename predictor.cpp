#include "predictor.h"
#include <QtAlgorithms>
#include <QDebug>
#include <QString>
#include <QMultiMap>

Predictor::Predictor()
{
    QFile file("../PredictiveText/dictionary.txt"); // Edit path to dictionary if needed

    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    int len;

    in >> len;
    for (int i = 0; i < len; i++) {
        QString word;
        int cnt;

        in >> word >> cnt;

        this->dictionary.push_back(qMakePair(word, cnt));
        this->low_dictionary.push_back(qMakePair(word.toLower(), cnt));
    }

    file.close();
}

QVector<QString> Predictor::predict(QString str)
{
    QVector<QPair<int,QString> > pre_result;
    QVector<QString> result;

    if (str.size() < 2) return result;

    str = str.toLower();
    QVector<QPair<QString, int> >::iterator it = qLowerBound(this->low_dictionary.begin(),
                                                 this->low_dictionary.end(),
                                                 qMakePair(str,0));

    int idx = it - this->low_dictionary.begin();

    for (int i = idx; i < this->low_dictionary.size(); i++) {
        if (this->low_dictionary[i].first.startsWith(str)) {
            pre_result.push_back(qMakePair(this->dictionary[i].second,
                                           this->dictionary[i].first));

            if (pre_result.size() > 50) break;
        } else {
            break;
        }
    }

    qSort(pre_result);

    for (int i = (int)pre_result.size()-1; i >= 0; i--) {
        result.push_back(pre_result[i].second);
    }
    return result;
}

inline int Predictor::getd(int (*D)[3], int i, int j)
{
    if (i == 0) return j;
    if (j == 0) return i;
    if (i-j > MAX_DISTANCE || j-i > MAX_DISTANCE) return MAX_DISTANCE;
    return D[i][j%3];
}

int Predictor::dist(const QString & _a, const QString & _b)
{
    const QString& a = (_a.size() >= _b.size() ? _a : _b);
    const QString& b = (_a.size() <  _b.size() ? _a : _b);

    int n = (int)a.size();
    int m = (int)b.size();
    if (n-m >= MAX_DISTANCE) return MAX_DISTANCE;

    int _D[m+1][3], (*D)[3] = _D;

    for (int j = 1; j <= n; j++) {
        for (int i = qMax(1,j-MAX_DISTANCE); i <= qMin(m,j+MAX_DISTANCE); i++) {
            D[i][j%3] = qMin(qMin(getd(D,i-1,j) + 1, getd(D,i,j-1) + 1),
                             getd(D,i-1,j-1) + (b[i-1] != a[j-1]));

            if (i > 1 && j > 1 && b[i-1] == a[j-2] && a[j-1] == b[i-2]) {
                D[i][j%3] = qMin(D[i][j%3], getd(D,i-2,j-2) + 1);
            }
        }
    }
    return D[m][n%3];
}

QVector<QString> Predictor::correct(QString str)
{
    QVector<QString> result;
    QMultiMap<QPair<int,int>, QString> mp;

    if (str.size() < 2) return result;
    str = str.toLower();

    for (int i = 0; i < this->low_dictionary.size(); i++) {
        int d = dist(str, low_dictionary[i].first);

        if (d < MAX_DISTANCE) {
            mp.insert(qMakePair(d, -dictionary[i].second), dictionary[i].first);
        }
    }

    for (QMultiMap<QPair<int,int>, QString>::const_iterator it = mp.begin(); it != mp.end(); it++) {
        result.push_back(*it);
        if (result.size() > 10) break;
    }
    return result;
}
