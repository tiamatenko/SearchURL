#pragma once

#include <QObject>
#include <QUrl>

class QReadWriteLock;

class ScanNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(ScanStatus scanStatus READ scanStatus NOTIFY scanStatusChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
public:
    enum ScanStatus { None, Loading, Found, NotFound, Error };
    Q_ENUM(ScanStatus)

    ScanNode() = default;
    ScanNode(const QUrl &url, const QString &searchText, QReadWriteLock *pauseLock, QObject *parent = nullptr);
    ~ScanNode();

    inline QUrl url() const { return m_url; }
    inline ScanStatus scanStatus() const { return m_scanStatus; }
    inline QString errorString() const { return m_errorString; }

public slots:
    void start();
    void stop();
    void pause();

private slots:
    void scanFinished(bool foundText, const QList<QUrl> &foundUrls);
    void handleError(const QString &error);

signals:
    void requestStart(const QUrl &url, const QString &searchText);
    void requestStop();
    void requestPause();
    void finished();
    void urlsFound(const QList<QUrl> &urls);
    void scanStatusChanged(ScanStatus scanStatus);
    void errorStringChanged(const QString &errorString);

private:
    void setScanStatus(ScanStatus status);

private:
    const QUrl m_url;
    const QString m_searchText;
    ScanStatus m_scanStatus = None;
    QString m_errorString;
    QThread *m_scanThread;
};

