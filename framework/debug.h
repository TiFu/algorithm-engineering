#pragma once

#ifndef NDEBUG
#include <iostream>
#include <cassert>
#include <memory>
#include <mutex>

struct safe_ostream {
    struct guarded_impl {
        guarded_impl() = delete;

        guarded_impl(const guarded_impl &) = delete;

        void operator=(const guarded_impl &) = delete;

        guarded_impl(std::ostream &ostream, std::mutex &mutex) : ostream_(ostream), guard_(mutex) {
        }

        ~guarded_impl() {
            ostream_.flush();
        }

        template<typename T>
        void write(const T &x) {
            ostream_ << x;
        }

        std::ostream &ostream_;
        std::lock_guard<std::mutex> guard_;
    };

    struct impl {
        impl() = delete;

        void operator=(const impl &) = delete;

        impl(std::ostream &ostream, std::mutex &mutex) : unique_impl_(new guarded_impl(ostream, mutex)) {
        }

        impl(const impl &rhs) {
            assert(rhs.unique_impl_.get());
            unique_impl_.swap(rhs.unique_impl_);
        }

        template<typename T>
        impl &operator<<(const T &x) {
            guarded_impl *p = unique_impl_.get();
            assert(p);
            p->write(x);
            return *this;
        }

        mutable std::unique_ptr<guarded_impl> unique_impl_;
    };

    explicit safe_ostream(std::ostream &ostream) : ostream_(ostream) {
    }

    template<typename T>
    impl operator<<(const T &x) {
        return impl(ostream_, mutex_) << x;
    }

    std::ostream &ostream_;
    std::mutex mutex_;
};

static safe_ostream safe_cout(std::cout);
static safe_ostream safe_cerr(std::cerr);
#define dout(obj) do { \
   safe_cerr << "" <<  __FILE__ << ":" << __LINE__ << ": " << obj; \
} while(false)
#else
/**
 * for debug statements
 */
#define dout(obj) do {} while(false)
#endif
