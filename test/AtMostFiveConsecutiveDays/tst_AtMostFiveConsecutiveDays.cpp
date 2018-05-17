#include "../../src/AtMostFiveConsecutiveDays.h"

#include <QTest>

typedef Cogent::ConstraintInterface::QStringSet QStringSet;

class tst_AtMostFiveConsecutiveDays : public QObject
{
    Q_OBJECT

private slots:
    void constrain_data();
    void constrain();
};

void tst_AtMostFiveConsecutiveDays::constrain_data()
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

    QTest::newRow("alice-rostered-once")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("alice-rostered-twice")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("alice-rostered-three-times")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("alice-rostered-four-times")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("alice-rostered-five-times")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << bob << 1;

    QTest::newRow("alice-rostered-ten-times")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << bob << 1;

    QTest::newRow("both-already-rostered-five-times")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(aliceAndBob.toList()) } },
               QVariantMap{ { QStringLiteral("evening"), QVariant(aliceAndBob.toList()) } },
               QVariantMap{ { QStringLiteral("night"),   QVariant(aliceAndBob.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(aliceAndBob.toList()) } },
               QVariantMap{ { QStringLiteral("evening"), QVariant(aliceAndBob.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << none << 2;
}

void tst_AtMostFiveConsecutiveDays::constrain()
{
    QFETCH(QVariantList, days);
    QFETCH(QStringSet, nurses);
    QFETCH(QStringSet, expected);
    QFETCH(int, removed);

    Cogent::AtMostFiveConsecutiveDays constraint;
    QCOMPARE(constraint.constrain(nurses, QString(), days), removed);
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

QTEST_APPLESS_MAIN(tst_AtMostFiveConsecutiveDays)
#include "tst_AtMostFiveConsecutiveDays.moc"
