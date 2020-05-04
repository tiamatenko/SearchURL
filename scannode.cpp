#include "scannode.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QRegularExpression>
#include <QTextCursor>
#include <QTextDocument>

uint ScanNode::m_maxThreadCount = 5;
QString ScanNode::m_text;
uint ScanNode::m_maxDocCount = 20;
QSet<QUrl> ScanNode::m_foundLinks;

ScanNode::ScanNode(ScanNode *parent)
    : QObject(parent)
    , m_loader(new QNetworkAccessManager(this))
{
    connect(m_loader, &QNetworkAccessManager::authenticationRequired, this, [=] (QNetworkReply *, QAuthenticator *) {
        handleError(tr("Authentication Required"));
    });
    connect(this, &ScanNode::scanStopped, this, &ScanNode::stop);
    connect(this, &ScanNode::scanPaused, this, &ScanNode::pause);
    connect(this, &ScanNode::scanResumed, this, &ScanNode::resume);
}

void ScanNode::startScan(const QUrl &link, uint maxThreadCount, const QString &text, uint maxDocCount)
{
    m_maxThreadCount = maxThreadCount;
    m_text = text;
    m_maxDocCount = maxDocCount;
    m_foundLinks.clear();

    for (auto node : m_childNodes)
        node->deleteLater();

    m_childNodes.clear();
    emit childNodesChanged(m_childNodes);

    start(link);
}

void ScanNode::stopScan()
{
    emit scanStopped();
}

void ScanNode::pauseScan()
{
    emit scanPaused();
}

void ScanNode::resumeScan()
{
    emit scanResumed();
}

void ScanNode::start(const QUrl &link)
{
    m_link = link;
    emit linkChanged(link);

    m_reply = m_loader->get(QNetworkRequest(link));
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
        const QUrl redirectedUrl = m_link.resolved(redirectionTarget.toUrl());
        start(redirectedUrl);
    }
}

void ScanNode::httpReadyRead()
{
    const QString &html = m_reply->readAll();
    QTextDocument doc;
    doc.setHtml(html);
    QTextCursor cursor = doc.find(m_text);
    setScanStatus(cursor.isNull() ? ScanStatus::NotFound : ScanStatus::Found);

    static const QRegularExpression re(QStringLiteral("^http:\\/\\/[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(:[0-9]{1,5})?(\\/.*)?$"));
    QRegularExpressionMatchIterator it = re.globalMatch(html);
    while (it.hasNext() && uint(m_foundLinks.size()) <= m_maxDocCount) {
        QRegularExpressionMatch match = it.next();
        const QUrl &link = match.captured(1);
        if (!m_foundLinks.contains(link)) {
            m_foundLinks.insert(link);
            ScanNode *childNode = new ScanNode(this);
            m_childNodes << childNode;
            childNode->start(link);
        }
    }
    if (!m_childNodes.isEmpty())
        emit childNodesChanged(m_childNodes);
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
//    m_reply->deleteLater();
//    m_reply = nullptr;
}
