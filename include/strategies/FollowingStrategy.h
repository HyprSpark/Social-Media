#pragma once
#include <QList>
#include "models/Post.h"
#include "FeedStrategy.h"

class FollowingStrategy : public FeedStrategy {
public:
    void sort(QList<Post>& posts, const User& currentUser) override {
        std::sort(posts.begin(), posts.end(), [&currentUser](const Content& a, const Content& b) {
            bool aIsFollowed = currentUser.following.contains(a.senderUsername);
            bool bIsFollowed = currentUser.following.contains(b.senderUsername);
            return (aIsFollowed != bIsFollowed) ? aIsFollowed : false;
            });
    }
};