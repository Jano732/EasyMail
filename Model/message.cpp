#include "message.h"
#include <qdebug.h>

Message::Message(ResolvedBody& rb)
{
    _resolved_body = rb;
}

Message::~Message()
{

}

