//
// Created by rui ding on 2025/9/11.
//
#include <unistd.h>
#include <cstdio>
#include <dirent.h>
#include <functional>

/// 通用RAII资源管理类
template <typename T>
class ResourceGuard
{
public:
    // 资源回收清理函数
    using Deleter = std::function<void(T)>;

    /**
     * @param resource 资源
     * @param deleter 清理函数
     */
    ResourceGuard(T resource = T(), Deleter deleter = nullptr) : resource_(resource), deleter_(deleter) {}

    ~ResourceGuard() { reset(); }

    // 禁止拷贝，避免重复释放
    ResourceGuard(const ResourceGuard&)            = delete;
    ResourceGuard& operator=(const ResourceGuard&) = delete;

    // 允许移动
    ResourceGuard(ResourceGuard&& other) noexcept : resource_(other.resource_), deleter_(std::move(other.deleter_))
    {
        other.resource_ = T();
    }

    ResourceGuard& operator=(ResourceGuard&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            resource_       = other.resource_;
            deleter_        = std::move(other.deleter_);
            other.resource_ = T();
        }
        return *this;
    }

    /// 获取底层资源
    T get() const { return resource_; }

    /// 释放旧资源并绑定新资源
    void reset(T new_resource = T(), Deleter new_deleter = nullptr)
    {
        if (resource_ != T() && deleter_)
        {
            deleter_(resource_);
        }
        resource_ = new_resource;
        if (new_deleter)
        {
            deleter_ = new_deleter;
        }
    }

    /// 放弃管理，返回资源
    T release()
    {
        T tmp     = resource_;
        resource_ = T();
        return tmp;
    }

private:
    /// 管理着的资源
    T resource_;
    /// 资源的释放回收函数
    Deleter deleter_;
};