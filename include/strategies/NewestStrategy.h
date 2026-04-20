#pragma once
#include <QList>
#include "models/Post.h"
#include "FeedStrategy.h"
#include <algorithm>

class NewestStrategy : public FeedStrategy {
public:
    void sort(QList<Post>& posts, const User& currentUser) override {
        std::sort(posts.begin(), posts.end(), [](const Post& a, const Post& b) {
			QDateTime dtA = QDateTime::fromString(a.timestamp, "MMM dd, HH:mm"); // Convert the timestamp strings to QDateTime objects for comparison
            QDateTime dtB = QDateTime::fromString(b.timestamp, "MMM dd, HH:mm"); 
			return dtA > dtB; // Sort in descending order (newest first)
            });
    }
};

