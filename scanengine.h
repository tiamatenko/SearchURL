#pragma once

#include <QObject>

#include <QQueue>
#include <QReadWriteLock>
#include <QSet>
#include <QUrl>

class ScanNode;

class ScanEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> nodes READ nodes NOTIFY nodesChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString searchText READ searchText NOTIFY searchTextChanged)
    Q_PROPERTY(int maxThreadCount READ maxThreadCount NOTIFY maxThreadCountChanged)
    Q_PROPERTY(int maxDocCount READ maxDocCount NOTIFY maxDocCountChanged)
    Q_PROPERTY(int scannedDocs READ scannedDocs NOTIFY scannedDocsChanged)
    Q_PROPERTY(int activeThreadCount READ activeThreadCount NOTIFY activeThreadCountChanged)
    Q_PROPERTY(int deferredThreadCount READ deferredThreadCount NOTIFY deferredThreadCountChanged)
    Q_PROPERTY(int foundDocCount READ foundDocCount NOTIFY foundDocCountChanged)
    Q_PROPERTY(int notFoundDocCount READ notFoundDocCount NOTIFY notFoundDocCountChanged)
    Q_PROPERTY(int errorDocCount READ errorDocCount NOTIFY errorDocCountChanged)
public:
    enum State { Stopped, Running, Paused };
    Q_ENUM(State)

    explicit ScanEngine(QObject *parent = nullptr);
    ~ScanEngine();

    inline QList<QObject*> nodes() const { return m_nodes; }
    inline State state() const { return m_state; }
    inline QString searchText() const { return m_searchText; }
    inline int maxThreadCount() const { return m_maxThreadCount; }
    inline int maxDocCount() const { return m_maxDocCount; }
    inline int scannedDocs() const { return m_scannedDocs; }
    inline int activeThreadCount() const { return m_activeNodeCount; }
    inline int deferredThreadCount() const { return m_deferredNodes.count(); }
    inline int foundDocCount() const { return m_foundDocCount; }
    inline int notFoundDocCount() const { return m_notFoundDocCount; }
    inline int errorDocCount() const { return m_errorDocCount; }

public slots:
    void start(const QUrl &url, const QString &searchText, int maxDocCount, int maxThreadCount);
    void stop();
    void pause();
    void resume();

private slots:
    void handleUrls(const QList<QUrl> &urls);

signals:
    void nodesChanged(const QList<QObject*> &nodes);
    void stopped();
    void paused();

    void stateChanged();
    void searchTextChanged(const QString &searchText);
    void maxThreadCountChanged(int maxThreadCount);
    void maxDocCountChanged(int maxDocCount);
    void scannedDocsChanged(int scannedDocs);
    void activeThreadCountChanged(int activeThreadCount);
    void deferredThreadCountChanged(int deferredThreadCount);
    void foundDocCountChanged(int foundDocCount);
    void notFoundDocCountChanged(int notFoundDocCount);
    void errorDocCountChanged(int errorDocCount);

protected:
    void setState(State newState);
    void createNode(const QUrl &url);
    void setMaxThreadCount(int maxThreadCount);
    void setSearchText(const QString &searchText);
    void setMaxDocCount(int maxDocCount);
    void setScannedDocs(int scannedDocs);

private:
    QList<QObject *> m_nodes;
    State m_state = Stopped;
    QString m_searchText;
    int m_maxThreadCount;
    int m_maxDocCount = 100;
    int m_scannedDocs = 0;
    QSet<QUrl> m_foundUrls;
    int m_activeNodeCount = 0;
    QQueue<ScanNode *> m_deferredNodes;
    QReadWriteLock m_pauseLock;
    int m_foundDocCount = 0;
    int m_notFoundDocCount = 0;
    int m_errorDocCount = 0;
};
