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
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
public:
    enum ScanStatus { None, Loading, Found, NotFound, Error };
    Q_ENUM(ScanStatus)

    ScanNode() = default;
    ScanNode(const QUrl &url, const QString &searchText, QObject *parent = nullptr);

    inline QUrl url() const { return m_url; }
    inline ScanStatus scanStatus() const { return m_scanStatus; }
    inline QString errorString() const { return m_errorString; }

public slots:
    void start(const QUrl &url = QUrl());
    void stop();
    void pause();
    void resume();

private slots:
    void httpFinished();
    void httpReadyRead();
    void handleNetworkError(int error);

signals:
    void childNodesChanged(const QObjectList &childNodes);
    void htmlDocChanged(const QString &htmlDoc);
    void scanStatusChanged(ScanStatus scanStatus);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void urlsFound(const QList<QUrl> &urls);
    void errorStringChanged(const QString &errorString);

private:
    void setScanStatus(ScanStatus status);
    void handleError(const QString &error);

private:
    const QUrl m_url;
    const QString m_searchText;
    ScanStatus m_scanStatus = None;
    QString m_errorString;

    QNetworkAccessManager *m_loader = nullptr;
    QNetworkReply *m_reply = nullptr;
};

