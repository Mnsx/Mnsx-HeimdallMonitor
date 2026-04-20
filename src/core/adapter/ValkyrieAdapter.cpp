/** 
 * @file ValkyrieAdapter.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 */
#include "ValkyrieAdapter.h"
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

        QJsonObject obj = doc.object();
        QString msgType = obj["type"].toString();

        if (msgType == "LOG") {
            // 日志内容
            // emit logReceived(obj["level"].toString(), obj["content"].toString());
        } else if (msgType == "DATA") {
            // 数据
        }
    }
}

void ValkyrieAdapter::sendPayload(const QString& type, const QVariantMap& data) {
    if (!socket_ || socket_->state() != QAbstractSocket::ConnectedState) {
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