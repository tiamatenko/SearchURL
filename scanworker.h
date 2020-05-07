#pragma once

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QReadWriteLock;

class ScanWorker : public QObject
{
    Q_OBJECT
public:
    explicit ScanWorker(QReadWriteLock *pauseLock, QObject *parent = nullptr);

public slots:
    void start(const QUrl &url, const QString &searchText);
    void stop();
    void pause();

signals:
    void finished(bool foundText, const QList<QUrl> &foundUrls);
    void connectionClosed();
    void errorOccurred(const QString &errorString);

private slots:
    void httpReadyRead();
    void httpFinished();

private:
    QReadWriteLock *m_pauseLock;
    QNetworkAccessManager *m_loader = nullptr;
    QNetworkReply *m_reply = nullptr;
    QString m_searchText;
};

