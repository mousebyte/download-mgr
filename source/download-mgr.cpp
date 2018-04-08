#include "download-mgr.h"
#include <fstream>


namespace dl_mgr {

    void parse_extension_entries(std::string &&s, extension_map *map)
        {
        //find reference positions in the string
        const auto pos = s.find_first_of('='),
                   end = s.find_first_of(';');
        //stop if the ending delimiter wasn't found
        if (end == std::string::npos) return;
        //add the appropriate substrings to the map
        //and call this function again on the remaining substring
        (*map)[s.substr(0, pos)] = s.substr(pos + 1, end);
        parse_extension_entries(s.substr(end), map);
        }

    std::ostream& operator<<(std::ostream &os, const config_t &in_cfg)
        {
        const auto indent = "    ";
        os << "in_cfg {" << std::endl
            << "dl_directory=" << in_cfg.SourceDir << std::endl
            << "extensions {" << std::endl;
        for (const auto &a : in_cfg.Map) os << indent << a.first << '=' << a.second << ';';
        return os << indent << '}' << std::endl << '}';
        }

    std::istream& operator>>(std::istream &is, config_t &cfg)
        {
        std::string buffer;
        std::getline(is, buffer);
        if (buffer.substr(0, 3) != "cfg") return is;
        std::getline(is, buffer);
        cfg.SourceDir = buffer.substr(buffer.find('=') + 1);
        std::getline(is, buffer);
        if (buffer.substr(0, 10) != "extensions") return is;
        std::getline(is, buffer, '}');
        parse_extension_entries(std::move(buffer), &cfg.Map);
        return is;
        }


    /**
     * \brief Access the path of the destination directory for the
     *  specified file extension. If one does not exist, a new map entry is created.
     * \param key The extension whose destination directory you wish to access.
     * \return A reference to the path representing the extension's target directory.
     */
    path_t& extension_map::at(const std::string &key)
        {
        return m_map[key[0] == '.' ? key : '.' + key];
        }

    /**
     * \brief Resolves the target file path for the specified file.
     * \param file The file whose target you wish to resolve.
     * \return A path representing the file's destination directory
     *  with the file's name appended.
     */
    path_t extension_map::resolve_tgt(const dir_entry &file)
        {
        return m_map[file.path().extension()] / file.path().filename();
        }

    path_t& extension_map::operator[](const std::string &key)
        {
        return at(key);
        }

    path_t& extension_map::operator[](std::string &key)
        {
        return at(key);
        }

    extension_map::iterator extension_map::begin()
        {
        return m_map.begin();
        }

    extension_map::iterator extension_map::end()
        {
        return m_map.end();
        }

    extension_map::const_iterator extension_map::begin() const
        {
        return m_map.cbegin();
        }

    extension_map::const_iterator extension_map::end() const
        {
        return m_map.cend();
        }


    program::program(std::ostream *log_ostream) : log(log_ostream) 
        {
        read_config_file(config_file, m_config);
        }

    void program::set_src_dir(const std::string &path)
        {
        //TODO: support config files in custom location
        m_config.SourceDir = path;
        write_config_file(config_file, m_config);
        }

    void program::edit_ext_entry(const std::string &ext, const std::string &dest)
        {
        m_config.Map[ext] = dest;
        write_config_file(config_file, m_config);
        }

    void program::run(const std::string &path)
        {
        for (auto &p : dir_it(path.empty() ? m_config.SourceDir : path))
            if (is_regular_file(p)) rename(p, m_config.Map.resolve_tgt(p));
        }

    /**
     * \brief Reads a configuration file and stores it in
     *  the given configuration object.
     * \param path The path to the config file to be read.
     * \param cfg A `config_t` object that will be used to
     *  store the read data.
     */
    void read_config_file(const char *path, config_t &cfg)
        {
        std::ifstream stream(path);
        stream >> cfg;
        stream.close();
        }

    /**
     * \brief Takes a configuration object and writes it to
     *  the specified file.
     * \param path The path to the config file to be written.
     * \param in_cfg A `config_t` object containing the data
     *  to be written.
     */
    void write_config_file(const char *path, const config_t &in_cfg)
        {
        std::ofstream stream(path);
        stream << in_cfg;
        stream.close();
        }
}
