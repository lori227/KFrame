#pragma once

#include "base_sink.h"
#include "fmt/fmt.h"
#include "../details/file_helper.h"
#include "../details/null_mutex.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <string>

// Example for spdlog.h
//
// Create a file logger which creates new files with a specified time step and fixed file size:
//
// std::shared_ptr<logger> step_logger_mt(const std::string &logger_name, const filename_t &filename, unsigned seconds = 60, const
// filename_t &tmp_ext = ".tmp", unsigned max_file_size = std::numeric_limits<unsigned>::max(), bool delete_empty_files = true, const
// filename_t &file_header = ""); std::shared_ptr<logger> step_logger_st(const std::string &logger_name, const filename_t &filename,
// unsigned seconds = 60, const filename_t &tmp_ext = ".tmp", unsigned max_file_size = std::numeric_limits<unsigned>::max());

// Example for spdlog_impl.h
// Create a file logger that creates new files with a specified increment
// inline std::shared_ptr<spdlog::logger> spdlog::step_logger_mt(
//     const std::string &logger_name, const filename_t &filename_fmt, unsigned seconds, const filename_t &tmp_ext, unsigned max_file_size,
//     bool delete_empty_files, const filename_t &file_header)
// {
//     return create<spdlog::sinks::step_file_sink_mt>(logger_name, filename_fmt, seconds, tmp_ext, max_file_size, delete_empty_files,
//     file_header);
// }

// inline std::shared_ptr<spdlog::logger> spdlog::step_logger_st(
//     const std::string &logger_name, const filename_t &filename_fmt, unsigned seconds, const filename_t &tmp_ext, unsigned max_file_size,
//     bool delete_empty_files, const filename_t &file_header)
// {
//     return create<spdlog::sinks::step_file_sink_st>(logger_name, filename_fmt, seconds, tmp_ext, max_file_size, delete_empty_files,
//     file_header);
// }

namespace spdlog {
    namespace sinks {

#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
#elif _LINUX
    #include <stdarg.h>
    #include <sys/stat.h>
#endif

#ifdef _WIN32
    #define ACCESS _access
    #define MKDIR(a) _mkdir((a))
#else
    #define ACCESS access
    #define MKDIR(a) mkdir((a),0755)
#endif

        /*
         * Default generator of glog log file names.
         */
        struct default_glog_file_name_calculator
        {
            // Create filename for the form filename_YYYY-MM-DD_hh-mm-ss.ext
            static std::tuple<filename_t, filename_t> calc_filename( const filename_t& filename, const filename_t& split, unsigned filecount )
            {
                std::tm tm = spdlog::details::os::localtime();
                filename_t basename, ext;
                std::tie( basename, ext ) = spdlog::details::file_helper::split_by_extension( filename );      
                char tmp_dir_path[MAX_PATH] = { 0 };
                for ( size_t i = 0; i < filename.length(); ++i )
                {
                    tmp_dir_path[i] = filename[i];
                    if ( tmp_dir_path[i] == spdlog::details::os::folder_sep )
                    {
                        if ( ACCESS( tmp_dir_path, 0 ) == -1 )
                        {
                            MKDIR( tmp_dir_path );
                        }
                    }
                }

                filename_t newfile;
                if( filecount <= 1)
                {
                    newfile = fmt::format( "{}{}{:04d}{:02d}{:02d}{}{:02d}{}",
                        basename, split, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, split, tm.tm_hour, ext );
                }
                else
                {
                    newfile = fmt::format( "{}{}{:04d}{:02d}{:02d}{}{:02d}.{}{}", 
                        basename, split, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, split, filecount, ext );
                }
                return std::make_tuple( newfile, ext );
            }
        };

        /*
         * Rotating file sink based on size and a specified time step
         */
        template<class Mutex, class FileNameCalc = default_glog_file_name_calculator>
        class glog_file_sink final : public base_sink<Mutex>
        {
        public:
            glog_file_sink( filename_t base_filename, filename_t split, unsigned step_seconds, unsigned max_size )
                : _base_filename( std::move( base_filename ) )
                , _split( std::move( split ) )
                , _step_seconds( step_seconds )
                , _max_size( max_size )
                , _delete_empty_files( true )
                , _file_count( 1 )
            {
                if ( step_seconds == 0 )
                {
                    step_seconds = 3600;
                }

                _tp = _next_tp();
                std::tie( _current_filename, _ext ) = FileNameCalc::calc_filename( _base_filename, _split, _file_count );

                _file_helper.open( _current_filename );
                _current_size = ( unsigned )_file_helper.size(); // expensive. called only once
            }

        protected:
            void sink_it_( const details::log_msg& msg ) override
            {
                fmt::memory_buffer formatted;
                sink::formatter_->format( msg, formatted );

                unsigned msg_size = ( unsigned )formatted.size();
                if ( std::chrono::system_clock::now() >= _tp || _current_size + msg_size > _max_size )
                {
                    ++_file_count;
                    std::chrono::system_clock::time_point tp = _next_tp();
                    if ( tp != _tp )
                    {
                        _file_count = 1;
                    }
                    _tp = tp;

                    filename_t new_filename;
                    std::tie( new_filename, std::ignore ) = FileNameCalc::calc_filename( _base_filename, _split, _file_count );
                    bool change_occured = !details::file_helper::file_exists( new_filename );
                    if ( change_occured )
                    {
                        _current_size = 0;
                        _current_filename = std::move( new_filename );
                        _file_helper.open( _current_filename );
                    }
                }

                _current_size += msg_size;
                _file_helper.write( formatted );
            }

            void flush_() override
            {
                _file_helper.flush();
            }

        private:
            std::chrono::system_clock::time_point _next_tp()
            {
                std::chrono::seconds nowtime = std::chrono::duration_cast< std::chrono::seconds >( std::chrono::system_clock::now().time_since_epoch() );
                std::chrono::seconds nexttime = ( nowtime / _step_seconds + 1 ) * _step_seconds;
                std::chrono::system_clock::time_point tp( nexttime );
                return tp;
            }

            const filename_t _base_filename;
            const filename_t _split;
            const std::chrono::seconds _step_seconds;
            const unsigned _max_size;
            const bool _delete_empty_files;

            std::chrono::system_clock::time_point _tp;
            unsigned _file_count;
            filename_t _current_filename;
            filename_t _ext;
            unsigned _current_size;

            details::file_helper _file_helper;
        };

        using glog_file_sink_mt = glog_file_sink<std::mutex>;
        using glog_file_sink_st = glog_file_sink<details::null_mutex>;

    } // namespace sinks
} // namespace spdlog
