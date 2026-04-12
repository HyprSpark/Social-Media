#pragma once
#include <QList>
#include "models/Content.h"
#include "models/Post.h"
#include "models/User.h"

// The abstract base class for sorting strategies 
class FeedStrategy {
public:
    virtual ~FeedStrategy() {}
    // Pure virtual function that each concrete strategy must implement
    virtual void sort(QList<Post>& posts, const User& currentUser) = 0;
};
