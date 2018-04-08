#ifndef DL_MGR_IO_H
#define DL_MGR_IO_H

#define HASFLAG(val, flag) ((val) & (flag)) == flag

#include "stream.h"
#include <iomanip>
#include <chrono>
#include <string>


namespace dl_mgr {
    namespace io {

        enum ts_options
        {
            TIME = 1 << 0,
            DATE = 1 << 1,
            TWELVE_HR = 1 << 2
        };


        inline ts_options operator|
            (const ts_options &,
             const ts_options &);

        inline os_manip<std::tm, ts_options> timestamp
            (std::tm &&, ts_options &&);

        inline os_manip<const std::string> brackets
            (const std::string &&s);

        inline os_manip<const std::string> braces
            (const std::string &&s);

        inline os_manip<const std::string, const std::string
        > put_kvp
            (const std::string &&key,
             const std::string &&val);


        class logger
        {
            struct time
            {
                operator std::tm() const
                    {
                    auto sysTime =
                        std::chrono::system_clock::to_time_t
                        (std::chrono::system_clock::now());
                    std::tm localTime{ };
                    localtime_s(&localTime, &sysTime);
                    return localTime;
                    }
            };


            time m_time;
            std::ostream *m_out;
        public:

            explicit logger(std::ostream *out);

            void write
                (const std::string &s,
                 ts_options &&options = TIME) const;
        };
    }
}

#endif
