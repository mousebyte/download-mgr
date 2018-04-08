#ifndef DL_MGR_STREAM_H
#define DL_MGR_STREAM_H
#include <iostream>


namespace dl_mgr {
    namespace io {

        template<int N, int... Rest>
        struct seq { };


        template<int N, int... Rest>
        struct gseq : gseq<N - 1, N - 1, Rest...> { };


        template<int... Is>
        struct gseq<0, Is...> : seq<Is...> { };


        template<typename TStream, typename... Args>
        struct _ios_man
        {
            typedef TStream stream_t;
            typedef stream_t& (*func_t)
                (stream_t &, Args &&...);

            stream_t& operator()(stream_t &s);

            explicit _ios_man
                (const func_t &&f, Args &&... args);
        private:
            std::tuple<Args&...> m_values;
            func_t m_fn;
            template<int... Is>
            stream_t& exec(stream_t &s, seq<Is...> seq);
        };


        template<typename... Args>
        using os_manip = _ios_man<std::ostream, Args...>;
        template<typename... Args>
        using is_manip = _ios_man<std::istream, Args...>;

        template<typename Ch, typename Tr, typename... Args>
        std::basic_ostream<Ch, Tr>& operator<<
            (std::basic_ostream<Ch, Tr> &os,
             os_manip<Args...> &&m)
            {
            return m(os);
            }

        template<typename Ch, typename Tr, typename... Args>
        std::basic_istream<Ch, Tr>& operator>>
            (std::basic_istream<Ch, Tr> &is,
             is_manip<Args...> &&m)
            {
            return m(is);
            }
    }
}
#endif
