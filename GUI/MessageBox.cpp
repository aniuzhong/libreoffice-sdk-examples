#include <iostream>

#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/awt/MessageBoxType.hpp>
#include <com/sun/star/awt/XMessageBox.hpp>
#include <com/sun/star/awt/MessageBoxButtons.hpp>
#include <com/sun/star/awt/XMessageBoxFactory.hpp>
#include <com/sun/star/awt/XVclWindowPeer.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>

// helper method to get the window peer of a document or if no
// document is specified it tries to get the active frame
// which is potentially dangerous
static css::uno::Reference<css::awt::XWindowPeer> getWindowPeerOfFrame(css::uno::Reference<css::lang::XComponent> comp)
{
    try {
        css::uno::Reference<css::frame::XModel> model(comp,  css::uno::UNO_QUERY);
        css::uno::Reference<css::frame::XFrame> frame = model->getCurrentController()->getFrame();
        css::uno::Reference<css::awt::XWindow> window = frame->getContainerWindow();
        css::uno::Reference<css::awt::XWindowPeer> window_peer(window, css::uno::UNO_QUERY);
        return window_peer;
    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
    }

    return nullptr;
}

/** @param vcl_window_peer the windowpeer of a dialog control or the dialog itself
 *  @return true if HighContrastMode is activated or false if HighContrastMode is deactivated
 */
static bool isHighContrastModeActivated(css::uno::Reference<css::awt::XVclWindowPeer> vcl_window_peer)
{
    bool is_activated = false;

    try {
        sal_Int32 ui_color;
        vcl_window_peer->getProperty("DisplayBackgroundColor") >>= ui_color;
        int red = ui_color / 65536;
        int green = (ui_color % 65536) / 256;
        int blue = (ui_color % 65536) %  256;
        int luminance = ((blue*28 + green*151 + red*77 ) / 256 );
        is_activated = (luminance <= 25);
    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
    }

    return is_activated;
}

int main()
{
    try {
        css::uno::Reference<css::uno::XComponentContext> ctx = cppu::bootstrap();
        if (ctx != nullptr)
            std::cout << "Connected to a running office ...\n";
        css::uno::Reference<css::lang::XMultiComponentFactory> mcf = ctx->getServiceManager();

        // createDefaultTextDocument()
        css::uno::Reference<css::frame::XDesktop> desktop(mcf->createInstanceWithContext("com.sun.star.frame.Desktop", ctx), css::uno::UNO_QUERY);
        css::uno::Reference<css::frame::XComponentLoader> loader(desktop, css::uno::UNO_QUERY);
        css::uno::Sequence<css::beans::PropertyValue> args(1);
        args[0].Name = "Hidden";
        args[0].Value = css::uno::Any(true);
        css::uno::Reference<css::lang::XComponent> comp = loader->loadComponentFromURL("private:factory/swriter", "_blank", 0, args);

        css::uno::Reference<css::awt::XWindowPeer> window_peer = getWindowPeerOfFrame(comp);
        css::uno::Reference<css::awt::XVclWindowPeer> vcl_window_peer(window_peer, css::uno::UNO_QUERY);
        

        rtl::OUString is_activated = isHighContrastModeActivated(vcl_window_peer) ? rtl::OUString("true") : rtl::OUString("false");
        
        css::uno::Reference<css::awt::XToolkit> toolkit(mcf->createInstanceWithContext("com.sun.star.awt.Toolkit", ctx), css::uno::UNO_QUERY);
        css::uno::Reference<css::awt::XMessageBoxFactory> factory(toolkit, css::uno::UNO_QUERY);
        css::uno::Reference<css::awt::XMessageBox> message_box = factory->createMessageBox(window_peer, css::awt::MessageBoxType_ERRORBOX,
                                                                                           css::awt::MessageBoxButtons::BUTTONS_OK, "My Sampletitle",
                                                                                           rtl::OUString("HighContrastMode is enabled: ")+is_activated);
        message_box->execute();
    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return 1;
    }

    return 0;
}
