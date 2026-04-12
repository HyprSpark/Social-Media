#pragma once
#include <QString>
#include <QJsonObject>

/**
 * @brief Abstract base class for all user-generated content.
 */
class Content {
public:
    QString senderUsername;
    QString textContent;
    QString timestamp;

    Content() {}
    Content(const QString& sender, const QString& text, const QString& time)
        : senderUsername(sender), textContent(text), timestamp(time) {
    }

    virtual ~Content() {}

    // Pure virtual function forces derived classes to implement their own JSON logic
    virtual QJsonObject toJson() const = 0;
};