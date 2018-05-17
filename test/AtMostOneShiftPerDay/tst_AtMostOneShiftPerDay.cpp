#include "../../src/AtMostOneShiftPerDay.h"

#include <QTest>

class tst_AtMostOneShiftPerDay : public QObject
{
    Q_OBJECT

private slots:
    void constrain();

};

void tst_AtMostOneShiftPerDay::constrain()
{

}

QTEST_APPLESS_MAIN(tst_AtMostOneShiftPerDay)
#include "tst_AtMostOneShiftPerDay.moc"
