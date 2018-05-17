#include "../../src/LeastRecentScheduler.h"

#include <QTest>

class tst_LeastRecentScheduler : public QObject
{
    Q_OBJECT

private slots:
    void getNext();

};

void tst_LeastRecentScheduler::getNext()
{
    Cogent::LeastRecentScheduler scheduler;
    /// @todo
}

QTEST_APPLESS_MAIN(tst_LeastRecentScheduler)
#include "tst_LeastRecentScheduler.moc"
