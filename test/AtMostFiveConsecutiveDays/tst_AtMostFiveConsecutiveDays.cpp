#include "../../src/AtMostFiveConsecutiveDays.h"

#include <QTest>

class tst_AtMostFiveConsecutiveDays : public QObject
{
    Q_OBJECT

private slots:
    void constrain();

};

void tst_AtMostFiveConsecutiveDays::constrain()
{
    Cogent::AtMostFiveConsecutiveDays constraint;
    /// @todo
}

QTEST_APPLESS_MAIN(tst_AtMostFiveConsecutiveDays)
#include "tst_AtMostFiveConsecutiveDays.moc"
