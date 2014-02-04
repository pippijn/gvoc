#ifndef CLASSREGISTRY_H
#define CLASSREGISTRY_H

#include <QMap>
#include <QSharedPointer>

template<typename BaseClass>
class ClassRegistry
{
    struct BaseConstructor
    {
        virtual ~BaseConstructor() { }
        virtual BaseClass *construct() const = 0;
    };

    template<typename DerivedClass>
    struct DerivedConstructor : BaseConstructor
    {
        DerivedClass *construct() const
        {
            return new DerivedClass;
        }
    };

    typedef QMap<QString, BaseConstructor *> ConstructorMap;
    static ConstructorMap &constructors()
    {
        static ConstructorMap constructors;
        return constructors;
    }

    template<typename DerivedClass>
    struct RegisterClass
    {
        RegisterClass(QString name)
            : name(name)
        {
            constructors().insert(name, new DerivedConstructor<DerivedClass>);
        }

        ~RegisterClass()
        {
            delete constructors()[name];
            constructors().remove(name);
        }

        void constructorCalled() { }
        void destructorCalled() { }

    private:
        QString name;
    };

public:
    template<typename DerivedClass>
    class Derived : public BaseClass
    {
        static RegisterClass<DerivedClass> classInit;

    protected:
        Derived()
        {
            classInit.constructorCalled();
        }

        ~Derived()
        {
            classInit.destructorCalled();
        }
    };

    static BaseClass *create(QString name)
    {
        typename ConstructorMap::const_iterator found = constructors().find(name);
        if (found == constructors().end())
            return NULL;

        return found.value()->construct();
    }
};

template<typename BaseClass>
template<typename DerivedClass>
typename ClassRegistry<BaseClass>::template RegisterClass<DerivedClass> ClassRegistry<BaseClass>::Derived<DerivedClass>::classInit(DerivedClass::className);

#endif // CLASSREGISTRY_H
