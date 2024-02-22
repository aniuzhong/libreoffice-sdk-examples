#pragma once

#include <com/sun/star/lang/XMultiComponentFactory.hpp>

/**
 * Support ONE singleton uno connection to running a office installation.
 * Use to open/use/close connection to uno enviroment of an already running office
 * Ctor isn't available from outside. You should call static function "instance"
 * to open or use internal set conncetion which is created one times only.
 */
class OfficeConnection final {
private:
    // Disallow instantiation outside of the class.
    OfficeConnection(); 

public:
    OfficeConnection(const OfficeConnection&) = delete;
    OfficeConnection& operator=(const OfficeConnection &) = delete;
    OfficeConnection(OfficeConnection &&) = delete;
    OfficeConnection & operator=(OfficeConnection &&) = delete;

    // Creating a singleton is to rely on C++11 Magic Statics (N2660).
    // Exceptions such as css::uno::RuntimeException could be thrown.
    static auto& instance()
    {
        static OfficeConnection connection;
        return connection;
    }
    
    auto manager() const noexcept { return m_manager; }
    auto context() const noexcept { return m_context; }

    template <typename T>
    css::uno::Reference<T> qurey(rtl::OUString spec) noexcept
    {
        try {
            css::uno::Reference<T> instance(m_manager->createInstanceWithContext(spec, m_context),
                                            css::uno::UNO_QUERY);
            return instance;
        } catch (...) {
            fprintf(stderr, "Could not create service of type\n"); 
            return nullptr;
        }
    }

private:
    css::uno::Reference<css::lang::XMultiComponentFactory> m_manager;
    css::uno::Reference<css::uno::XComponentContext>       m_context;
};

