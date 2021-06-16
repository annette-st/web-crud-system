//
// Created by igor on 31/12/2020.
//

#include "data_manager.hh"

#include <mutex>
#include <set>
#include <cstdint>
#include <array>

// ===================================================================================

namespace
{
    class registry
    {
    public:
        static registry& instance();

        void add(binadox::data_source::type_t type, binadox::data_source_observer* obs);
        void remove(binadox::data_source::type_t type, binadox::data_source_observer* obs);

        void handle_data_source(binadox::data_source::type_t type, const binadox::data_source* ds) const;
    private:
        registry() = default;
    private:
        mutable std::mutex m_mtx;

        using observer_set_t = std::set<uintptr_t>;

        std::array<observer_set_t, binadox::data_source::eMAX_TYPE> m_observers;
    };
    // -----------------------------------------------------------------------------------------------
    registry& registry::instance()
    {
        static registry inst;
        return inst;
    }
    // -----------------------------------------------------------------------------------------------
    void registry::add(binadox::data_source::type_t type, binadox::data_source_observer* obs)
    {
        std::lock_guard<std::mutex> g(m_mtx);
        const auto key = (uintptr_t)obs;

        if (m_observers[type].find(key) == m_observers[type].end())
        {
            m_observers[type].insert(key);
        }
    }
    // -----------------------------------------------------------------------------------------------
    void registry::remove(binadox::data_source::type_t type, binadox::data_source_observer* obs)
    {
        std::lock_guard<std::mutex> g(m_mtx);
        const auto key = (uintptr_t)obs;

        m_observers[type].erase(key);
    }
    // -----------------------------------------------------------------------------------------------
    void registry::handle_data_source(binadox::data_source::type_t type, const binadox::data_source* ds) const
    {
        std::lock_guard<std::mutex> g(m_mtx);
        for (const auto key : m_observers[type])
        {
            auto obs = (binadox::data_source_observer*)key;
            obs->handle_data_source(type, ds);
        }
    }

} // anon. ns

// ===================================================================================

namespace binadox
{
    data_manager::~data_manager()
    {

    }
    // -------------------------------------------------------------------------------
    void data_manager::register_observer(data_source::type_t type, data_source_observer* observer)
    {
        registry::instance().add(type, observer);
    }
    // -------------------------------------------------------------------------------
    void data_manager::unregister_observer(data_source::type_t type, data_source_observer* observer)
    {
        registry::instance().remove(type, observer);
    }
    // -------------------------------------------------------------------------------
    void data_manager::notify_all() const
    {
        std::lock_guard<std::mutex> g(m_mtx);
        data_source* ds = m_data_source.get();
        for (int i=0; i < data_source::eMAX_TYPE; i++)
        {
            registry::instance().handle_data_source((data_source::type_t)i, ds);
        }
    }
    // -------------------------------------------------------------------------------
    void data_manager::notify(data_source::type_t type) const
    {
        std::lock_guard<std::mutex> g(m_mtx);
        data_source* ds = m_data_source.get();
        registry::instance().handle_data_source(type, ds);
    }
    // -------------------------------------------------------------------------------
    void data_manager::initialize()
    {
        m_data_source = init();
    }
    // -------------------------------------------------------------------------------
    void data_manager::clear (data_source::type_t type)
    {
        std::lock_guard<std::mutex> g(m_mtx);
        m_data_source->clear(type);
    }
    // -------------------------------------------------------------------------------
    std::shared_ptr<data_source> data_manager::get_data_source () const
    {
        return m_data_source;
    }
}