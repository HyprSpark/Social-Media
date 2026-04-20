#pragma once
#include <QList>
#include "models/Post.h"
#include "models/User.h"
#include "FeedStrategy.h"
#include <algorithm>

class MostLikedStrategy : public FeedStrategy {
public:
    void sort(QList<Post>& posts, const User& currentUser) override {
        std::sort(posts.begin(), posts.end(), [](const Post& a, const Post& b) {
			return a.likedBy.size() > b.likedBy.size(); // Sort in descending order of likes
            });
    }
};