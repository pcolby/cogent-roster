#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QLoggingCategory>
#include <iostream>

#include "AtMostFiveConsecutiveDays.h"
#include "AtMostFiveNightShiftsPerMonth.h"
#include "AtMostOneShiftPerDay.h"
#include "NoSingleDaysOff.h"
#include "RosterGenerator.h"

using namespace Cogent;

void configureLogging(const QCommandLineParser &parser);
QStringList readNursesList(const QCommandLineParser &parser);

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Parse the command line options.
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Generate nursing rosters"));
    parser.addHelpOption();
    parser.addOptions({
        {{QStringLiteral("d"), QStringLiteral("debug")}, QStringLiteral("Enable debug output")},
        { QStringLiteral("no-color"), QStringLiteral("Do not color the output")},
        { QStringLiteral("no-c1"),    QStringLiteral("Skip constraint 1 (AtMostFiveConsecutiveDays)")},
        { QStringLiteral("no-c2"),    QStringLiteral("Skip constraint 2 (AtMostFiveNightShiftsPerMonth)")},
        { QStringLiteral("no-c3"),    QStringLiteral("Skip constraint 3 (AtMostOneShiftPerDay)")},
        { QStringLiteral("no-c4"),    QStringLiteral("Skip constraint 4 (NoSingleDaysOff)")},
        {{QStringLiteral("c"), QStringLiteral("compact")}, QStringLiteral("Use compact output")},
        { QStringLiteral("no-color"), QStringLiteral("Do not color the output")},
        {{QStringLiteral("i"), QStringLiteral("nurses")},
          QStringLiteral("Read names of available nurses from file (default is stdin)"),
          QStringLiteral("file") },
        {{QStringLiteral("o"), QStringLiteral("output")},
          QStringLiteral("Write output to file (default is stdout)"),
          QStringLiteral("file")},
        { QStringLiteral("skip-dups"), QStringLiteral("Skip duplicate nurse names")},
    });
    parser.addPositionalArgument(
        QStringLiteral("YYYY-MM"),
        QStringLiteral("Month to produce roster for, in either ISO 8601 format such as 'YYYY-MM'"));
    parser.process(app);
    configureLogging(parser);

    // Read the nurses list.
    const QStringList nurses = readNursesList(parser);
    if (nurses.isEmpty()) {
        qCritical() << "have no nurses to roster";
        return EXIT_FAILURE;
    }

    // Generate the roster.
    Cogent::RosterGenerator generator;
    if (!parser.isSet(QStringLiteral("no-c1")))
        generator.addConstraint(new Cogent::AtMostFiveConsecutiveDays());
    if (!parser.isSet(QStringLiteral("no-c2")))
        generator.addConstraint(new Cogent::AtMostFiveNightShiftsPerMonth(QObject::tr("night")));
    if (!parser.isSet(QStringLiteral("no-c3")))
        generator.addConstraint(new Cogent::AtMostOneShiftPerDay());
    if (!parser.isSet(QStringLiteral("no-c4")))
        generator.addConstraint(new Cogent::NoSingleDaysOff());
    const QVariantMap roster = generator.generate(2018,5,nurses);

    // Convert the roster to JSON, and print it to stdout.
    const QJsonDocument::JsonFormat jsonFormat = parser.isSet(QStringLiteral("compact"))
        ? QJsonDocument::Compact : QJsonDocument::Indented;
    std::cout << QJsonDocument::fromVariant(roster).toJson(jsonFormat).toStdString();
    return roster.isEmpty() ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*!
 * Configure application logging based on the command line \a parser
 */
void configureLogging(const QCommandLineParser &parser)
{
    // Start with the Qt default message pattern (see qtbase:::qlogging.cpp:defaultPattern)
    QString messagePattern = QStringLiteral("%{if-category}%{category}: %{endif}%{message}");

    if (parser.isSet(QStringLiteral("debug"))) {
        messagePattern.prepend(QStringLiteral("%{time process} %{type} %{function} "));
        QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
    }

    if (!parser.isSet(QStringLiteral("no-color"))) {
        messagePattern.prepend(QStringLiteral(
        "%{if-warning}\x1b[35m%{endif}" // Magenta
        "%{if-critical}\x1b31m%{endif}" // Red
        "%{if-fatal}31;1%{endif}"));    // Red and bold
        messagePattern.append(QStringLiteral("\x1b[0m"));
    }

    qSetMessagePattern(messagePattern);
}

/*!
 * Returns a list of nurses read from either a file or stdin, according to the options in \a parser.
 *
 * \note This function assumes the file (including stdin) uses the current system's local 8-bit
 * encoding. Typically, this is UTF-8, but it not guaranteed. In future, we should support BOM
 * detection for UTF-16, and/or a command line option to specify the encoding manually.
 */
QStringList readNursesList(const QCommandLineParser &parser)
{
    // Open the input file (or stdin) for reading.
    QFile file(parser.value(QStringLiteral("nurses")));
    if (parser.isSet(QStringLiteral("nurses"))) {
        if (!file.open(QFile::ReadOnly|QFile::Text)) {
            qCritical() << "failed to open" << parser.value(QStringLiteral("nurses")) << "for reading";
            return QStringList();
        }
    } else {
        qDebug() << "reading nurses list from stdin";
        if (!file.open(stdin, QFile::ReadOnly|QFile::Text)) {
            qCritical() << "failed to open stdin for reading";
            return QStringList();
        }
    }

    // Read all nurses from the input file (or stdin).
    QSet<QString> nurses;
    while (!file.atEnd()) {
        QString nurse = QString::fromLocal8Bit(file.readLine().trimmed());
        if (!parser.isSet(QStringLiteral("skip-dups"))) {
            // If (while) we already have a nurse with this name, append the nurse's line number
            // to their name. Note, the use of 'while' instead of 'if' is rarely necessary, but it
            // does guarantee uniqueness (in case another nurse's name ends in a number).
            while (nurses.contains(nurse)) {
                nurse += QStringLiteral(" (%1)").arg(nurses.size()+1);
            }
        }
        nurses.insert(nurse);
    }
    qDebug() << "read" << nurses.size() << "nurses";
    return nurses.toList();
}
