#include "../../src/RosterGenerator.h"

#include <QTest>

class tst_RosterGenerator : public QObject
{
    Q_OBJECT

private slots:
    void generate();

};

void tst_RosterGenerator::generate()
{
    Cogent::RosterGenerator generator;
    /// @todo
}

QTEST_APPLESS_MAIN(tst_RosterGenerator)
#include "tst_RosterGenerator.moc"
