#include "scannode.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QRegularExpression>
#include <QTextCursor>
#include <QTextDocument>

ScanNode::ScanNode(const QUrl &url, const QString &text, QObject *parent)
    : QObject(parent)
    , m_url(url)
    , m_searchText(text)
    , m_loader(new QNetworkAccessManager(this))
{
    m_loader->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    connect(m_loader, &QNetworkAccessManager::authenticationRequired, this, [=] (QNetworkReply *, QAuthenticator *) {
        handleError(tr("Authentication Required"));
    });
}

void ScanNode::start(const QUrl &url)
{
    setScanStatus(ScanStatus::Loading);
    m_reply = m_loader->get(QNetworkRequest(url.isValid() ? url : m_url));
    m_reply->ignoreSslErrors();
    connect(m_reply, &QNetworkReply::finished, this, &ScanNode::httpFinished);
    connect(m_reply, &QIODevice::readyRead, this, &ScanNode::httpReadyRead);
    connect(m_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &ScanNode::handleNetworkError);
}

void ScanNode::stop()
{
    setScanStatus(ScanStatus::NotFound);
    m_reply->abort();
}

void ScanNode::pause()
{
    // TODO
}

void ScanNode::resume()
{
    // TODO
}

void ScanNode::httpFinished()
{
    if (scanStatus() == ScanStatus::Loading)
        setScanStatus(ScanStatus::Error);

    m_reply->deleteLater();
    m_reply = nullptr;
}

void ScanNode::httpReadyRead()
{
    const QString &html = m_reply->readAll();
    QTextDocument doc;
    doc.setHtml(html);
    QTextCursor cursor = doc.find(m_searchText);
    setScanStatus(cursor.isNull() ? ScanStatus::NotFound : ScanStatus::Found);

    QSet<QUrl> urls;

    static const QRegularExpression re(QStringLiteral("https?:\\/\\/(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{1,256}\\.[a-zA-Z0-9()]{1,6}\\b([-a-zA-Z0-9()@:%_\\+.~#?&//=]*)"),
                                                      QRegularExpression::InvertedGreedinessOption);
    QRegularExpressionMatchIterator it = re.globalMatch(html);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        const QUrl &url = match.captured();
        urls << url;
    }
    if (!urls.isEmpty())
        emit urlsFound(urls.toList());
}

void ScanNode::handleNetworkError(int error)
{
    handleError(tr("Network error is occurred: %1").arg(error));
}

void ScanNode::setScanStatus(ScanNode::ScanStatus status)
{
    if (m_scanStatus != status) {
        m_scanStatus = status;
        emit scanStatusChanged(status);
    }
}

void ScanNode::handleError(const QString &error)
{
    m_errorString = error;
    emit errorStringChanged(error);

    setScanStatus(ScanStatus::Error);

    m_reply->abort();
}
