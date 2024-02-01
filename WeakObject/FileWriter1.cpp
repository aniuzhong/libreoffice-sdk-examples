#include <cstdio>

#include <cppuhelper/weak.hxx>
#include <cppuhelper/queryinterface.hxx>
#include <com/sun/star/io/XOutputStream.hpp>

class OFileWriter : public cppu::OWeakObject,
                    public css::io::XOutputStream
{
public:
    OFileWriter(const char *filename)
        : m_f(::fopen(filename, "w"))
    {
    }

public: // XInterface implementation
    virtual css::uno::Any SAL_CALL queryInterface(const css::uno::Type& type) throw() override 
    {
        css::uno::Any a = cppu::queryInterface(type, static_cast<css::io::XOutputStream *>(this));
        if (a.hasValue())
            return a;
        return OWeakObject::queryInterface(type);
    }

    virtual void SAL_CALL acquire() noexcept override { OWeakObject::acquire(); }
    virtual void SAL_CALL release() noexcept override { OWeakObject::release(); }

public: // XOutputStream implementation
    virtual void SAL_CALL writeBytes(const css::uno::Sequence<sal_Int8> &data) throw() override
    {
        ::fwrite(data.getConstArray(), 1, data.getLength(), m_f);
    }

    virtual void SAL_CALL flush(void) throw() override  
    {
        ::fflush(m_f);
    }

    virtual void SAL_CALL closeOutput(void) throw() override 
    {
        ::fclose(m_f);
    }

private:
    FILE *m_f;
};

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1;

    const char *filename = argv[1];

    OFileWriter writer(filename);

    css::uno::Sequence<sal_Int8> str(5);
    str[0] = 'h';
    str[1] = 'e';
    str[2] = 'l';
    str[3] = 'l';
    str[4] = 'o';
    
    writer.writeBytes(str);
    writer.flush();
    writer.closeOutput();

    return 0;
}
