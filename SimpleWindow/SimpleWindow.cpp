#include <cstdio>

#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>

int main()
{
    css::uno::Reference<css::uno::XComponentContext> ctx = cppu::bootstrap();
    css::uno::Reference<css::lang::XMultiComponentFactory> mcf = ctx->getServiceManager();

    css::uno::Reference<css::awt::XToolkit> toolkit(mcf->createInstanceWithContext("com.sun.star.awt.Toolkit", ctx), css::uno::UNO_QUERY);
    if (!toolkit.is()) {
        printf("XToolkit could not instantiate.\n");
        return 1;
    }

    css::uno::Reference<css::frame::XDesktop> desktop(mcf->createInstanceWithContext("com.sun.star.frame.Desktop", ctx), css::uno::UNO_QUERY);
    if (!desktop.is()) {
        printf("XDesktop could not instantiate.\n");
        return 1;
    }

    css::uno::Reference<css::frame::XComponentLoader> loader(desktop, css::uno::UNO_QUERY);
    if (!loader.is()) {
        printf("XComponentLoader could not instantiate.\n");
        return 1;
    }

    css::awt::WindowDescriptor wd;
    wd.Type              = css::awt::WindowClass_TOP;
    wd.WindowServiceName = "";
    wd.ParentIndex       = -1;
    wd.Parent            = toolkit->getDesktopWindow();
    wd.Bounds            = css::awt::Rectangle(100, 200, 300, 400);
    wd.WindowAttributes  = css::awt::WindowAttribute::BORDER |
                           css::awt::WindowAttribute::MOVEABLE |
                           css::awt::WindowAttribute::SIZEABLE;

    css::uno::Reference<css::awt::XWindowPeer> peer = toolkit->createWindow(wd);
    css::uno::Reference<css::awt::XWindow> window(peer, css::uno::UNO_QUERY);
    peer->setBackground(0xFF00FF);

    // At this point, if you stop the program, you will have a new OOo window on the screen,
    // but you cannot do anything with it. You cannot even close it!
    
    css::uno::Reference<css::frame::XFrame> frame(mcf->createInstanceWithContext("com.sun.star.frame.Frame", ctx), css::uno::UNO_QUERY);
    frame->initialize(window);
    css::uno::Reference<css::frame::XFramesSupplier> frame_supplier(desktop, css::uno::UNO_QUERY);
    frame->setCreator(frame_supplier);
    frame->setName("Simple Window");

    window->setVisible(true);

    ::getchar();

    return 0;
}
