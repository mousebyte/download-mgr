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

    /**
     * \brief Reads a configuration file and stores it in
     *  the given configuration object.
     * \param path The path to the config file to be read.
     * \param cfg A `config_t` object that will be used to
     *  store the read data.
     */
    void read_config_file(const char *path, config_t &cfg)
        {
        std::ifstream stream(path, std::ios::binary);
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
        std::ofstream stream(path, std::ios::binary);
        stream << in_cfg;
        stream.close();
        }
}
