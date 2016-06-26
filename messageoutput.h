#include <QMessageLogContext>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QDateTime>



static QTextStream* logStream = nullptr;
static QFile* logFile = nullptr;

void installLog();
void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);
void finishLog();



void installLog()
{
    if (logStream || logFile)
        finishLog();

    auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    logFile = new QFile(path + "/log.txt");
    if (!logFile->open(QFile::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
    {
        qDebug("Could not open log file");
        return;
    }

    logStream = new QTextStream(logFile);

#ifdef Q_WS_WIN
    logStream->setCodec("Windows-1251");
#else
    logStream->setCodec("utf-8");
#endif

    *logStream << QString("Runned at %1.").arg(QDateTime::currentDateTime().toString()) << endl;

    qInstallMessageHandler(myMessageOutput);

    qDebug("Success opening log file");
}



void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    if (!logStream)
        return;

    QByteArray localMsg = msg.toLocal8Bit();
    QString typeString;
    switch (type) {
    case QtDebugMsg:
        typeString = "Debug";
        break;
    case QtInfoMsg:
        typeString = "Info";
        break;
    case QtWarningMsg:
        typeString = "Wrng";
        break;
    case QtCriticalMsg:
        typeString = "Crtcl";
        break;
    case QtFatalMsg:
        typeString = "Fatal";
        abort();
    }

    auto text = QString("%1:\t%2 (%3:%4, %5)\n").arg(typeString).arg(localMsg.constData())
            .arg(context.file).arg(context.line).arg(context.function);

    *logStream << text;
}



void finishLog()
{
    *logStream << "Success closing log file" << endl << endl;

    delete logStream;
    logStream = nullptr;
    delete logFile;
    logFile = nullptr;

    qInstallMessageHandler(nullptr);
}
