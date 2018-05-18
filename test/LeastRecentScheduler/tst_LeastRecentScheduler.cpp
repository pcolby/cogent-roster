#include "../../src/LeastRecentScheduler.h"

#include <QTest>

typedef Cogent::SchedulerInterface::QStringSet QStringSet;

class tst_LeastRecentScheduler : public QObject
{
    Q_OBJECT

private slots:
    void chooseNextNurse_data();
    void chooseNextNurse();
};

void tst_LeastRecentScheduler::chooseNextNurse_data()
{
    QTest::addColumn<QStringList>("seedNurses");
    QTest::addColumn<QStringSet>("nurses");
    QTest::addColumn<QString>("expected");

    const QStringList alice { QStringLiteral("Alice") };
    const QStringList bob   { QStringLiteral("Bob")   };
    const QStringList aliceThenBob{ QStringLiteral("Alice"), QStringLiteral("Bob") };
    const QStringList none;

    QTest::newRow("no-nurses") << QStringList() << QStringSet() << QString();

    QTest::newRow("never-seen-before") << QStringList() << alice.toSet() << alice.first();

    QTest::newRow("only-seen-bob-before") << bob << alice.toSet() << alice.first();

    QTest::newRow("seen-alice-before") << alice << alice.toSet() << alice.first();

    QTest::newRow("seen-both-only-alice-available") << aliceThenBob << alice.toSet() << alice.first();

    QTest::newRow("seen-both-only-bob-available")   << aliceThenBob << bob.toSet()   << bob.first();

    QTest::newRow("saw-alice-first") << aliceThenBob << aliceThenBob.toSet() << alice.first();
}

void tst_LeastRecentScheduler::chooseNextNurse()
{
    QFETCH(QStringList, seedNurses);
    QFETCH(QStringSet, nurses);
    QFETCH(QString, expected);

    Cogent::LeastRecentScheduler scheduler;

    // Seed the scheduler with the test nurses, in explicit order.
    foreach (const QString &nurse, seedNurses) {
        scheduler.chooseNextNurse(QStringSet{nurse});
    }

    // Check the scheduler returns the expected nurse.
    QCOMPARE(scheduler.chooseNextNurse(nurses), expected);
}

// Let QTest know how to format QStringSet values (via QDebug, which already supports QSet).
namespace QTest {
    template<> char *toString(const QStringSet &value)
    {
        QString string;
        QDebug debug(&string);
        debug << value;
        return qstrdup(string.toLocal8Bit().data());
    }
};

QTEST_APPLESS_MAIN(tst_LeastRecentScheduler)
#include "tst_LeastRecentScheduler.moc"
