#include <cstdio>
#include <iostream>

extern "C" {
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
}

#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <com/sun/star/awt/Toolkit.hpp>
#include <com/sun/star/awt/XSystemChildFactory.hpp>
#include <com/sun/star/frame/XFramesSupplier.hpp>
#include <com/sun/star/frame/XFrames.hpp>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/presentation/XPresentationSupplier.hpp>
#include <com/sun/star/presentation/XPresentation2.hpp>

#include "OfficeConnection.h"

void loadDocument(rtl::OUString url, int64_t window)
{
    try {
        auto& connect = OfficeConnection::instance();

        // Create XWindow
        auto toolkit = connect.qurey<css::awt::XToolkit>("com.sun.star.awt.Toolkit");
        css::uno::Reference<css::awt::XSystemChildFactory> factory(toolkit, css::uno::UNO_QUERY);
        css::uno::Reference<css::awt::XWindowPeer> xpeer = factory->createSystemChild(css::uno::Any(window),
                                                                                    css::uno::Sequence<sal_Int8>(),
                                                                                    6);
        if (!xpeer.is()) {
            printf("Could not instantiate XWindowPeer\n");
            return;
        }

        css::uno::Reference<css::awt::XWindow> xwindow(xpeer, css::uno::UNO_QUERY);
        if (!xwindow.is()) {
            printf("Could not instantiate XWindow\n");
            return;
        }

        // Create XFrame
        auto frame = connect.qurey<css::frame::XFrame>("com.sun.star.frame.Frame");
        if (!frame.is()) {
            printf("Could not instantiate XFrame\n");
            return;
        }
        frame->initialize(xwindow);

        auto root = connect.qurey<css::frame::XFramesSupplier>("com.sun.star.frame.Desktop");
        if (!root.is()) {
            printf("Could not instantiate XFramsSupplier\n");
            return;
        }

        auto container = root->getFrames();
        container->append(frame);

        rtl::OUString old_name = frame->getName();
        frame->setName("odk_slideplayer");

        auto loader = connect.qurey<css::frame::XComponentLoader>("com.sun.star.frame.Desktop");
        if (!loader.is()) {
            printf("Could not instantiate XComponentLoader\n");
            return;
        }

        css::uno::Sequence<css::beans::PropertyValue> load_props(0);
        auto document = loader->loadComponentFromURL(url,
                                                    "odk_slideplayer",
                                                    css::frame::FrameSearchFlag::CHILDREN,
                                                    load_props);
        if (!document.is()) {
            printf("Could not load Component from URL\n");
            return;
        }

        frame->setName(old_name);

        xwindow->setVisible(true);
    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return;
    }
}


static void on_button_clicked(GtkWidget* widget, gpointer data)
{
    (void)data;

    int64_t winid = gdk_x11_window_get_xid(gtk_widget_get_window(widget));
    loadDocument("file:///home/aniu/Documents/Presentation.pptx", winid);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *button_box;

    (void)user_data;
    
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW (window), "A Slideplayer based on LibreOffice SDK");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_container_add(GTK_CONTAINER(window), button_box);

    button = gtk_button_new_with_label("Load document");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    gtk_widget_show_all(window);
}

int main (int argc, char** argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("odk.slideplayer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);

    return status;
}

