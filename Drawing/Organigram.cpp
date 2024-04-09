#include <iostream>

#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/text/XText.hpp>

int main()
{
    try {
        // Get the remote office context of a running office
        css::uno::Reference<css::uno::XComponentContext> ctx = cppu::bootstrap();
        css::uno::Reference<css::lang::XMultiComponentFactory> factory = ctx->getServiceManager();

        css::uno::Reference<css::uno::XInterface> desktop = factory->createInstanceWithContext("com.sun.star.frame.Desktop", ctx);
        css::uno::Reference<css::frame::XComponentLoader> loader(desktop, css::uno::UNO_QUERY);

        css::uno::Sequence<css::beans::PropertyValue> load_props(0);
        css::uno::Reference<css::lang::XComponent> draw_doc = loader->loadComponentFromURL("private:factory/sdraw", "_blank", 0, load_props);

        // Get draw page by index
        css::uno::Reference<css::drawing::XDrawPagesSupplier> pages_supplier(draw_doc, css::uno::UNO_QUERY);
        css::uno::Reference<css::drawing::XDrawPages> pages = pages_supplier->getDrawPages();
        css::uno::Reference<css::drawing::XDrawPage> page(pages->getByIndex(0), css::uno::UNO_QUERY);

        // Find out page dimensions
        css::uno::Reference<css::beans::XPropertySet> page_props(page, css::uno::UNO_QUERY);

        sal_Int64 page_width, page_height;
        sal_Int64 page_border_top, page_border_left, page_border_right;
        page_props->getPropertyValue("Width") >>= page_width;
        page_props->getPropertyValue("Height") >>= page_height;
        page_props->getPropertyValue("BorderTop") >>= page_border_top;
        page_props->getPropertyValue("BorderLeft") >>= page_border_left;
        page_props->getPropertyValue("BorderRight") >>= page_border_right;

        sal_Int64 draw_width = page_width - page_border_left - page_border_right;
        sal_Int64 hor_center = page_border_left + draw_width / 2;

        // Data for organigram
        rtl::OUString org_units[2][4];
        org_units[0][0] = "Management";  // level 0
        org_units[1][0] = "Production";  // level 1
        org_units[1][1] = "Purchasing";  // level 1
        org_units[1][2] = "IT Services"; // level 1
        org_units[1][3] = "Sales";       // level 1
        sal_Int64 level_count[2] = {1, 4};

        // Calculate shape sizes and positions
        sal_Int64 hor_space = 300;
        sal_Int64 ver_space = 3000;
        sal_Int32 shape_width = (draw_width - (level_count[1] - 1) * hor_space) / level_count[1];
        sal_Int32 shape_height = page_height / 20;
        sal_Int32 shape_x = page_width / 2 - shape_width / 2;
        sal_Int32 level_y = 0;
        css::uno::Reference<css::drawing::XShape> start_shape = nullptr;

        // Get document factory
        css::uno::Reference<css::lang::XMultiServiceFactory> doc_factory(draw_doc, css::uno::UNO_QUERY);

        // Creating and adding RecangleShapes and Connectors
        for (int level = 0; level <= 1; ++level) {
            level_y = page_border_top + 2000 + level * (shape_height + ver_space);
            for (int i = level_count[level] - 1; i > -1; --i) {
                shape_x = hor_center - (level_count[level] * shape_width + (level_count[level] - 1) * hor_space) / 2 + i * shape_width + i * hor_space;
                css::uno::Reference<css::drawing::XShape> shape(doc_factory->createInstance("com.sun.star.drawing.RectangleShape"), css::uno::UNO_QUERY);
                shape->setPosition({shape_x, level_y});
                shape->setSize({shape_width, shape_height});
                page->add(shape);

                // Set the text
                css::uno::Reference<css::text::XText> text(shape, css::uno::UNO_QUERY);
                text->setString(org_units[level][i]);

                // memorize the root shape, for connectors
                if (level == 0 && i == 0)
                    start_shape = shape;

                // add connectors for level 1
                if (level == 1) {
                    doc_factory->createInstance("com.sun.star.drawing.ConnectorShape");
                    css::uno::Reference<css::drawing::XShape> connector(doc_factory->createInstance("com.sun.star.drawing.ConnectorShape"), css::uno::UNO_QUERY);
                    page->add(connector);
                    css::uno::Reference<css::beans::XPropertySet> connector_props(connector, css::uno::UNO_QUERY);
                    connector_props->setPropertyValue("StartShape", css::uno::Any(start_shape));
                    connector_props->setPropertyValue("EndShape", css::uno::Any(shape));
                    // Glue point positions: 0=top 1=left 2=bottom 3=right
                    connector_props->setPropertyValue("StartGluePointIndex", css::uno::Any(sal_Int64(2)));
                    connector_props->setPropertyValue("EndGluePointIndex", css::uno::Any(sal_Int64(0)));
                }
            }
        }
    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return 1;
    }

    return 0;
}
