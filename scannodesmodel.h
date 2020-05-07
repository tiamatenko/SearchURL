#pragma once

#include <QObject>

#include <QQueue>
#include <QReadWriteLock>
#include <QSet>
#include <QUrl>

class ScanNode;

class ScanNodesModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> nodes READ nodes NOTIFY nodesChanged)
    Q_PROPERTY(QString searchText READ searchText NOTIFY searchTextChanged)
    Q_PROPERTY(int maxThreadCount READ maxThreadCount NOTIFY maxThreadCountChanged)
    Q_PROPERTY(int maxDocCount READ maxDocCount NOTIFY maxDocCountChanged)
    Q_PROPERTY(int scannedDocs READ scannedDocs NOTIFY scannedDocsChanged)
    Q_PROPERTY(int activeThreadCount READ activeThreadCount NOTIFY activeThreadCountChanged)
    Q_PROPERTY(int deferredThreadCount READ deferredThreadCount NOTIFY deferredThreadCountChanged)
public:
    explicit ScanNodesModel(QObject *parent = nullptr);
    ~ScanNodesModel();

    inline QList<QObject*> nodes() const { return m_nodes; }
    inline QString searchText() const { return m_searchText; }
    inline int maxThreadCount() const { return m_maxThreadCount; }
    inline int maxDocCount() const { return m_maxDocCount; }
    inline int scannedDocs() const { return m_scannedDocs; }
    inline int activeThreadCount() const { return m_activeNodeCount; }
    inline int deferredThreadCount() const { return m_deferredNodes.count(); }

public slots:
    void start(const QUrl &url, int maxThreadCount, const QString &searchText, int maxDocCount);
    void stop();
    void pause();
    void resume();

private slots:
    void handleUrls(const QList<QUrl> &urls);

signals:
    void nodesChanged(const QList<QObject*> &nodes);
    void stopped();
    void paused();
    void resumed();

    void searchTextChanged(const QString &searchText);
    void maxThreadCountChanged(int maxThreadCount);
    void maxDocCountChanged(int maxDocCount);
    void scannedDocsChanged(int scannedDocs);
    void activeThreadCountChanged(int activeThreadCount);
    void deferredThreadCountChanged(int deferredThreadCount);

protected:
    void createNode(const QUrl &url);
    void setMaxThreadCount(int maxThreadCount);
    void setSearchText(const QString &searchText);
    void setMaxDocCount(int maxDocCount);
    void setScannedDocs(int scannedDocs);

private:
    QList<QObject *> m_nodes;
    int m_maxThreadCount = 5;
    QString m_searchText;
    int m_maxDocCount = 20;
    int m_scannedDocs = 0;
    QSet<QUrl> m_foundUrls;
    int m_activeNodeCount = 0;
    QQueue<ScanNode *> m_deferredNodes;
    QReadWriteLock m_pauseLock;
    bool m_paused = false;
};

