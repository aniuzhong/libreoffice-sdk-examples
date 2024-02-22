#include <iostream>

#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>

int main()
{
    css::uno::Reference<css::uno::XComponentContext> ctx = cppu::bootstrap();
    css::uno::Reference<css::lang::XMultiComponentFactory> mcf = ctx->getServiceManager();
    for (auto name : mcf->getAvailableServiceNames())
        std::cout << name << '\n';
    
    return 0;
}
