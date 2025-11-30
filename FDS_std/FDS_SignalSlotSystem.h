/*
        Copyright(C) 2025 Fordans
                        This source follows the GPL licence
                        See https://www.gnu.org/licenses/gpl-3.0.html for details
*/

#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>
#include <cstddef>

namespace fds
{
    template <typename... Args>
    class Signal
    {
    public:
        using Slot = std::function<void(Args...)>;

        class Connection
        {
            friend class Signal;
            Signal *sig_ = nullptr;
            std::size_t id_ = 0;
            Connection(Signal *s, std::size_t id) : sig_(s), id_(id) {}

        public:
            Connection() = default;
            bool connected() const
            {
                return sig_ && sig_->slots_.find(id_) != sig_->slots_.end();
            }
            void disconnect()
            {
                if (sig_)
                {
                    sig_->disconnect(id_);
                    sig_ = nullptr;
                    id_ = 0;
                }
            }
        };

        class ScopedConnection
        {
        public:
            ScopedConnection() = default;
            explicit ScopedConnection(Connection c) : c_(std::move(c)) {}
            ScopedConnection(const ScopedConnection &) = delete;
            ScopedConnection &operator=(const ScopedConnection &) = delete;
            ScopedConnection(ScopedConnection &&o) noexcept : c_(std::move(o.c_)) { o.c_ = {}; }
            ScopedConnection &operator=(ScopedConnection &&o) noexcept
            {
                if (this != &o)
                {
                    reset();
                    c_ = std::move(o.c_);
                    o.c_ = {};
                }
                return *this;
            }
            ~ScopedConnection() { reset(); }
            void reset()
            {
                if (c_.connected())
                    c_.disconnect();
                c_ = {};
            }
            bool connected() const { return c_.connected(); }
            Connection release()
            {
                Connection tmp = c_;
                c_ = {};
                return tmp;
            }

        private:
            Connection c_;
        };

    public:
        Signal() = default;

        Connection connect(Slot slot)
        {
            const std::size_t id = ++next_id_;
            slots_.emplace(id, SlotData{std::move(slot)});
            return Connection(this, id);
        }

        // non-const member function
        template <typename T, typename... A>
        Connection connect_weak(std::weak_ptr<T> wptr, void (T::*mf)(A...))
        {
            static_assert(std::is_invocable_r_v<void, decltype(mf), T *, Args...>,
                          "The member function signature does not match the Signal parameter");
            return connect([wptr, mf](Args... args)
                           {
        if (auto sp = wptr.lock()) {
            (sp.get()->*mf)(std::forward<Args>(args)...);
        } });
        }

        // const member function
        template <typename T, typename... A>
        Connection connect_weak(std::weak_ptr<T> wptr, void (T::*mf)(A...) const)
        {
            static_assert(std::is_invocable_r_v<void, decltype(mf), const T *, Args...>,
                          "The member function signature does not match the Signal parameter");
            return connect([wptr, mf](Args... args)
                           {
        if (auto sp = wptr.lock()) {
            (sp.get()->*mf)(std::forward<Args>(args)...);
        } });
        }

        void emit(Args... args)
        {
            // enable disconnect in callbacks
            std::vector<std::size_t> ids;
            ids.reserve(slots_.size());
            for (auto &kv : slots_)
                ids.push_back(kv.first);

            for (auto id : ids)
            {
                auto it = slots_.find(id);
                if (it != slots_.end())
                {
                    it->second.slot(std::forward<Args>(args)...);
                }
            }
        }

        void disconnect(std::size_t id)
        {
            slots_.erase(id);
        }

        void disconnect_all()
        {
            slots_.clear();
        }

    private:
        struct SlotData
        {
            Slot slot;
        };

        std::unordered_map<std::size_t, SlotData> slots_;
        std::size_t next_id_ = 0;
    };
}