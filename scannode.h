#pragma once

#include <QObject>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;

class ScanNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(ScanStatus scanStatus READ scanStatus NOTIFY scanStatusChanged)
public:
    enum ScanStatus { None, Downloading, Found, NotFound, Error };
    Q_ENUM(ScanStatus)

    ScanNode(const QUrl &url, const QString &searchText, QObject *parent = nullptr);

    inline QUrl url() const { return m_url; }
    inline ScanStatus scanStatus() const { return m_scanStatus; }

public slots:
    void start(const QUrl &url = QUrl());
    void stop();
    void pause();
    void resume();

private slots:
    void httpFinished();
    void httpReadyRead();

signals:
    void childNodesChanged(const QObjectList &childNodes);
    void htmlDocChanged(const QString &htmlDoc);
    void scanStatusChanged(ScanStatus scanStatus);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void errorOccurred(const QString &error);
    void urlsFound(const QList<QUrl> &urls);

private:
    void setScanStatus(ScanStatus status);
    void handleError(const QString &error);

private:
    const QUrl m_url;
    const QString &m_searchText;
    ScanStatus m_scanStatus = None;

    QNetworkAccessManager *m_loader;
    QNetworkReply *m_reply = nullptr;
};

