#ifndef APPLICATIONUTILITY_H
#define APPLICATIONUTILITY_H

#include <QObject>

class ApplicationUtility : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationUtility(QObject *parent = 0);

    Q_INVOKABLE QString fileObjectPathToPath(const QString &path);
    Q_INVOKABLE QString fileObjectPathToFileName(const QString &path);

    Q_INVOKABLE QString sizeToStr(int size);

    Q_INVOKABLE QString formatDate(int date);

    ///
    /// \brief bytesPerSecond
    /// Format download bytes/sec to string like 10.0KB/s
    /// \param value
    /// \return
    ///
    Q_INVOKABLE QString bytesPerSecond(int value);

    ///
    /// \brief downloadPercent
    /// Format download size to percent string
    /// \param ready
    /// \param total
    /// \return
    ///
    Q_INVOKABLE QString downloadPercent(int ready, int total);

    Q_INVOKABLE QString milliSecsToStr(qint64 millseconds);

    Q_INVOKABLE void copyToClipboard(const QString &text);
};

#endif // APPLICATIONUTILITY_H
