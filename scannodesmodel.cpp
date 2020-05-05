#include "scannodesmodel.h"

#include "scannode.h"

ScanNodesModel::ScanNodesModel(QObject *parent)
    : QObject(parent)
{
}

void ScanNodesModel::start(const QUrl &url, uint maxThreadCount, const QString &searchText, uint maxDocCount)
{
    setMaxThreadCount(maxThreadCount);
    setSearchText(searchText);
    setScannedDocs(0);
    setMaxDocCount(maxDocCount);
    m_foundUrls.clear();

    for (auto node : m_nodes)
        node->deleteLater();

    m_nodes.clear();

    createNode(url);
    emit nodesChanged(m_nodes);
}

void ScanNodesModel::stop()
{
    emit stopped();
}

void ScanNodesModel::pause()
{
    emit paused();
}

void ScanNodesModel::resume()
{
    emit resumed();
}

void ScanNodesModel::handleUrls(const QList<QUrl> &urls)
{
    int oldCount = m_nodes.count();
    for (const auto &url : urls) {
        if (m_foundUrls.count() >= int(m_maxDocCount))
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

    ScanNode *node = new ScanNode(url, m_searchText, this);
    m_nodes << node;
    connect(this, &ScanNodesModel::stopped, node, &ScanNode::stop);
    connect(this, &ScanNodesModel::paused, node, &ScanNode::pause);
    connect(this, &ScanNodesModel::resumed, node, &ScanNode::resume);
    connect(node, &ScanNode::urlsFound, this, &ScanNodesModel::handleUrls);
    connect(node, &ScanNode::scanFinished, this, [this] {
        setScannedDocs(m_scannedDocs + 1);
    });

    node->start();
}

void ScanNodesModel::setMaxThreadCount(uint maxThreadCount)
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

void ScanNodesModel::setMaxDocCount(uint maxDocCount)
{
    if (m_maxDocCount != maxDocCount) {
         m_maxDocCount = maxDocCount;
         emit maxDocCountChanged(m_maxDocCount);
    }
}

void ScanNodesModel::setScannedDocs(uint scannedDocs)
{
     if (m_scannedDocs != scannedDocs) {
         m_scannedDocs = scannedDocs;
         emit scannedDocsChanged(m_scannedDocs);
     }
}
