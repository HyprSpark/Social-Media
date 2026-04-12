#pragma once
#include <QList>
#include "models/Post.h"
#include "FeedStrategy.h"

class NewestStrategy : public FeedStrategy {
public:
    void sort(QList<Post>& posts, const User& currentUser) override {
        
    }
};

