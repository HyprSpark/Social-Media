// -- Header --
#include "Messages.h"
#include "userManager.h"
#include "models/MessageData.h"
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QCoreApplication>

Messages::Messages(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    qDebug() << "[INFO] UI: Initializing Messages window.";

    // -- Button Connections -- //
    connect(ui.btnReturn, &QPushButton::clicked, this, &Messages::onReturnClicked);
    connect(ui.btnSend, &QPushButton::clicked, this, &Messages::onSendClicked);
    connect(ui.btnRead, &QPushButton::clicked, this, &Messages::onReadClicked);
    connect(ui.btnDelete, &QPushButton::clicked, this, &Messages::onClearInboxClicked);
}

Messages::~Messages() {}

void Messages::setActiveUser(const User& user)
{
    currentUser = user;
    qDebug() << "[INFO] Messages: Active session for user:" << currentUser.username;

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);
    int messageCount = 0;

    qDebug() << "[PERSISTENCE] Checking inbox count at:" << filePath;

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isArray()) {
            QJsonArray messagesArray = doc.array();
            for (const QJsonValue& value : messagesArray) {
                QJsonObject msgObj = value.toObject();
                if (msgObj["recipient"].toString() == currentUser.getUsername()) {
                    messageCount++;
                }
            }
        }
        file.close();
    }

    ui.msgCountlbl->setText(QString::number(messageCount) + " new messages");
    qDebug() << "[DEBUG] UI: Inbox count updated to" << messageCount;
}

void Messages::onReturnClicked()
{
    this->close();
}

void Messages::onSendClicked()
{
    QString recipient = ui.messageTarget->text().trimmed();
    QString messageBody = ui.messageText->text().trimmed();

    if (recipient.isEmpty() || messageBody.isEmpty()) {
        qDebug() << "[DEBUG] Messages: Rejected send attempt due to empty fields.";
        QMessageBox::warning(this, "Empty Fields", "Please enter both a recipient and a message.");
        return;
    }

    qDebug() << "[DEBUG] Messages: Validating recipient existence:" << recipient;

    if (!UserManager::userExists(recipient)) {
        qDebug() << "[ERROR] Messages: Recipient '" << recipient << "' not found in database.";
        QMessageBox::critical(this, "User Not Found",
            "The user '" + recipient + "' does not exist. Please check the spelling.");
        return;
    }

    if (recipient == currentUser.getUsername()) {
        qDebug() << "[DEBUG] Messages: Self-message attempt blocked.";
        QMessageBox::information(this, "Self-Talk", "You can't message yourself here. Try a diary!");
        return;
    }

    // --- Using the Domain Model ---
    QString currentTime = QDateTime::currentDateTime().toString(Qt::ISODate);
    MessageData newMsgData(currentUser.getUsername(), recipient, messageBody, currentTime);
    qDebug() << "[DEBUG] Domain: MessageData object created successfully.";

    QJsonObject newMessage = newMsgData.toJson();
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);
    QJsonArray messagesArray;

    qDebug() << "[PERSISTENCE] Attempting to write message to project head:" << filePath;

    if (file.open(QIODevice::ReadOnly)) {
        messagesArray = QJsonDocument::fromJson(file.readAll()).array();
        file.close();
    }

    messagesArray.append(newMessage);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(messagesArray).toJson());
        file.close();
        qDebug() << "[SUCCESS] PERSISTENCE: Message saved to JSON database.";
    }
    else {
        qDebug() << "[ERROR] PERSISTENCE: Write failed! Reason:" << file.errorString();
        QMessageBox::critical(this, "File Error", "Could not save the message. Check the logs.");
        return;
    }

    ui.messageText->clear();
    QMessageBox::information(this, "Sent", "Message delivered to " + recipient);
}

void Messages::onReadClicked()
{
    qDebug() << "[INFO] Messages: Fetching inbox for user:" << currentUser.getUsername();

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[DEBUG] PERSISTENCE: No message file found (Empty Inbox).";
        ui.msgCountlbl->setText("0 messages");
        QMessageBox::information(this, "Inbox", "Your inbox is empty.");
        return;
    }

    QJsonArray messagesArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    int messageCount = 0;
    QString inboxText = "";

    for (const QJsonValue& value : messagesArray) {
        QJsonObject msgObj = value.toObject();

        if (msgObj["recipient"].toString() == currentUser.getUsername()) {
            messageCount++;

            // Note: pull from "content" key as per our polymorphic MessageData model
            QString sender = msgObj["sender"].toString();
            QString body = msgObj["content"].toString();
            QString timestamp = msgObj["timestamp"].toString();

            inboxText += "From: " + sender + "\n";
            inboxText += "Date: " + timestamp + "\n";
            inboxText += "Message:\n" + body + "\n";
            inboxText += "-----------------------------------\n";
        }
    }

    ui.msgCountlbl->setText(QString::number(messageCount) + " messages");
    qDebug() << "[SUCCESS] UI: Processed" << messageCount << "received messages.";

    if (messageCount == 0) {
        QMessageBox::information(this, "Inbox", "No new messages for you.");
    }
    else {
        QMessageBox::information(this, "Your Inbox", inboxText);
    }
}

void Messages::onClearInboxClicked()
{
    qDebug() << "[INFO] Messages: Request to clear inbox for:" << currentUser.getUsername();

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, "Inbox", "Your inbox is already empty.");
        return;
    }

    QJsonArray messagesArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    QJsonArray updatedArray;
    bool messagesDeleted = false;

    for (const QJsonValue& value : messagesArray) {
        QJsonObject msgObj = value.toObject();
        if (msgObj["recipient"].toString() == currentUser.getUsername()) {
            messagesDeleted = true;
        }
        else {
            updatedArray.append(msgObj);
        }
    }

    if (messagesDeleted) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.write(QJsonDocument(updatedArray).toJson());
            file.close();

            ui.msgCountlbl->setText("0 messages");
            qDebug() << "[SUCCESS] PERSISTENCE: Inbox wiped for user. Shared file updated.";
            QMessageBox::information(this, "Cleared", "All your messages have been permanently deleted.");
        }
        else {
            qDebug() << "[ERROR] PERSISTENCE: Failed to truncate file for clearing.";
        }
    }
    else {
        qDebug() << "[DEBUG] UI: Delete request ignored (No messages found for current user).";
        QMessageBox::information(this, "Inbox", "You have no messages to delete.");
    }
}