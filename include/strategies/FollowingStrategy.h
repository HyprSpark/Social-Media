#pragma once
#include <QList>
#include "models/Post.h"
#include "FeedStrategy.h"

class FollowingStrategy : public FeedStrategy {
public:
    void sort(QList<Post>& posts, const User& currentUser) override {

        posts.erase(
            std::remove_if(posts.begin(), posts.end(), [&currentUser](const Post& p) {
                return !currentUser.following.contains(p.senderUsername)
                    && p.senderUsername != currentUser.username;
                }),
            posts.end()
        );

        std::sort(posts.begin(), posts.end(), [&currentUser](const Post& a, const Post& b) {
            bool aIsFollowed = currentUser.following.contains(a.senderUsername);
            bool bIsFollowed = currentUser.following.contains(b.senderUsername);
            return (aIsFollowed != bIsFollowed) ? aIsFollowed : false;
            });
    }
};