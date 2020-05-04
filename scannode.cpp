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
    connect(m_loader, &QNetworkAccessManager::authenticationRequired, this, [=] (QNetworkReply *, QAuthenticator *) {
        handleError(tr("Authentication Required"));
    });
}

void ScanNode::start(const QUrl &url)
{
    m_reply = m_loader->get(QNetworkRequest(url.isValid() ? url : m_url));
    m_reply->ignoreSslErrors();
    connect(m_reply, &QNetworkReply::finished, this, &ScanNode::httpFinished);
    connect(m_reply, &QIODevice::readyRead, this, &ScanNode::httpReadyRead);
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
    if (scanStatus() != Downloading) {
        m_reply->deleteLater();
        m_reply = nullptr;
        return;
    }

    const QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    m_reply->deleteLater();
    m_reply = nullptr;

    if (!redirectionTarget.isNull()) {
        const QUrl redirectedUrl = m_url.resolved(redirectionTarget.toUrl());
        start(redirectedUrl);
    }
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

void ScanNode::setScanStatus(ScanNode::ScanStatus status)
{
    if (m_scanStatus != status) {
        m_scanStatus = status;
        emit scanStatusChanged(status);
    }
}

void ScanNode::handleError(const QString &error)
{
    setScanStatus(ScanStatus::Error);

    emit errorOccurred(error);
    m_reply->abort();
}
