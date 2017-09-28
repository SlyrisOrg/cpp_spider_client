//
// Created by roman sztergbaum on 29/09/2017.
//

#ifndef SPIDER_CLIENT_LAZYSINGLETON_HPP
#define SPIDER_CLIENT_LAZYSINGLETON_HPP

#include <memory>
#include <mutex>
#include <utils/NonCopyable.hpp>

namespace utils
{
    template <class T>
    class LazySingleton : private utils::NonCopyable
    {
    public:
        template <typename... Args>
        static T &getInstance(Args &&... args)
        {

            std::call_once(__getOnceFlag(),
                           [](Args &&... args) {
                               _instance.reset(new T(std::forward<Args>(args)...));
                           }, std::forward<Args>(args)...);

            return *_instance;
        }

    protected:
        LazySingleton() = default;

        ~LazySingleton() override = default;

    private:
        static std::once_flag &__getOnceFlag() noexcept
        {
            static std::once_flag _once;
            return _once;
        }

    private:
        static std::unique_ptr<T> _instance;
    };
}

template <class T> std::unique_ptr<T> utils::LazySingleton<T>::_instance = nullptr;

#endif //SPIDER_CLIENT_LAZYSINGLETON_HPP
