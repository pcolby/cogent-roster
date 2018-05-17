#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QLoggingCategory>
#include <QVariantList>

void configureLogging(const QCommandLineParser &parser);

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
        {{QStringLiteral("c"), QStringLiteral("compact")}, QStringLiteral("Use compact output")},
        { QStringLiteral("no-color"), QStringLiteral("Do not color the output")},
        {{QStringLiteral("n"), QStringLiteral("nurses")},
          QStringLiteral("Read names of available nurses from file (default is stdin)"),
          QStringLiteral("file") },
        {{QStringLiteral("o"), QStringLiteral("output")},
          QStringLiteral("Write output to file (default is stdout)"),
          QStringLiteral("file")},
    });
    parser.addPositionalArgument(
        QStringLiteral("YYYY-MM"),
        QStringLiteral("Month to produce roster for, in either ISO 8601 format such as 'YYYY-MM'"));
    parser.process(app);
    configureLogging(parser);

    auto roster = QVariantMap(); /// @todo Invoke scheduler.

    qInfo().noquote() << QJsonDocument::fromVariant(roster).toJson(
        parser.isSet(QStringLiteral("compact")) ? QJsonDocument::Compact : QJsonDocument::Indented);

    return EXIT_SUCCESS;
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
