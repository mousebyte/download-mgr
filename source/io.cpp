#include "io.h"


namespace dl_mgr {
    namespace io {

        template<typename TStream, typename... Args>
        TStream& _ios_man<TStream, Args...>::
        operator()(stream_t &s)
            {
            return exec(s, gseq<sizeof...(Args)>{ });
            }

        template<typename TStream, typename ... Args>
        _ios_man<TStream, Args...>::
        _ios_man(const func_t &&f, Args &&... args)
            : m_values(std::forward<Args&>(args)),
              m_fn(std::move(f)) { }

        template<typename TStream, typename ... Args>
        template<int... Is>
        TStream& _ios_man<TStream, Args...>::
        exec(stream_t &s, seq<Is...> seq)
            {
            return m_fn(s,
                        std::move(
                            std::get<Is>(m_values))...);
            }

        ts_options operator|
            (const ts_options &lhs, const ts_options &rhs)
            {
            return static_cast<ts_options>
            (static_cast<int>(lhs)
             | static_cast<int>(rhs));
            }

        os_manip<std::tm, ts_options> timestamp
            (std::tm &&t, ts_options &&o)
            {
            return os_manip<std::tm, ts_options>(
                []
                (std::ostream &stream, std::tm &&time,
                 ts_options &&options)-> auto&
                    {
                        std::string format;

                        if (HASFLAG(options, DATE))
                            format.append("%m-%d-%Y");
                        if (HASFLAG(options, TIME))
                            {
                            if (!format.empty())
                                format.append(" ");
                            if (HASFLAG(options, TWELVE_HR))
                                format.
                                    append("%I:%M:%S %p");
                            else format.append("%T");
                            }

                        return stream
                               << '['
                               << std::put_time(
                                   &time, format.c_str())
                               << ']';
                    }, std::forward<std::tm&&>(t),
                std::forward<ts_options&&>(o));
            }

        os_manip<const std::string> brackets
            (const std::string &&s)
            {
            return os_manip<const std::string>(
                []
                (std::ostream &stream,
                 const std::string &&str)-> auto&
                    {
                        return stream << '[' << str << ']';
                    }, std::forward<const std::string&&>(
                    s));
            }

        os_manip<const std::string> braces
            (const std::string &&s)
            {
            return os_manip<const std::string>(
                []
                (std::ostream &stream,
                 const std::string &&str)-> auto&
                    {
                        return stream << '{' << str << '}';
                    }, std::forward<const std::string&&>(
                    s));
            }

        os_manip<const std::string, const std::string>
        put_kvp
            (const std::string &&key,
             const std::string &&val)
            {
            return os_manip<
                const std::string, const std::string>(
                []
                (std::ostream &stream,
                 const std::string &&k,
                 const std::string &&v)-> auto&
                    {
                        return
                            stream << k << '=' << v << ';';
                    }, std::forward<const std::string&&>(
                    key),
                std::forward<const std::string&&>(val));
            }

        logger::logger
            (std::ostream *out)
            : m_out(out) {}

        void logger::write
            (const std::string &s,
             ts_options &&options) const
            {
            *m_out
                << timestamp(m_time,
                             std::forward<ts_options&&>(
                                 options))
                << " " << s << std::endl;
            }
    }
}
