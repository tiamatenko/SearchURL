#include "scannodesmodel.h"

#include "scannode.h"

ScanNodesModel::ScanNodesModel(QObject *parent)
    : QObject(parent)
{
}

ScanNodesModel::~ScanNodesModel()
{
    resume();
}

void ScanNodesModel::start(const QUrl &url, int maxThreadCount, const QString &searchText, int maxDocCount)
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

    for (auto node : m_nodes)
        node->deleteLater();

    m_nodes.clear();

    createNode(url);
    emit nodesChanged(m_nodes);
}

void ScanNodesModel::stop()
{
    resume();
    emit stopped();
}

void ScanNodesModel::pause()
{
    if (m_paused)
        return;
    m_paused = true;
    m_pauseLock.lockForWrite();
    emit paused();
}

void ScanNodesModel::resume()
{
    if (m_paused) {
        m_paused = false;
        m_pauseLock.unlock();
        emit resumed();
    }
}

void ScanNodesModel::handleUrls(const QList<QUrl> &urls)
{
    int oldCount = m_nodes.count();
    for (const auto &url : urls) {
        if (m_foundUrls.count() >= m_maxDocCount)
            break;
        if (!m_foundUrls.contains(url))
            createNode(url);
    }
    if (m_nodes.count() != oldCount)
        emit nodesChanged(m_nodes);
}

void ScanNodesModel::createNode(const QUrl &url)
{
    m_foundUrls.insert(url);

    ScanNode *node = new ScanNode(url, m_searchText, &m_pauseLock, this);
    m_nodes << node;
    connect(this, &ScanNodesModel::stopped, node, &ScanNode::stop);
    connect(this, &ScanNodesModel::paused, node, &ScanNode::pause);
    connect(node, &ScanNode::urlsFound, this, &ScanNodesModel::handleUrls);
    connect(node, &ScanNode::finished, this, [this] {
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

void ScanNodesModel::setMaxThreadCount(int maxThreadCount)
{
    if (m_maxThreadCount != maxThreadCount) {
        m_maxThreadCount = maxThreadCount;
        emit maxThreadCountChanged(m_maxThreadCount);
    }
}

void ScanNodesModel::setSearchText(const QString &searchText)
{
    if (m_searchText != searchText) {
         m_searchText = searchText;
         emit searchTextChanged(m_searchText);
    }
}

void ScanNodesModel::setMaxDocCount(int maxDocCount)
{
    if (m_maxDocCount != maxDocCount) {
         m_maxDocCount = maxDocCount;
         emit maxDocCountChanged(m_maxDocCount);
    }
}

void ScanNodesModel::setScannedDocs(int scannedDocs)
{
     if (m_scannedDocs != scannedDocs) {
         m_scannedDocs = scannedDocs;
         emit scannedDocsChanged(m_scannedDocs);
     }
}
