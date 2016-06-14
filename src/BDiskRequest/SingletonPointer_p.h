#ifndef SINGLETONPOINTER_P_H
#define SINGLETONPOINTER_P_H

#include <cstddef>
#include <QDebug>

#include <QtCore/QtGlobal>
#include <QtCore/QAtomicInt>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QThreadStorage>
#include <QtCore/QThread>
#include <QtCore/QtGlobal>
#include <QtCore/QScopedPointer>

namespace CallOnce {
enum ECallOnce {
    CO_Request,
    CO_InProgress,
    CO_Finished
};
}

Q_GLOBAL_STATIC(QThreadStorage<QAtomicInt*>, once_flag)

template <class Function>
inline static void qCallOnce_p(Function func, QBasicAtomicInt& flag)
{
    using namespace CallOnce;

#if QT_VERSION < 0x050000
    int protectFlag = flag.fetchAndStoreAcquire(flag);
#endif
#if QT_VERSION >= 0x050000
    int protectFlag = flag.fetchAndStoreAcquire(flag.load());
#endif

//   qDebug()<<"protectFlag is "<<protectFlag;

    if (protectFlag == CO_Finished)
        return;
    if (protectFlag == CO_Request && flag.testAndSetRelaxed(protectFlag,
                                                            CO_InProgress)) {
        func();
        flag.fetchAndStoreRelease(CO_Finished);
    }
    else {
        do {
            QThread::yieldCurrentThread();
        }
        while (!flag.testAndSetAcquire(CO_Finished, CO_Finished));
    }
}

template <class Function>
inline static void qCallOncePerThread_p(Function func)
{
    using namespace CallOnce;
    if (!once_flag()->hasLocalData()) {
        once_flag()->setLocalData(new QAtomicInt(CO_Request));
        qCallOnce_p(func, *once_flag()->localData());
    }
}


template <class T>
class SingletonPointer
{
public:
//    static T& instance()
//    {
//        qCallOnce(init, flag);
//        return *tptr;
//    }
    static T* instance()
    {
        qCallOnce_p(init, flag);
        return tptr.data ();
    }
    static void init()
    {
        qDebug()<<"SingletonPointer init";
        tptr.reset(new T);
    }

private:
    SingletonPointer() {

    }
    ~SingletonPointer() {

    }
    Q_DISABLE_COPY(SingletonPointer)

    static QScopedPointer<T> tptr;
    static QBasicAtomicInt flag;
};

template<class T> QScopedPointer<T> SingletonPointer<T>::tptr(0);
template<class T> QBasicAtomicInt SingletonPointer<T>::flag
= Q_BASIC_ATOMIC_INITIALIZER(CallOnce::CO_Request);

#endif // SINGLETONPOINTER_P_H

