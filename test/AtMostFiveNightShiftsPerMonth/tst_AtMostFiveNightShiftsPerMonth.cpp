#include "../../src/AtMostFiveNightShiftsPerMonth.h"

#include <QTest>

class tst_AtMostFiveNightShiftsPerMonth : public QObject
{
    Q_OBJECT

private slots:
    void constrain();

};

void tst_AtMostFiveNightShiftsPerMonth::constrain()
{

}

QTEST_APPLESS_MAIN(tst_AtMostFiveNightShiftsPerMonth)
#include "tst_AtMostFiveNightShiftsPerMonth.moc"
