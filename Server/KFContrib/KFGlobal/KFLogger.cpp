#include "KFLogger.h"
#include "KFSpdLog.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    KFLocalLogger::KFLocalLogger()
    {
        _spd_log = new KFSpdLog();
    }

    KFLocalLogger::~KFLocalLogger()
    {
        delete _spd_log;
    }

    bool KFLocalLogger::Initialize( const std::string& appname, const std::string& apptype, uint32 appid )
    {
        std::string outpath = __FORMAT__( ".{}_output", spdlog::details::os::folder_sep );
        return _spd_log->Initialize( outpath, appname, apptype, appid );
    }

    void KFLocalLogger::Log( uint32 loglevel, const std::string& content )
    {
        _spd_log->Log( loglevel, content );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    KFRemoteLogger::KFRemoteLogger()
    {
        _remote_log_function = nullptr;
    }

    KFRemoteLogger::~KFRemoteLogger()
    {
        _remote_log_function = nullptr;

        KFLocker locker( _kf_mutex );
        for ( auto kfdata : _log_data_list )
        {
            __KF_DESTROY__( KFLogData, kfdata );
        }
        _log_data_list.clear();
    }

    void KFRemoteLogger::SetRemoteLogFunction( KFLogFunction& function )
    {
        _remote_log_function = function;
    }

    // 最大缓存队列长度
#define __MAX_LOG_DATA_LIST_COUNT__ 1000u

    void KFRemoteLogger::Log( uint32 loglevel, const std::string& content )
    {
        // 加载程序出错时候, 日志打在本地
        if ( loglevel == spdlog::level::off )
        {
            _spd_log->Log( spdlog::level::err, content );
        }
        else
        {
            // 添加到队列
            auto kfdata = __KF_CREATE__( KFLogData );
            kfdata->_log_level = loglevel;
            kfdata->_content = content;

            KFLocker locker( _kf_mutex );
            _log_data_list.push_back( kfdata );

            // 判断队列最大数量, 超出数量打印到本地
            if ( _log_data_list.size() > __MAX_LOG_DATA_LIST_COUNT__ )
            {
                auto kfdata = _log_data_list.front();
                KFLocalLogger::Log( kfdata->_log_level, kfdata->_content );

                __KF_DESTROY__( KFLogData, kfdata );
                _log_data_list.pop_front();
            }
        }
    }

    void KFRemoteLogger::RunUpdate()
    {
        if ( _remote_log_function == nullptr )
        {
            return;
        }

        std::list< KFLogData* > templist;
        {
            KFLocker locker( _kf_mutex );
            if ( _log_data_list.empty() )
            {
                return;
            }

            templist.swap( _log_data_list );
        }

        // 发送远程log
        do
        {
            auto kfdata = templist.front();
            auto ok = _remote_log_function( kfdata->_log_level, kfdata->_content );
            if ( ok )
            {
                __KF_DESTROY__( KFLogData, kfdata );
                templist.pop_front();
            }
            else
            {
                KFLocker locker( _kf_mutex );
                _log_data_list.splice( _log_data_list.begin(), templist );
                break;
            }
        } while ( !templist.empty() );
    }
}
