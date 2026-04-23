/** 
 * @file ValkyrieAdapter.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 */
#include "ValkyrieAdapter.h"
#include <QJsonArray>
#include <QJsonObject>

ValkyrieAdapter::ValkyrieAdapter(QObject *parent) : QObject(parent), socket_(new QTcpSocket(this)),
    expectedSize_(0) {

    connect(socket_, &QTcpSocket::connected, this, [this]() {
        expectedSize_ = 0; // 连接时重置状态机
        emit connectionChanged(true);
    });

    connect(socket_, &QTcpSocket::disconnected, this, [this]() {
        emit connectionChanged(false);
    });

    connect(socket_, &QTcpSocket::readyRead, this, &ValkyrieAdapter::onReadyRead);
}

ValkyrieAdapter::~ValkyrieAdapter() {
    if (socket_->isOpen()) {
        socket_->disconnectFromHost();
    }
}

void ValkyrieAdapter::connectToEngine(const QString& host, quint16 port) {
    if (socket_->state() == QAbstractSocket::UnconnectedState) {
        socket_->connectToHost(host, port);
    }
}

void ValkyrieAdapter::disconnectFromEngine() {
    socket_->disconnectFromHost();
}

void ValkyrieAdapter::onReadyRead() {
    QDataStream in(socket_);
    in.setByteOrder(QDataStream::BigEndian);

    while (true) {
        if (expectedSize_ == 0) {
            if (socket_->bytesAvailable() < 4) {
                return;
            }
            in >> expectedSize_;
        }

        if (socket_->bytesAvailable() < expectedSize_) {
            return;
        }

        QByteArray jsonData = socket_->read(expectedSize_);
        expectedSize_ = 0;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            continue;
        }

        QJsonArray json_array = doc.array();
        if (json_array.size() >= 0) {
            QJsonObject type_obj = json_array[0].toObject();
            QString msg_type = type_obj.value("type").toString();

            if (msg_type == "LOG") {
                // 解析日志数据
                QVector<QString> contents;
                for (int i = 1; i < json_array.size(); i++) {

                    QJsonObject content_obj = json_array[i].toObject();
                    QString content = content_obj.value("content").toString();
                    contents.append(content);
                }
                // 日志内容
                emit logReceived(contents);
            } else if (msg_type == "DATA") {
                QVector<QVariantMap> datas;
                for (int i = 1; i < json_array.size(); i++) {

                    QJsonObject content_obj = json_array[i].toObject();
                    QVariantMap data = content_obj.toVariantMap();
                    datas.append(data);
                }
                // 数据
                emit clustDataReceived(datas);
            } else if (msg_type == "SIGNAL") {
                // 发送数据请求
                QJsonObject content_obj = json_array[1].toObject();
                QVariantMap data = content_obj.toVariantMap();

                emit defectDataReceived(data);
            } else if (msg_type == "IMAGE") {
                QJsonObject content_obj = json_array[1].toObject();
                QVariantMap data = content_obj.toVariantMap();

                emit defectImageReceived(data);
            }
        }
    }
}

void ValkyrieAdapter::sendPayload(const QString& type, const QVariantMap& data) {
    if (!socket_) {
        return;
    }

    QJsonObject obj;
    obj["type"] = type;

    for (auto it = data.begin(); it != data.end(); ++it) {
        obj[it.key()] = QJsonValue::fromVariant(it.value());
    }

    QByteArray body = QJsonDocument(obj).toJson(QJsonDocument::Compact);

    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    out << static_cast<quint32>(body.size());
    packet.append(body);

    socket_->write(packet);
}