#pragma once
#include "Content.h"

/**
 * @brief Represents a private direct message. Inherits from Content.
 */
class MessageData : public Content {
public:
    QString recipientUsername;
    bool isRead;

    MessageData() : isRead(false) {}
    MessageData(const QString& sender, const QString& recipient, const QString& text, const QString& time, bool read = false)
        : Content(sender, text, time), recipientUsername(recipient), isRead(read) {
    }

    // Override the base class method
    QJsonObject toJson() const override {
        QJsonObject obj;
        obj["sender"] = senderUsername;
        obj["recipient"] = recipientUsername;
        obj["content"] = textContent;
        obj["timestamp"] = timestamp;
        obj["isRead"] = isRead;
        return obj;
    }

    static MessageData fromJson(const QJsonObject& obj) {
        return MessageData(
            obj["sender"].toString("Unknown"),
            obj["recipient"].toString("Unknown"),
            obj["content"].toString("No Content"),
            obj["timestamp"].toString("Long ago"),
            obj["isRead"].toBool(false)
        );
    }
};