#include "../../src/AtMostOneShiftPerDay.h"

#include <QTest>

typedef Cogent::ConstraintInterface::QStringSet QStringSet;

class tst_AtMostOneShiftPerDay : public QObject
{
    Q_OBJECT

private slots:
    void constrain_data();
    void constrain();
};

void tst_AtMostOneShiftPerDay::constrain_data()
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

    QTest::newRow("alice-already-rostered")
        << QVariantList{ QVariantMap{ { QStringLiteral("morning"), QVariant(alice.toList()) } } }
        << aliceAndBob << bob << 1;

    QTest::newRow("bob-already-rostered")
        << QVariantList{ QVariantMap{ { QStringLiteral("evening"), QVariant(bob.toList()) } } }
        << aliceAndBob << alice << 1;

    QTest::newRow("both-already-rostered")
        << QVariantList{ QVariantMap{ { QStringLiteral("night"), QVariant(aliceAndBob.toList()) } } }
        << aliceAndBob << none << 2;
}

void tst_AtMostOneShiftPerDay::constrain()
{
    QFETCH(QVariantList, days);
    QFETCH(QStringSet, nurses);
    QFETCH(QStringSet, expected);
    QFETCH(int, removed);

    Cogent::AtMostOneShiftPerDay constraint;
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

QTEST_APPLESS_MAIN(tst_AtMostOneShiftPerDay)
#include "tst_AtMostOneShiftPerDay.moc"
