//
// Created by igor on 31/12/2020.
//

#ifndef BINADOX_ECAP_DATA_MANAGER_HH
#define BINADOX_ECAP_DATA_MANAGER_HH

#include <memory>
#include <mutex>
#include "backend/data_source.hh"

namespace binadox
{
    class data_manager
    {
    public:
        virtual ~data_manager();

        void initialize();

        static void register_observer(data_source::type_t type, data_source_observer* observer);
        static void unregister_observer(data_source::type_t type, data_source_observer* observer);

        void notify_all() const;
        void notify(data_source::type_t type) const;

        void clear (data_source::type_t type);

        template <data_source::type_t what, typename T>
        void update(const T& data)
        {
            { // start of update mutex scope
                std::lock_guard<std::mutex> g(m_mtx);
                detail::data_source_setter<what>::call(m_data_source.get(), data);
            } // end of update mutex scope
            notify(what);
        }

        std::shared_ptr<data_source> get_data_source () const;
    protected:
        virtual std::unique_ptr<data_source> init() = 0;
    private:
        mutable std::mutex m_mtx;
        std::shared_ptr<data_source> m_data_source;
    };
}

#endif //BINADOX_ECAP_DATA_MANAGER_HH
