#pragma once
#include <QList>
#include "models/Post.h"
#include "FeedStrategy.h"

class FollowingStrategy : public FeedStrategy {
public:
    void sort(QList<Post>& posts, const User& currentUser) override {

        posts.erase(
			std::remove_if(posts.begin(), posts.end(), [&currentUser](const Post& p) { // Remove posts from users that the current user is not following, and also remove the current user's own posts
                return !currentUser.following.contains(p.senderUsername)
                    && p.senderUsername != currentUser.username; 
                }),
            posts.end()
        );

        std::sort(posts.begin(), posts.end(), [&currentUser](const Post& a, const Post& b) {
			bool aIsFollowed = currentUser.following.contains(a.senderUsername); // Check if the sender of post a is followed by the current user
            bool bIsFollowed = currentUser.following.contains(b.senderUsername); // Check if the sender of post b is followed by the current user
			return (aIsFollowed != bIsFollowed) ? aIsFollowed : false; // If one post is from a followed user and the other isn't, the followed user's post comes first.
            });
    }
};