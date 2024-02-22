#include <cppuhelper/bootstrap.hxx>

#include "OfficeConnection.h"

OfficeConnection::OfficeConnection()
{
    // Get the remote office context.
    // If necessary a new process is started.
    m_context = cppu::bootstrap(); 
    m_manager = m_context->getServiceManager();
}

