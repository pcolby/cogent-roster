#include "../../src/AtMostFiveNightShiftsPerMonth.h"

#include <QTest>

typedef Cogent::ConstraintInterface::QStringSet QStringSet;

class tst_AtMostFiveNightShiftsPerMonth : public QObject
{
    Q_OBJECT

private slots:
    void constrain_data();
    void constrain();
};

void tst_AtMostFiveNightShiftsPerMonth::constrain_data()
{
    QTest::addColumn<QVariantList>("days");
    QTest::addColumn<QStringSet>("nurses");
    QTest::addColumn<QStringSet>("expected");
    QTest::addColumn<int>("removed");

    const QStringSet alice { QStringLiteral("Alice") };
    const QStringSet bob   { QStringLiteral("Bob")   };
    const QStringSet aliceAndBob{ QStringLiteral("Alice"), QStringLiteral("Bob") };
    const QStringSet none;

    QTest::newRow("no-nurses")
        << QVariantList{ QVariant() } << QStringSet() << QStringSet() << 0;

    QTest::newRow("noone-rostered-yet")
        << QVariantList{ QVariant() } << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("one-night")
        << QVariantList{
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("two-nights")
        << QVariantList{
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("three-nights")
        << QVariantList{
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("four-nights")
        << QVariantList{
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("five-nights")
        << QVariantList{
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << bob << 1;

    QTest::newRow("ten-nights")
        << QVariantList{
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << bob << 1;

    QTest::newRow("six-days-four-nights")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"),   QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"),   QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"),   QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("night"),   QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("both-already-rostered-five-nights")
        << QVariantList{
               QVariantMap{ { QStringLiteral("night"), QVariant(aliceAndBob.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(aliceAndBob.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(aliceAndBob.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(aliceAndBob.toList()) } },
               QVariantMap{ { QStringLiteral("night"), QVariant(aliceAndBob.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << none << 2;
}

void tst_AtMostFiveNightShiftsPerMonth::constrain()
{
    QFETCH(QVariantList, days);
    QFETCH(QStringSet, nurses);
    QFETCH(QStringSet, expected);
    QFETCH(int, removed);

    Cogent::AtMostFiveNightShiftsPerMonth constraint(QStringLiteral("night"));

    {   // When for non-night shifts, the constrain should remove no-one.
        QStringSet tempNurses = nurses;
        QCOMPARE(constraint.constrain(tempNurses, QString(), days), 0);
        QCOMPARE(tempNurses, nurses);
        QCOMPARE(constraint.constrain(tempNurses, QStringLiteral("morning"), days), 0);
        QCOMPARE(tempNurses, nurses);
    }

    QCOMPARE(constraint.constrain(nurses, QStringLiteral("night"), days), removed);
    QCOMPARE(nurses, expected);
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

QTEST_APPLESS_MAIN(tst_AtMostFiveNightShiftsPerMonth)
#include "tst_AtMostFiveNightShiftsPerMonth.moc"
