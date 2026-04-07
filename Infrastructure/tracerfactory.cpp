#include "tracerfactory.h"
#include "Infrastructure/tracer.h"
#include "vmime/net/service.hpp"

tracerFactory::tracerFactory() {}

vmime::shared_ptr<vmime::net::tracer> tracerFactory::create(const vmime::shared_ptr<vmime::net::service>& serv, const int connectionId)
{
    return vmime::make_shared<tracer>(serv->getProtocolName(), connectionId);
}
