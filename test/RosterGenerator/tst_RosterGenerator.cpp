#include "../../src/AtMostFiveConsecutiveDays.h"
#include "../../src/AtMostFiveNightShiftsPerMonth.h"
#include "../../src/AtMostOneShiftPerDay.h"
#include "../../src/NoSingleDaysOff.h"
#include "../../src/RosterGenerator.h"

#include <QTest>

class tst_RosterGenerator : public QObject
{
    Q_OBJECT

private slots:
    void generate_data();
    void generate();
};

void tst_RosterGenerator::generate_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("days");
    QTest::addColumn<QStringList>("nurses");

    // Load the nurses from the test data.
    QFile file(QFINDTESTDATA("../../data/nurses.txt"));
    QVERIFY(file.open(QFile::ReadOnly|QFile::Text));
    const QStringList nurses = // Note, the list.toSet().toList() is for lazy uniqueness.
        QString::fromLocal8Bit(file.readAll()).split(QLatin1Char('\n')).toSet().toList();
    QCOMPARE(nurses.size(), 98); // Ensure we loaded all 98 unique nurese from the file.

    // Test cases with a generous number of nurses.
    QTest::newRow("28-days-98-nurses") << 2018 << 02 << 28 << nurses;
    QTest::newRow("29-days-98-nurses") << 2020 << 02 << 29 << nurses;
    QTest::newRow("30-days-98-nurses") << 2018 << 06 << 30 << nurses;
    QTest::newRow("31-days-98-nurses") << 2018 << 05 << 31 << nurses;

    // Test cases with only just enough nurses.
    QTest::newRow("28-days-28-nurses") << 2018 << 02 << 28 << QStringList(nurses.mid(0, 34));
    QTest::newRow("29-days-29-nurses") << 2020 << 02 << 29 << QStringList(nurses.mid(0, 34));
    QTest::newRow("30-days-30-nurses") << 2018 << 06 << 30 << QStringList(nurses.mid(0, 34));
    QTest::newRow("31-days-31-nurses") << 2018 << 05 << 31 << QStringList(nurses.mid(0, 34));
}

void tst_RosterGenerator::generate()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, days);
    QFETCH(QStringList, nurses);

    // Setup a generator instance.
    Cogent::RosterGenerator generator;
    generator.addConstraint(new Cogent::AtMostFiveConsecutiveDays());
    generator.addConstraint(new Cogent::AtMostFiveNightShiftsPerMonth(QObject::tr("night")));
    generator.addConstraint(new Cogent::AtMostOneShiftPerDay());
    generator.addConstraint(new Cogent::NoSingleDaysOff());

    // Check that insufficient nurses results in a (graceful) empty variant map.
    // Note we trim the nurses list to just 1, since one nurse can never satisfy any constraints.
    QCOMPARE(generator.generate(year, month, nurses.mid(0,1)), QVariantMap());

    // Generate the roster.
    QVariantMap roster = generator.generate(year, month, nurses);

    // Check the roster contains the right month, with the expected number of days.
    const QString monthKey = QStringLiteral("%1-%2").arg(year).arg(month, 2, 10,QLatin1Char('0'));
    QVERIFY(roster.contains(monthKey));
    const QVariantList monthRoster = roster.value(monthKey).toList();
    QCOMPARE(monthRoster.size(), days);

    // Check constraints.
}

QTEST_APPLESS_MAIN(tst_RosterGenerator)
#include "tst_RosterGenerator.moc"
