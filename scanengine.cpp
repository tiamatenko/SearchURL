#include "scanengine.h"

#include "scannode.h"

#include <QThread>

ScanEngine::ScanEngine(QObject *parent)
    : QObject(parent)
    , m_maxThreadCount(QThread::idealThreadCount())
{
}

ScanEngine::~ScanEngine()
{
    resume();
}

void ScanEngine::start(const QUrl &url, const QString &searchText, int maxDocCount, int maxThreadCount)
{
    setMaxThreadCount(maxThreadCount);
    setSearchText(searchText);
    setScannedDocs(0);
    setMaxDocCount(maxDocCount);
    m_foundUrls.clear();
    m_activeNodeCount = 0;
    emit activeThreadCountChanged(0);
    m_deferredNodes.clear();
    emit deferredThreadCountChanged(0);

    m_foundDocCount = 0;
    emit foundDocCountChanged(m_foundDocCount);

    m_notFoundDocCount = 0;
    emit notFoundDocCountChanged(m_notFoundDocCount);

    m_errorDocCount = 0;
    emit errorDocCountChanged(m_errorDocCount);

    for (auto node : m_nodes)
        node->deleteLater();

    m_nodes.clear();

    setState(State::Running);
    createNode(url);
    emit nodesChanged(m_nodes);
}

void ScanEngine::stop()
{
    resume();
    emit stopped();
    m_activeNodeCount = 0;
    emit activeThreadCountChanged(0);
    m_deferredNodes.clear();
    emit deferredThreadCountChanged(0);
    setState(State::Stopped);
}

void ScanEngine::pause()
{
    if (state() != State::Running)
        return;
    setState(State::Paused);
    m_pauseLock.lockForWrite();
    emit paused();
}

void ScanEngine::resume()
{
    if (state() == State::Paused) {
        m_pauseLock.unlock();
        setState(State::Running);
    }
}

void ScanEngine::handleUrls(const QList<QUrl> &urls)
{
    if (state() == State::Stopped)
        return;
    int oldCount = m_nodes.count();
    for (const auto &url : urls) {
        if (m_foundUrls.count() >= m_maxDocCount) {
            setState(State::Stopped);
            break;
        }
        if (!m_foundUrls.contains(url))
            createNode(url);
    }
    if (m_nodes.count() != oldCount)
        emit nodesChanged(m_nodes);
}

void ScanEngine::setState(ScanEngine::State newState)
{
    if (state() != newState) {
        m_state = newState;
        emit stateChanged();
    }
}

void ScanEngine::createNode(const QUrl &url)
{
    m_foundUrls.insert(url);

    ScanNode *node = new ScanNode(url, m_searchText, &m_pauseLock, this);
    m_nodes << node;
    connect(this, &ScanEngine::stopped, node, &ScanNode::stop);
    connect(this, &ScanEngine::paused, node, &ScanNode::pause);
    connect(node, &ScanNode::urlsFound, this, &ScanEngine::handleUrls);
    connect(node, &ScanNode::finished, this, [=] {
        switch (node->scanStatus()) {
        case ScanNode::ScanStatus::Found:
            ++m_foundDocCount;
            emit foundDocCountChanged(m_foundDocCount);
            break;
        case ScanNode::ScanStatus::NotFound:
            ++m_notFoundDocCount;
            emit notFoundDocCountChanged(m_notFoundDocCount);
            break;
        case ScanNode::ScanStatus::Error:
            ++m_errorDocCount;
            emit errorDocCountChanged(m_errorDocCount);
            break;
        default:
            break;
        }

        setScannedDocs(m_scannedDocs + 1);
        if (m_deferredNodes.isEmpty()) {
            if (m_activeNodeCount > 0)
                emit activeThreadCountChanged(--m_activeNodeCount);
        } else {
            m_deferredNodes.dequeue()->start();
        }
    });

    if (m_activeNodeCount < m_maxThreadCount) {
        emit activeThreadCountChanged(++m_activeNodeCount);
        node->start();
    }
    else {
        m_deferredNodes.enqueue(node);
    }
}

void ScanEngine::setMaxThreadCount(int maxThreadCount)
{
    if (m_maxThreadCount != maxThreadCount) {
        m_maxThreadCount = maxThreadCount;
        emit maxThreadCountChanged(m_maxThreadCount);
    }
}

void ScanEngine::setSearchText(const QString &searchText)
{
    if (m_searchText != searchText) {
         m_searchText = searchText;
         emit searchTextChanged(m_searchText);
    }
}

void ScanEngine::setMaxDocCount(int maxDocCount)
{
    if (m_maxDocCount != maxDocCount) {
         m_maxDocCount = maxDocCount;
         emit maxDocCountChanged(m_maxDocCount);
    }
}

void ScanEngine::setScannedDocs(int scannedDocs)
{
     if (m_scannedDocs != scannedDocs) {
         m_scannedDocs = scannedDocs;
         emit scannedDocsChanged(m_scannedDocs);
     }
}
