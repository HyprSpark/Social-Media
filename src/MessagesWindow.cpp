#include "MessagesWindow.h"
#include "UserManager.h"
#include "models/MessageData.h"
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QCoreApplication>
#include <QDebug>

MessagesWindow::MessagesWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    qDebug() << "[INFO] UI: Initializing MessagesWindow instance.";

    connect(ui.btnReturn, &QPushButton::clicked, this, &MessagesWindow::onReturnClicked);
    connect(ui.btnSend, &QPushButton::clicked, this, &MessagesWindow::onSendClicked);
    connect(ui.btnRead, &QPushButton::clicked, this, &MessagesWindow::onReadClicked);
    connect(ui.btnDelete, &QPushButton::clicked, this, &MessagesWindow::onClearInboxClicked);
}

MessagesWindow::~MessagesWindow() {}

void MessagesWindow::setActiveUser(const User& user)
{
    currentUser = user;
    qDebug() << "[INFO] Messages: Active session for user:" << currentUser.username;

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);

    int unreadCount = 0;
    int totalCount = 0;

    if (file.open(QIODevice::ReadOnly)) {
        QJsonArray messagesArray = QJsonDocument::fromJson(file.readAll()).array();
        for (const QJsonValue& value : messagesArray) {
            QJsonObject msgObj = value.toObject();
            if (msgObj["recipient"].toString() == currentUser.getUsername()) {
                totalCount++;
                if (!msgObj["isRead"].toBool()) {
                    unreadCount++;
                }
            }
        }
        file.close();
    }

    // Dynamic UI feedback for the Inbox label
    if (unreadCount > 0) {
        ui.msgCountlbl->setText(QString::number(unreadCount) + " NEW messages");
        ui.msgCountlbl->setStyleSheet("color: #4cd137; font-weight: bold;"); // Green for unread
    }
    else {
        ui.msgCountlbl->setText(QString::number(totalCount) + " messages");
        ui.msgCountlbl->setStyleSheet("color: #f5f6fa;"); // Default for read
    }
}

void MessagesWindow::onReturnClicked()
{
    qDebug() << "[DEBUG] UI: Wiping drafts and closing MessagesWindow.";
    ui.messageTarget->clear();
    ui.messageText->clear();
    this->close();
}

void MessagesWindow::onSendClicked()
{
    QString recipient = ui.messageTarget->text().trimmed();
    QString messageBody = ui.messageText->text().trimmed();

    if (recipient.isEmpty() || messageBody.isEmpty()) {
        QMessageBox::warning(this, "Empty Fields", "Please enter a recipient and a message.");
        return;
    }

    if (!UserManager::userExists(recipient)) {
        QMessageBox::critical(this, "Error", "User '" + recipient + "' not found.");
        return;
    }

    QString currentTime = QDateTime::currentDateTime().toString(Qt::ISODate);
    MessageData newMsgData(currentUser.getUsername(), recipient, messageBody, currentTime);

    QJsonObject newMessage = newMsgData.toJson();
    newMessage["isRead"] = false; // Explicitly set as unread for the recipient

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);
    QJsonArray messagesArray;

    if (file.open(QIODevice::ReadOnly)) {
        messagesArray = QJsonDocument::fromJson(file.readAll()).array();
        file.close();
    }

    messagesArray.append(newMessage);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(messagesArray).toJson());
        file.close();
        qDebug() << "[SUCCESS] PERSISTENCE: Message delivered to" << recipient;
    }

    ui.messageText->clear();
    QMessageBox::information(this, "Sent", "Message delivered!");
}

void MessagesWindow::onReadClicked()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonArray messagesArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    QString inboxText = "";
    bool databaseChanged = false;

    for (int i = 0; i < messagesArray.size(); ++i) {
        QJsonObject msgObj = messagesArray[i].toObject();

        if (msgObj["recipient"].toString() == currentUser.getUsername()) {
            inboxText += "From: " + msgObj["sender"].toString() + "\n";
            inboxText += "Message: " + msgObj["content"].toString() + "\n";
            inboxText += "--------------------------\n";

            if (!msgObj["isRead"].toBool()) {
                msgObj["isRead"] = true; // Mark as read
                messagesArray[i] = msgObj;
                databaseChanged = true;
            }
        }
    }

    if (databaseChanged && file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(messagesArray).toJson());
        file.close();
    }

    QMessageBox::information(this, "Your Inbox", inboxText.isEmpty() ? "No messages." : inboxText);
    setActiveUser(currentUser); // Refresh local label immediately
}

void MessagesWindow::onClearInboxClicked()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonArray messagesArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    QJsonArray updatedArray;
    for (const QJsonValue& value : messagesArray) {
        if (value.toObject()["recipient"].toString() != currentUser.getUsername()) {
            updatedArray.append(value);
        }
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(updatedArray).toJson());
        file.close();
        setActiveUser(currentUser);
        QMessageBox::information(this, "Cleared", "Inbox wiped.");
    }
}