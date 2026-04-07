#ifndef TRACERFACTORY_H
#define TRACERFACTORY_H

#include "vmime/net/tracer.hpp"

class tracerFactory : public vmime::net::tracerFactory
{
public:
    tracerFactory();

    vmime::shared_ptr<vmime::net::tracer> create(const vmime::shared_ptr<vmime::net::service>&, const int);
};

#endif // TRACERFACTORY_H
