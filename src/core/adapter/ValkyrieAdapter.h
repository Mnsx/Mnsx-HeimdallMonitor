/** 
 * @file ValkyrieAdapter.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 * @description 
 */
#ifndef HEIMDALLMONITOR_VALKYRIEADAPTER_H
#define HEIMDALLMONITOR_VALKYRIEADAPTER_H
#include <qtmetamacros.h>
#include <QTcpSocket>

namespace mnsx {
    namespace valkyrie {
        class ValkyriePayload;
    }
}

class ValkyrieAdapter : public QObject {

    Q_OBJECT

public:
    ~ValkyrieAdapter() override;

    static ValkyrieAdapter& getIntance() {
        static ValkyrieAdapter valkyrieAdapter;
        return valkyrieAdapter;
    }

    void connectToEngine(const QString& host, quint16 port);
    void disconnectFromEngine();

    // 发送数据到 WSL 后端 (例如发送控制指令)
    void sendPayload(const QString& type, const QVariantMap& data);

    QTcpSocket* getSocket() { return socket_; }

signals:
    // --- UI 通知信号 ---
    void connectionChanged(bool connected);
    void defectImageReceived(const QVariantMap& map);
    void logReceived(const QVector<QString>);
    void clustDataReceived(const QVector<QVariantMap>);
    void defectDataReceived(const QVariantMap& map);
    void realtimeYieldDataReceived(double res);
    void typeCountAdd(int type);
    void falseRateReceived(const QVector<QMap<QString, QVariant>> & maps);

private:
    explicit ValkyrieAdapter(QObject *parent = nullptr);

    // 处理底层 Socket 数据到达
    void onReadyRead();

    // socket
    QTcpSocket* socket_;

    quint32 expectedSize_; // 当前数据包长度
};

#endif //HEIMDALLMONITOR_VALKYRIEADAPTER_H