#ifndef TRACER_H
#define TRACER_H

#include "vmime/net/tracer.hpp"
#include <QObject>

class tracer : public vmime::net::tracer
{
    vmime::string _proto;
    int _connectionId;

public:
    tracer(const vmime::string&, const int);

    void traceSend(const vmime::string& line);
    void traceReceive(const vmime::string& line);
};

#endif // TRACER_H
