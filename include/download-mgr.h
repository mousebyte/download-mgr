#ifndef DOWNLOAD_MGR_H
#define DOWNLOAD_MGR_H

#include <filesystem>
#include <map>
#include <iomanip>
#include "io.h"


namespace dl_mgr {

    //! The name of this application.
    static const char *application_name = "download-mgr";
    //! The name of the configuration file.
    static const char *config_file = "download-mgr.cfg";

    namespace fs = std::experimental::filesystem;
    typedef fs::path path_t;
    using dir_it = fs::directory_iterator;
    using dir_entry = fs::directory_entry;


    enum options
    {
        NONE,
        HELP,
        CFG,
        ADD,
        SRC
    };


    //! Contains a map of file extensions to their destination
    //! directories and provides methods to help access it.
    class extension_map
    {
        std::map<path_t, path_t> m_map;
    public:
        typedef std::map<path_t, path_t> map_t;
        using iterator = map_t::iterator;
        using const_iterator = map_t::const_iterator;

        path_t& at
            (const std::string &key);

        path_t resolve_tgt
            (const dir_entry &file);

        path_t& operator[]
            (const std::string &key);

        path_t& operator[]
            (std::string &key);

        iterator begin();

        iterator end();

        const_iterator begin() const;

        const_iterator end() const;
    };


    //! Holds configuration information for the program.
    typedef struct _dl_mgr_config
    {
        path_t SourceDir;
        extension_map Map;

        friend std::ostream& operator<<
            (std::ostream &, const _dl_mgr_config &);
        friend std::istream& operator>>
            (std::istream &, _dl_mgr_config &);
    } config_t;


    class program
    {
        io::logger log;
        config_t m_config;
    public:
        explicit program(std::ostream * = &std::cout);
        void set_src_dir(const std::string &);
        void edit_ext_entry
            (const std::string &, const std::string &);
        void run(const std::string & = "");
    };


    void read_config_file(const char *, config_t &);
    void write_config_file(const char *, const config_t &);
}
#endif
