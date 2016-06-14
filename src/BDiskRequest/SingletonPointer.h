
#ifdef SINGLETONPOINTER_H
#undef SINGLETONPOINTER_H
#endif

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    #include <QMutex>
    #include <QScopedPointer>
    #include <QObject>

    #define DECLARE_SINGLETON_POINTER(Class) \
    Q_DISABLE_COPY(Class) \
    public: \
        static Class *instance() { \
            static QMutex mutex; \
            static QScopedPointer<Class> sp; \
            if (Q_UNLIKELY(sp.isNull())) { \
                mutex.lock(); \
                if (Q_UNLIKELY(sp.isNull())) \
                    sp.reset(new Class); \
                mutex.unlock(); \
            } \
            return sp.data(); \
        } \
    protected: \
        explicit Class(QObject *parent = 0);

#else
    #include "SingletonPointer_p.h"
    #include <QObject>

    #define DECLARE_SINGLETON_POINTER(Class) \
        Q_DISABLE_COPY(Class) \
        public: \
            static Class *instance() { \
                return SingletonPointer<Class>::instance (); \
            } \
            explicit Class(QObject *parent = 0);
#endif
