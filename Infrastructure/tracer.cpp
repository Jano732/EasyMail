#include "tracer.h"
#include "QtCore/qdebug.h"


tracer::tracer(const vmime::string& proto, const int connectionId)
    :_proto(proto), _connectionId(connectionId)
{}

void tracer::traceSend(const vmime::string& line)
{
    qDebug() << "[" << _proto << "," << _connectionId << "] C: " << line << "\n";
}

void tracer::traceReceive(const vmime::string& line)
{
    qDebug() << "[" << _proto << ':' << _connectionId << "] S: " << line << "\n";
}
