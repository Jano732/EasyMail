#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H

#include "../Repository/repositoryemail.h"
#include <qobject.h>
class Message
{  
    ResolvedBody _resolved_body;

public:
    Message();
    Message(ResolvedBody&);
    ~Message();


};


#endif // MESSAGE_H
