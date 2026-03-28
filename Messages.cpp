// -- Header -- //
#include "Messages.h"
#include "userManager.h"
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QCoreApplication>

/**
* @brief This window allows users to access messages
* Users can send messages to other users, read their inbox, and clear their inbox.
*/
Messages::Messages(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	// -- Button Connections -- //
    connect(ui.btnReturn, &QPushButton::clicked, this, &Messages::onReturnClicked);
    connect(ui.btnSend, &QPushButton::clicked, this, &Messages::onSendClicked);
    connect(ui.btnRead, &QPushButton::clicked, this, &Messages::onReadClicked);
	connect(ui.btnDelete, &QPushButton::clicked, this, &Messages::onClearInboxClicked);
}

Messages::~Messages()
{
}

/**
* @brief Sets the active user for the messages window.
* This is used to identify who is sending and recieving messages,
*/

void Messages::setActiveUser(const User& user)
{
    currentUser = user;

    // --- Automatically count messages on load ---
    QString filePath = QCoreApplication::applicationDirPath() + "/messages.json";
    QFile file(filePath);
    int messageCount = 0;

    // Open the file and count only the messages for this specific user
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

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

    // Update the label immediately
    ui.msgCountlbl->setText(QString::number(messageCount) + " new messages");
}

/**
* @brief Handles the Return button click.
* Prevents the user from having to close the window manually.
*/
void Messages::onReturnClicked()
{
    this->close();
}

/**
* @brief Handles the Send button click.
* Checks if the recipient exists before saving the message to a JSON file.
* When the target user clicks "Read", they will only see messages where they are the recipient.
*/

void Messages::onSendClicked()
{
    QString recipient = ui.messageTarget->text().trimmed();
    QString messageBody = ui.messageText->text().trimmed();

    // 1. Basic Validation
    if (recipient.isEmpty() || messageBody.isEmpty()) {
        QMessageBox::warning(this, "Empty Fields", "Please enter both a recipient and a message.");
        return;
    }

    qDebug() << "LOG [Step 1]: Validation passed. Checking if user exists...";

    // 2. Test if the recipient exists.
    if (!UserManager::userExists(recipient)) {
        QMessageBox::critical(this, "User Not Found",
            "The user '" + recipient + "' does not exist. Please check the spelling.");
        return;
    }

    qDebug() << "LOG [Step 2]: User exists! Building the JSON object...";

    // 3. Prevent messaging yourself 
    if (recipient == currentUser.getUsername()) {
        QMessageBox::information(this, "Self-Talk", "You can't message yourself here. Try a diary!");
        return;
    }

    // 4. Build the JSON Object
    QJsonObject newMessage;
    newMessage["sender"] = currentUser.getUsername();
    newMessage["recipient"] = recipient;
    newMessage["body"] = messageBody;
    newMessage["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    // 5. Setup File Path
    QString filePath = QCoreApplication::applicationDirPath() + "/messages.json";

    qDebug() << "LOG [Step 3]: Attempting to save to this exact path:" << filePath;

    QFile file(filePath);
    QJsonArray messagesArray;

    // 6. Read existing messages
    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << "LOG [Step 4]: Existing file found and opened for reading.";
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray()) {
            messagesArray = doc.array();
        }
        file.close();
    }
    else {
        qDebug() << "LOG [Step 4]: No existing file found to read (Normal for the first message).";
    }

    messagesArray.append(newMessage);

    // 7. Write to the file
    if (file.open(QIODevice::WriteOnly)) {
        qDebug() << "LOG [Step 5]: File successfully opened for writing! Saving data...";
        QJsonDocument doc(messagesArray);
        file.write(doc.toJson());
        file.close();
    }
    else {
        // Log the exact error to the terminal, but still alert the user it failed
        qDebug() << "ERROR [Step 5]: Write failed! Reason:" << file.errorString();
        QMessageBox::critical(this, "File Error", "Could not save the message. Check the logs.");
        return;
    }

    ui.messageText->clear();
    QMessageBox::information(this, "Sent", "Message delivered to " + recipient);
}

/**
* @brief Handles the Read button click.
* Shows all messages in the inbox that are meant for the current user.
* Shows an alert if the inbox is empty.
*/

void Messages::onReadClicked()
{
    qDebug() << "LOG: Checking for messages for user:" << currentUser.getUsername();

    // 1. Locate the file we just created
    QString filePath = QCoreApplication::applicationDirPath() + "/messages.json";
    QFile file(filePath);

    // 2. If the file doesn't exist or can't be opened, the inbox is empty
    if (!file.open(QIODevice::ReadOnly)) {
        ui.msgCountlbl->setText("0 messages");
        QMessageBox::information(this, "Inbox", "Your inbox is empty.");
        return;
    }

    // 3. Read the JSON data
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray messagesArray = doc.array();

    int messageCount = 0;
    QString inboxText = "";

    // 4. Loop through every message in the database
    for (const QJsonValue& value : messagesArray) {
        QJsonObject msgObj = value.toObject();

        // 5. Filter: Only grab messages meant for the CURRENT user
        if (msgObj["recipient"].toString() == currentUser.getUsername()) {
            messageCount++;

            QString sender = msgObj["sender"].toString();
            QString body = msgObj["body"].toString();
            QString timestamp = msgObj["timestamp"].toString();

            // Format the message nicely
            inboxText += "From: " + sender + "\n";
            inboxText += "Date: " + timestamp + "\n";
            inboxText += "Message:\n" + body + "\n";
            inboxText += "-----------------------------------\n";
        }
    }

    // 6. Update your UI label with the total count
    ui.msgCountlbl->setText(QString::number(messageCount) + " messages");

    // 7. Display the results
    if (messageCount == 0) {
        QMessageBox::information(this, "Inbox", "No new messages for you.");
    }
    else {
        // Show all messages in a popup
        QMessageBox::information(this, "Your Inbox", inboxText);
    }
}

/**
* @brief Handles the Clear Inbox button click.
* Loops through the array only copying messages that don't belong to the user, then saves this new array back to the file.
* This way we don't accidentally delete messages meant for other users who share the same database file.
*/

void Messages::onClearInboxClicked()
{
    qDebug() << "LOG: Attempting to clear inbox for:" << currentUser.getUsername();

    QString filePath = QCoreApplication::applicationDirPath() + "/messages.json";
    QFile file(filePath);

    // 1. Open the file to read existing messages
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, "Inbox", "Your inbox is already empty.");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray messagesArray = doc.array();

    // We will build a NEW array that only contains messages we want to keep
    QJsonArray updatedArray;
    bool messagesDeleted = false;

    // 2. Loop through every message
    for (const QJsonValue& value : messagesArray) {
        QJsonObject msgObj = value.toObject();

        // 3. Filter: Does this message belong to the current user?
        if (msgObj["recipient"].toString() == currentUser.getUsername()) {
            messagesDeleted = true; // skip this message
        }
        else {
            updatedArray.append(msgObj); // add this message
        }
    }

    // 4. Save the updated list back to the file
    if (messagesDeleted) {
        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument newDoc(updatedArray);
            file.write(newDoc.toJson());
            file.close();

            ui.msgCountlbl->setText("0 messages");
            QMessageBox::information(this, "Cleared", "All your messages have been permanently deleted.");
        }
        else {
            QMessageBox::critical(this, "File Error", "Could not save the updated database.");
        }
    }
    else {
        QMessageBox::information(this, "Inbox", "You have no messages to delete.");
    }
}