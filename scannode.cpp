#include "scannode.h"

#include "scanworker.h"

#include <QThread>

ScanNode::ScanNode(const QUrl &url, const QString &text, QReadWriteLock *pauseLock, QObject *parent)
    : QObject(parent)
    , m_url(url)
    , m_searchText(text)
    , m_scanThread(new QThread(this))
{
    ScanWorker *worker = new ScanWorker(pauseLock);
    worker->moveToThread(m_scanThread);
    connect(m_scanThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(m_scanThread, &QThread::finished, this, &ScanNode::finished);
    connect(this, &ScanNode::requestStart, worker, &ScanWorker::start);
    connect(this, &ScanNode::requestStop, worker, &ScanWorker::stop);
    connect(this, &ScanNode::requestPause, worker, &ScanWorker::pause);
    connect(worker, &ScanWorker::finished, this, &ScanNode::scanFinished);
    connect(worker, &ScanWorker::connectionClosed, m_scanThread, &QThread::quit);
    connect(worker, &ScanWorker::errorOccurred, this, &ScanNode::handleError);
    m_scanThread->start();
}

ScanNode::~ScanNode()
{
    m_scanThread->quit();
    m_scanThread->wait();
}

void ScanNode::start()
{
    setScanStatus(ScanStatus::Loading);
    emit requestStart(m_url, m_searchText);
}

void ScanNode::stop()
{
    emit requestStop();
}

void ScanNode::pause()
{
    emit requestPause();
}

void ScanNode::scanFinished(bool foundText, const QList<QUrl> &foundUrls)
{
    setScanStatus(foundText ? ScanStatus::Found : ScanStatus::NotFound);
    emit urlsFound(foundUrls);
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
}
