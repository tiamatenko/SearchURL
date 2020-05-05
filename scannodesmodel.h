#pragma once

#include <QObject>

#include <QSet>
#include <QUrl>

class ScanNodesModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> nodes READ nodes NOTIFY nodesChanged)
    Q_PROPERTY(QString searchText READ searchText NOTIFY searchTextChanged)
    Q_PROPERTY(uint maxThreadCount READ maxThreadCount NOTIFY maxThreadCountChanged)
    Q_PROPERTY(uint maxDocCount READ maxDocCount NOTIFY maxDocCountChanged)
    Q_PROPERTY(uint scannedDocs READ scannedDocs NOTIFY scannedDocsChanged)
public:
    explicit ScanNodesModel(QObject *parent = nullptr);

    inline QList<QObject*> nodes() const { return m_nodes; }
    inline QString searchText() const { return m_searchText; }
    inline uint maxThreadCount() const { return m_maxThreadCount; }
    inline uint maxDocCount() const { return m_maxDocCount; }
    inline uint scannedDocs() const { return m_scannedDocs; }

public slots:
    void start(const QUrl &url, uint maxThreadCount, const QString &searchText, uint maxDocCount);
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
    void maxThreadCountChanged(uint maxThreadCount);
    void maxDocCountChanged(uint maxDocCount);
    void scannedDocsChanged(uint scannedDocs);

protected:
    void createNode(const QUrl &url);
    void setMaxThreadCount(uint maxThreadCount);
    void setSearchText(const QString &searchText);
    void setMaxDocCount(uint maxDocCount);
    void setScannedDocs(uint scannedDocs);

private:
    QList<QObject *> m_nodes;
    uint m_maxThreadCount = 5;
    QString m_searchText;
    uint m_maxDocCount = 20;
    uint m_scannedDocs = 0;
    QSet<QUrl> m_foundUrls;
};

