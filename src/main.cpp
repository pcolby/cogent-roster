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

void configureGenerator(Cogent::RosterGenerator &generator, const QCommandLineParser &parser);
void configureLogging(const QCommandLineParser &parser);
QStringList readNursesList(const QCommandLineParser &parser);
bool writeToJson(const QVariantMap &roster, const QCommandLineParser &parser);

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
        QStringLiteral("YYYY MM"),
        QStringLiteral("Month to produce roster for, in either ISO 8601 format such as 'YYYY-MM'"));
    parser.process(app);
    configureLogging(parser);

    // Fetcth the year / month.
    if (parser.positionalArguments().size() != 2) {
        parser.showHelp(EXIT_FAILURE);
    }
    const int year = parser.positionalArguments().first().toInt();
    const int month = parser.positionalArguments().last().toInt();
    if ((month < 1) || (month > 12)) {
        qCritical() << "month must be between 1 and 12 inclusive";
        return EXIT_FAILURE;
    }

    // Read the nurses list.
    const QStringList nurses = readNursesList(parser);
    if (nurses.isEmpty()) {
        qCritical() << "have no nurses to roster";
        return EXIT_FAILURE;
    }

    // Generate the roster.
    Cogent::RosterGenerator generator;
    configureGenerator(generator, parser);
    const QVariantMap roster = generator.generate(year, month, nurses);
    if (roster.isEmpty()) {
        return EXIT_FAILURE;
    }

    // Output the roster in JSON format.
    return (writeToJson(roster, parser)) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*!
 * Configure the given \a generator according the command line \a parser
 */
void configureGenerator(Cogent::RosterGenerator &generator, const QCommandLineParser &parser)
{
    if (!parser.isSet(QStringLiteral("no-c1")))
        generator.addConstraint(new Cogent::AtMostFiveConsecutiveDays());
    if (!parser.isSet(QStringLiteral("no-c2")))
        generator.addConstraint(new Cogent::AtMostFiveNightShiftsPerMonth(QObject::tr("night")));
    if (!parser.isSet(QStringLiteral("no-c3")))
        generator.addConstraint(new Cogent::AtMostOneShiftPerDay());
    if (!parser.isSet(QStringLiteral("no-c4")))
        generator.addConstraint(new Cogent::NoSingleDaysOff());
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
        qDebug() << "reading nurses list from" << parser.value(QStringLiteral("nurses"));
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

/*!
 * Converts \a roster to JSON, and writes it to file or stdout according to the options in
 * \a parser.
 *
 * Returns \c true on success; \c false otherwise.
 */
bool writeToJson(const QVariantMap &roster, const QCommandLineParser &parser)
{
    // Open the file (or stdout) for writing.
    QFile file(parser.value(QStringLiteral("output")));
    if (parser.isSet(QStringLiteral("output"))) {
        qDebug() << "writing roster to" << parser.value(QStringLiteral("output"));
        if (!file.open(QFile::WriteOnly|QFile::Text)) {
            qCritical() << "failed to open" << parser.value(QStringLiteral("output")) << "for writing";
            return false;
        }
    } else {
        qDebug() << "writing to roster stdout";
        if (!file.open(stdout, QFile::WriteOnly|QFile::Text)) {
            qCritical() << "failed to open stdout for writing";
            return false;
        }
    }

    // Convert the roster to JSON, and write it to file.
    const QJsonDocument::JsonFormat jsonFormat = parser.isSet(QStringLiteral("compact"))
        ? QJsonDocument::Compact : QJsonDocument::Indented;
    const QByteArray json = QJsonDocument::fromVariant(roster).toJson(jsonFormat);
    const int bytesWritten = file.write(json);
    if (bytesWritten != json.size()) {
        qCritical() << "failed to write JSON to file";
        return false;
    }
    return true;
}
