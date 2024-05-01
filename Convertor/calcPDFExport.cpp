#include <iostream>

#include <osl/file.hxx>
#include <rtl/process.h>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/presentation/XPresentation.hpp>

int main()
{
    try
    {
        css::uno::Reference<css::uno::XComponentContext> xComponentContext = cppu::bootstrap();
        css::uno::Reference<css::lang::XMultiComponentFactory> xMultiComponentFactory = xComponentContext->getServiceManager();

        css::uno::Reference<css::frame::XDesktop> xDesktop(xMultiComponentFactory->createInstanceWithContext("com.sun.star.frame.Desktop", xComponentContext), css::uno::UNO_QUERY);
        if (!xDesktop.is()) {
            printf("XDesktop could not instantiate.\n");
            return 1;
        }

        css::uno::Reference<css::frame::XComponentLoader> xComponentLoader(xDesktop, css::uno::UNO_QUERY);
        if (!xComponentLoader.is()) {
            printf("XComponentLoader could not instantiate.\n");
            return 1;
        }

        rtl::OUString sOutputUrl, sAbsoluteOutputUrl, sWorkingDir;

        osl_getProcessWorkingDir(&sWorkingDir.pData);

        osl::FileBase::getFileURLFromSystemPath("spreadsheet.pdf", sOutputUrl);
        osl::FileBase::getAbsoluteFileURL(sWorkingDir, sOutputUrl, sAbsoluteOutputUrl);
        std::cout << sAbsoluteOutputUrl << std::endl;

        css::uno::Sequence<css::beans::PropertyValue> loadProperties(1);
        loadProperties[0].Name = "Hidden";
        loadProperties[0].Value <<= true;
        // L"file:\\D:\\resources\\example.xlsx"
        css::uno::Reference<css::lang::XComponent> xComponent = xComponentLoader->loadComponentFromURL(L"file:\\D:\\resources\\sheet.xlsx", "_blank", 0, loadProperties);
        css::uno::Reference<css::frame::XStorable> xStorable(xComponent, css::uno::UNO_QUERY_THROW);
        css::uno::Sequence<css::beans::PropertyValue> storeProps(3);
        storeProps[0].Name = "FilterName";
        storeProps[0].Value <<= rtl::OUString("calc_pdf_Export");
        storeProps[1].Name = "Overwrite";
        storeProps[1].Value <<= true;
        storeProps[2].Name = "SelectPdfVersion";
        storeProps[2].Value <<= sal_Int32(1);
        xStorable->storeToURL(sAbsoluteOutputUrl, storeProps);
    }
    catch (css::uno::Exception& e)
    {
        std::cout << e.Message << std::endl;
        return 1;
    }

    return 0;
}