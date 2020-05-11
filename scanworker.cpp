#include "scanworker.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QRegularExpression>
#include <QTextCursor>
#include <QTextDocument>
#include <QReadWriteLock>

ScanWorker::ScanWorker(QReadWriteLock *pauseLock, QObject *parent)
    : QObject(parent)
    , m_pauseLock(pauseLock)
    , m_loader(new QNetworkAccessManager(this))
{
    m_loader->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    connect(m_loader, &QNetworkAccessManager::authenticationRequired, this, [=] (QNetworkReply *, QAuthenticator *) {
        emit errorOccurred(tr("Authentication Required"));
        m_reply->close();
    });
}

void ScanWorker::start(const QUrl &url, const QString &searchText)
{
    if (m_reply) {
        qWarning() << Q_FUNC_INFO << "Worker thread is already running";
        return;
    }
    m_searchText = searchText;
    m_reply = m_loader->get(QNetworkRequest(url));
    m_reply->ignoreSslErrors();
    connect(m_reply, &QNetworkReply::finished, this, &ScanWorker::httpFinished);
    connect(m_reply, &QIODevice::readyRead, this, &ScanWorker::httpReadyRead);
    connect(m_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError error) {
        emit errorOccurred(tr("Network error %1").arg(error));
    });
}

void ScanWorker::stop()
{
    if (m_reply)
        m_reply->abort();
}

void ScanWorker::pause()
{
    QReadLocker lock(m_pauseLock);
}


void ScanWorker::httpReadyRead()
{
    const QString &html = m_reply->readAll();
    QTextDocument doc;
    doc.setHtml(html);
    QTextCursor cursor = doc.find(m_searchText);

    static const QRegularExpression re(QStringLiteral("https?:\\/\\/(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,4}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)"),
                                                      QRegularExpression::InvertedGreedinessOption);

    QSet<QUrl> urls;
    QRegularExpressionMatchIterator it = re.globalMatch(html);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        const QUrl &url = match.captured();
        urls << url;
    }
    emit finished(!cursor.isNull(), urls.toList());
}

void ScanWorker::httpFinished()
{
    m_reply->deleteLater();
    m_reply = nullptr;
    emit connectionClosed();
}
