#include "../../src/NoSingleDaysOff.h"

#include <QTest>

class tst_NoSingleDaysOff : public QObject
{
    Q_OBJECT

private slots:
    void constrain();

};

void tst_NoSingleDaysOff::constrain()
{
    Cogent::NoSingleDaysOff constraint;
    /// @todo
}

QTEST_APPLESS_MAIN(tst_NoSingleDaysOff)
#include "tst_NoSingleDaysOff.moc"
