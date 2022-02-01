#pragma once
#ifndef VICTORTUTORIAL_REF_H
#define VICTORTUTORIAL_REF_H

#include <memory>

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> MakeRef(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#endif //VICTORTUTORIAL_REF_H
