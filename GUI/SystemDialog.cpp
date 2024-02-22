#include <iostream>

#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
#include <com/sun/star/ui/dialogs/XFolderPicker2.hpp>
#include <com/sun/star/util/thePathSettings.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#include <com/sun/star/ui/dialogs/ExtendedFilePickerElementIds.hpp>
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#include <com/sun/star/util/XPathSettings.hpp>

int main()
{
    try {
        css::uno::Reference<css::uno::XComponentContext> ctx = cppu::bootstrap();
        if (ctx != nullptr)
            std::cout << "Connected to a running office ...\n";
        css::uno::Reference<css::lang::XMultiComponentFactory> mcf = ctx->getServiceManager();

        /* raiseSaveAsDialog */
        
        // the filepicker is instantiated with the global Multicomponentfactory...
        css::uno::Reference<css::ui::dialogs::XFilePicker> file_picker(mcf->createInstanceWithContext("com.sun.star.ui.dialogs.FilePicker", ctx), css::uno::UNO_QUERY);

        // the default name is the initially proposed filename...
        file_picker->setDefaultName("MyExampleDocument");

        // set the initial displaydirectory. In this example the user template directory is used
        css::uno::Reference<css::uno::XInterface> path_settings = mcf->createInstanceWithContext("com.sun.star.util.PathSettings", ctx);

        css::uno::Reference<css::beans::XPropertySet> property_set(path_settings, css::uno::UNO_QUERY);
        rtl::OUString template_url;
        property_set->getPropertyValue("Template_writable") >>= template_url;
        file_picker->setDisplayDirectory(template_url);

        // set the filters of the dialog. The filternames may be retrieved from
        // http://wiki.openoffice.org/wiki/Framework/Article/Filter
        css::uno::Reference<css::ui::dialogs::XFilterManager> filter_manager(file_picker, css::uno::UNO_QUERY);
        filter_manager->appendFilter("OpenDocument Text Template", "writer8_template");
        filter_manager->appendFilter("OpenDocument Text", "writer8");

        // choose the template that defines the capabilities of the filepicker dialog
        css::uno::Reference<css::lang::XInitialization> initialization(file_picker, css::uno::UNO_QUERY);
        css::uno::Sequence<css::uno::Any> any_list(1);
        any_list[0] = css::uno::Any(css::ui::dialogs::TemplateDescription::FILESAVE_AUTOEXTENSION);
        initialization->initialize(any_list);

        // add a control to the dialog to add the extension automatically to the filename...
        css::uno::Reference<css::ui::dialogs::XFilePickerControlAccess> file_picker_control_access(file_picker, css::uno::UNO_QUERY);
        file_picker_control_access->setValue(css::ui::dialogs::ExtendedFilePickerElementIds::CHECKBOX_AUTOEXTENSION, 0, css::uno::Any(true));
        
        css::uno::Reference<css::lang::XComponent> component(file_picker, css::uno::UNO_QUERY);

        // execute the dialog...
        css::uno::Reference<css::ui::dialogs::XExecutableDialog> execuable_dialog(file_picker, css::uno::UNO_QUERY);
        sal_Int16 result = execuable_dialog->execute();
        
        // query the resulting path of the dialog...
        rtl::OUString store_path;
        if (result == css::ui::dialogs::ExecutableDialogResults::OK) {
            css::uno::Sequence<rtl::OUString> path_lists = file_picker->getFiles();
            if (path_lists.getLength() > 0)
                store_path = path_lists[0];
        }
        
        /* raiseFolderPicker */

        // retrieve the configured Work path...
        rtl::OUString work_url;
        property_set->getPropertyValue("Work") >>= work_url;
        
        // instantiate the folder picker and retrieve the necessary interfaces
        css::uno::Reference<css::ui::dialogs::XFolderPicker2> folder_picker2(mcf->createInstanceWithContext("com.sun.star.ui.dialogs.FolderPicker", ctx),
                                                                             css::uno::UNO_QUERY);
        execuable_dialog = css::uno::Reference<css::ui::dialogs::XExecutableDialog>(folder_picker2, css::uno::UNO_QUERY);
        component = css::uno::Reference<css::lang::XComponent>(folder_picker2, css::uno::UNO_QUERY);
        folder_picker2->setDisplayDirectory(work_url);
        // set the dialog title...
        folder_picker2->setTitle("My Title");
        // show the dialog
        result = execuable_dialog->execute();

        // User has clicked "Select" button...
        rtl::OUString return_folder;
        if (result == css::ui::dialogs::ExecutableDialogResults::OK)
            return_folder = folder_picker2->getDirectory();

    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return 1;
    }

    return 0;
}
