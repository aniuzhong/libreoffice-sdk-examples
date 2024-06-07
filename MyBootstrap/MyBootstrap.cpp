#include <windows.h>

#include <vector>
#include <thread>
#include <fstream>
#include <iostream>

#include <rtl/random.h>
#include <rtl/process.h>
#include <rtl/ustrbuf.hxx>
#include <osl/file.hxx>
#include <osl/security.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/bridge/UnoUrlResolver.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/connection/NoConnectException.hpp>



// Create a random pipe name
auto CreatePipeName() -> rtl::OUString
{
    rtlRandomPool hPool = rtl_random_createPool();
    sal_uInt8 bytes[16];
    switch (rtl_random_getBytes(hPool, bytes, std::size(bytes)))
    {
    case rtl_Random_E_None:
        break;
    case rtl_Random_E_Argument:
    case rtl_Random_E_Memory:
    case rtl_Random_E_Unknown:
        return {};
    }
    rtl_random_destroyPool(hPool);
    rtl::OUStringBuffer ustrbuf("uno");
    for (auto byte : bytes)
        ustrbuf.append(static_cast<sal_Int32>(byte));

    return rtl::OUString(ustrbuf.makeStringAndClear());
}

// Execute soffice process and return pidfile of soffice.bin
auto ExecuteOfficeProcess(const rtl::OUString& ustrOfficePath,
                          const rtl::OUString& ustrUserPath,
                          const rtl::OUString& ustrPipeName) -> rtl::OUString
{
    rtl::OUString ustrFileURL;
    osl::FileBase::getFileURLFromSystemPath(ustrOfficePath, ustrFileURL);
    if (!ustrFileURL.isEmpty() && !ustrFileURL.endsWith("/"))
        ustrFileURL += "/";

    // Create default local component context
    css::uno::Reference<css::uno::XComponentContext> xLocalContext(cppu::defaultBootstrap_InitialComponentContext());

    // Arguments
    rtl::OUString args[] = { rtl::OUString("-env:UserInstallation=file:///" + ustrUserPath),
                             rtl::OUString("--pidfile=" + ustrUserPath + "/pidfile"),
                             rtl::OUString("--accept=pipe,name=" + ustrPipeName + ";urp;"),
                             rtl::OUString("--nologo"),
                             rtl::OUString("--norestore"),
                             rtl::OUString("--nodefault"),
                             rtl::OUString("--nolockcheck") };
    rtl_uString* args1[] = { args[0].pData,
                             args[1].pData,
                             args[2].pData,
                             args[3].pData,
                             args[4].pData,
                             args[5].pData,
                             args[6].pData };

    // Start office process
    osl::Security oSecurity;
    oslProcess hProcess = nullptr;
    switch (osl_executeProcess(rtl::OUString(ustrFileURL + "soffice").pData,
                               args1,
                               std::size(args1),
                               osl_Process_DETACHED,
                               oSecurity.getHandle(),
                               nullptr, // => current working dir
                               nullptr, 0, // => no env vars
                               &hProcess))
    {
    case osl_Process_E_None: /* no error */
        break;
    case osl_Process_E_NotFound: /* image not found */
    case osl_Process_E_TimedOut: /* timeout occurred */
    case osl_Process_E_NoPermission: /* permission denied */
    case osl_Process_E_Unknown: /* unknown error */
    case osl_Process_E_InvalidError: /* unmapped error */
        return {};
    }

    return { ustrUserPath + "/pidfile" };
}

// Terminate soffice process
auto TerminateOfficeProcess(rtl::OUString ustrPidFile) -> void
{
    std::ifstream oPidFile(ustrPidFile.pData->buffer);
    if (!oPidFile.is_open())
        return;

    std::string sPid;
    oPidFile >> sPid;

    char* pEnd;
    TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, std::strtoll(sPid.c_str(), &pEnd, 10)), 0);
}

// Connect to an office process
auto ConnectToOfficeProcess(const rtl::OUString& ustrPipeName) -> css::uno::Reference<css::uno::XComponentContext>
{
    css::uno::Reference<css::uno::XComponentContext> xRemoteContext = nullptr;

    // Create default local component context
    css::uno::Reference<css::uno::XComponentContext> xLocalContext(cppu::defaultBootstrap_InitialComponentContext());

    // Create a URL resolver
    css::uno::Reference<css::bridge::XUnoUrlResolver> xUrlResolver(css::bridge::UnoUrlResolver::create(xLocalContext));

    // Connection string
    rtl::OUString sConnectString("uno:pipe,name=" + ustrPipeName + ";urp;StarOffice.ComponentContext");

    // Wait until office is started
    for (;;)
    {
        try
        {
            // Try to connect to office
            xRemoteContext.set(xUrlResolver->resolve(sConnectString), css::uno::UNO_QUERY_THROW);
            break;
        }
        catch (css::connection::NoConnectException&)
        {
            // Wait 500 ms, then try to connect again
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    return xRemoteContext;
}

css::uno::Reference<css::uno::XComponentContext>
MyBootstrap(const rtl::OUString& ustrOfficePath, const rtl::OUString& ustrUserPath)
{
    css::uno::Reference<css::uno::XComponentContext> xRemoteContext = nullptr;

    rtl::OUString ustrFileURL;
    osl::FileBase::getFileURLFromSystemPath(ustrOfficePath, ustrFileURL);
    if (!ustrFileURL.isEmpty() && !ustrFileURL.endsWith("/"))
        ustrFileURL += "/";

    // Create default local component context
    css::uno::Reference<css::uno::XComponentContext> xLocalContext(cppu::defaultBootstrap_InitialComponentContext());

    // Create a random pipe name
    rtlRandomPool hPool = rtl_random_createPool();
    sal_uInt8 bytes[16];
    switch (rtl_random_getBytes(hPool, bytes, std::size(bytes)))
    {
        case rtl_Random_E_None:
            break;
        case rtl_Random_E_Argument:
        case rtl_Random_E_Memory:
        case rtl_Random_E_Unknown:
            return xRemoteContext;
    }
    rtl_random_destroyPool(hPool);
    rtl::OUStringBuffer ustrbuf("uno");
    for (auto byte : bytes)
        ustrbuf.append(static_cast<sal_Int32>(byte));
    rtl::OUString sPipeName(ustrbuf.makeStringAndClear());

    // Arguments
    rtl::OUString args[] = { rtl::OUString("-env:UserInstallation=file:///" + ustrUserPath),
                             rtl::OUString("--nologo"),
                             rtl::OUString("--norestore"),
                             rtl::OUString("--nolockcheck"),
                             rtl::OUString("--accept=pipe,name=" + sPipeName + ";urp;") };
    rtl_uString* args1[] = { args[0].pData,
                             args[1].pData,
                             args[2].pData,
                             args[3].pData,
                             args[4].pData };

    // Start office process
    osl::Security oSecurity;
    oslProcess hProcess = nullptr;
    switch (osl_executeProcess(rtl::OUString(ustrFileURL + "soffice").pData,
                               args1,
                               std::size(args1),
                               osl_Process_DETACHED,
                               oSecurity.getHandle(),
                               nullptr, // => current working dir
                               nullptr, 0, // => no env vars
                               &hProcess))
    {
        case osl_Process_E_None: /* no error */
            break;
        case osl_Process_E_NotFound: /* image not found */
        case osl_Process_E_TimedOut: /* timeout occurred */
        case osl_Process_E_NoPermission: /* permission denied */
        case osl_Process_E_Unknown: /* unknown error */
        case osl_Process_E_InvalidError: /* unmapped error */
            return xRemoteContext;
    }

    // Create a URL resolver
    css::uno::Reference<css::bridge::XUnoUrlResolver> xUrlResolver(css::bridge::UnoUrlResolver::create(xLocalContext));

    // Connection string
    rtl::OUString sConnectString("uno:pipe,name=" + sPipeName + ";urp;StarOffice.ComponentContext");

    // Wait until office is started
    for (;;)
    {
        try
        {
            // Try to connect to office
            xRemoteContext.set(xUrlResolver->resolve(sConnectString), css::uno::UNO_QUERY_THROW);
            break;
        }
        catch (css::connection::NoConnectException&)
        {
            // Wait 500 ms, then try to connect again
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    return xRemoteContext;
}

int main()
{
    auto xRemoteContext = MyBootstrap(L"C:\\cygwin\\home\\user\\lode\\dev\\core\\instdir\\program",
                                      L"D:/tmp/test");
    auto XMultiComponentFactory = xRemoteContext->getServiceManager();
    for (auto ustrName : XMultiComponentFactory->getAvailableServiceNames())
        std::cout << ustrName << '\n';

    return 0;
}