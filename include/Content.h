#pragma once

#include <QString>

class Content {
public:
	QString username;
	QString content;
	int likes;

	Content() {}
	Content(const QString& username, const QString& content) 
	: username(username), content(content), likes(0) {}
};