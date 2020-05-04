#pragma once

#include <QObject>
#include <QSet>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;

class ScanNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObjectList childNodes READ childNodes NOTIFY childNodesChanged)
    Q_PROPERTY(QUrl link READ link NOTIFY linkChanged)
    Q_PROPERTY(QString htmlDoc READ htmlDoc NOTIFY htmlDocChanged)
    Q_PROPERTY(ScanStatus scanStatus READ scanStatus NOTIFY scanStatusChanged)
public:
    enum ScanStatus { None, Downloading, Found, NotFound, Error };
    Q_ENUM(ScanStatus)

    explicit ScanNode(ScanNode *parent = nullptr);

    inline QObjectList childNodes() const { return m_childNodes; }
    inline QUrl link() const { return m_link; }
    inline QString htmlDoc() const { return m_htmlDoc; }
    inline ScanStatus scanStatus() const { return m_scanStatus; }

public slots:
    void startScan(const QUrl &link, uint maxThreadCount, const QString &text, uint maxDocCount);
    void stopScan();
    void pauseScan();
    void resumeScan();

protected slots:
    void start(const QUrl &link);
    void stop();
    void pause();
    void resume();

private slots:
    void httpFinished();
    void httpReadyRead();

signals:
    void childNodesChanged(const QObjectList &childNodes);
    void linkChanged(const QUrl &link);
    void htmlDocChanged(const QString &htmlDoc);
    void scanStatusChanged(ScanStatus scanStatus);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void errorOccurred(const QString &error);

    void scanStopped();
    void scanPaused();
    void scanResumed();

private:
    void setScanStatus(ScanStatus status);
    void handleError(const QString &error);

private:
    QObjectList m_childNodes;
    QUrl m_link;
    QString m_htmlDoc;
    ScanStatus m_scanStatus = None;

    QNetworkAccessManager *m_loader;
    QNetworkReply *m_reply = nullptr;

    static uint m_maxThreadCount;
    static QString m_text;
    static uint m_maxDocCount;
    static QSet<QUrl> m_foundLinks;
};

