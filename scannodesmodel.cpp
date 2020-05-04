#include "scannodesmodel.h"

#include "scannode.h"

ScanNodesModel::ScanNodesModel(QObject *parent)
    : QObject(parent)
{
}

void ScanNodesModel::start(const QUrl &url, uint maxThreadCount, const QString &searchText, uint maxDocCount)
{
    m_maxThreadCount = maxThreadCount;
    m_searchText = searchText;
    m_maxDocCount = maxDocCount;
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

    node->start();
}