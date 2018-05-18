#include "../../src/NoSingleDaysOff.h"

#include <QTest>

typedef Cogent::ConstraintInterface::QStringSet QStringSet;

class tst_NoSingleDaysOff : public QObject
{
    Q_OBJECT

private slots:
    void constrain_data();
    void constrain();
};

void tst_NoSingleDaysOff::constrain_data()
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

    QTest::newRow("one-day-on")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("one-day-off")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant() } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("two-days-off")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(none.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(none.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("one-off-one-on")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(none.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("one-on-one-off")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(none.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << bob << 1; // Alice needs another day off.

    QTest::newRow("two-days-on")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;

    QTest::newRow("both-two-days-on")
        << QVariantList{
               QVariantMap{ { QStringLiteral("morning"), QVariant(aliceAndBob.toList()) } },
               QVariantMap{ { QStringLiteral("morning"), QVariant(aliceAndBob.toList()) } },
               QVariantMap()
           }
        << aliceAndBob << aliceAndBob << 0;
}

void tst_NoSingleDaysOff::constrain()
{
    QFETCH(QVariantList, days);
    QFETCH(QStringSet, nurses);
    QFETCH(QStringSet, expected);
    QFETCH(int, removed);

    Cogent::NoSingleDaysOff constraint;
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

QTEST_APPLESS_MAIN(tst_NoSingleDaysOff)
#include "tst_NoSingleDaysOff.moc"
