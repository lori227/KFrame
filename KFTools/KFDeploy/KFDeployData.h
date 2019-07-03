#ifndef __KF_DEPLOY_DATA_H__
#define __KF_DEPLOY_DATA_H__

#include "KFDeployDefine.h"

class DeployData
{
public:
    uint64 _id = 0;
    std::string _str_id;
    std::string _name;
    uint32 _channel;
    std::string _channel_name;
    std::string _ip;
    uint32 _port = 0;
};


class LogData
{
public:
    uint64 _agent_id = 0u;
    std::string _content;
};

class AgentData
{
public:
    std::string _id;
    std::string _service;
    std::string _ip;
};

class AppData
{
public:
    std::string _name;
    std::unordered_map< std::string, uint32 > _types;
};

class ServerData
{
public:
    std::string _name;
    std::string _type;
    std::string _id;
    uint32 _zone_id;
    uint32 _worker_id;
};

class CommandData
{
public:
    std::string _command;
    std::string _name;
    std::string _text;
    bool _is_server = false;
};
///////////////////////////////////////////////////////////////////////
class CKFDeployDlg;
class DeployManage : public KFSingleton< DeployManage >
{
public:
    // 清空日志
    void ClearLogData()
    {
        for ( auto data : _log_data )
        {
            delete data;
        }
        _log_data.clear();
    }

    // 添加日志
    void AddLogData( uint64 id, const std::string& content )
    {
        auto data = new LogData();
        data->_agent_id = id;
        data->_content = content;
        _log_data.push_back( data );
    }

    // 清空Agent
    void ClearAgentData()
    {
        for ( auto data : _agent_data )
        {
            delete data;
        }
        _agent_data.clear();
    }

    // 添加日志
    void AddAgentData( const std::string& id, const std::string& ip, const std::string& service )
    {
        auto data = new AgentData();
        data->_id = id;
        data->_ip = ip;
        data->_service = service;
        _agent_data.push_back( data );
    }

    std::string& GetTableName( const std::string& agentid )
    {
        static std::string _result = "";
        _result.clear();

        for ( auto data : _agent_data )
        {
            if ( data->_id == agentid )
            {
                auto service = data->_service;
                KFUtility::ReplaceString( service, ".", "_" );

                KFAppId appid( data->_id );
                _result = __FORMAT__( "{}_{}", service, __KF_STRING__( deploy )  );
                break;
            }
        }

        return _result;
    }

    // 清空日志
    void ClearAppData()
    {
        for ( auto data : _app_data )
        {
            delete data;
        }
        _app_data.clear();
    }

    // 添加日志
    void AddAppData( const std::string& name, const std::string& type, uint32 flag )
    {
        for ( auto data : _app_data )
        {
            if ( data->_name == name )
            {
                data->_types[ type ] = flag;
                return;
            }
        }

        auto data = new AppData();
        data->_name = name;
        data->_types[ type ] = flag;
        _app_data.push_back( data );
    }

    const AppData* FindAppData( const std::string& name )
    {
        for ( auto data : _app_data )
        {
            if ( data->_name == name )
            {
                return data;
            }
        }

        return nullptr;
    }

    void ClearServerData()
    {
        for ( auto data : _server_data )
        {
            delete data;
        }
        _server_data.clear();
    }

    bool AddServerData( const std::string& name, const std::string& type, const std::string& id )
    {
        for ( auto data : _server_data )
        {
            if ( data->_id == id )
            {
                return false;
            }
        }

        auto data = new ServerData();
        data->_name = name;
        data->_type = type;
        data->_id = id;

        KFAppId appid( id );
        data->_zone_id = appid.GetZoneId();
        data->_worker_id = appid.GetWorkId();
        _server_data.push_back( data );
        return true;
    }

    uint32 FindMaxServerId( const std::string& name, const std::string& type )
    {
        auto maxworkid = 0u;

        for ( auto data : _server_data )
        {
            if ( data->_name == name && data->_type == type )
            {
                if ( maxworkid < data->_worker_id )
                {
                    maxworkid = data->_worker_id;
                }
            }
        }

        return maxworkid + 1;
    }

    void ClearCommandData()
    {
        for ( auto data : _command_data )
        {
            delete data;
        }
        _command_data.clear();
    }

    const CommandData* FindCommandData( std::string name )
    {
        for ( auto data : _command_data )
        {
            if ( data->_name == name )
            {
                return data;
            }
        }

        return nullptr;
    }

public:
    CKFDeployDlg* _dlg_deploy = nullptr;

    // 中控服务器列表
    KFHashMap< std::string, const std::string&, DeployData > _deploy_data;

    // 正在连接的id
    uint32 _channel = 0u;
    uint64 _connect_deploy_id = 0u;
    std::string _connect_deploy_strid;

    // 日志列表
    std::list< LogData* > _log_data;

    // agent列表
    std::list< AgentData* > _agent_data;

    // service列表
    std::unordered_map< uint32, std::string > _channel_name;
    std::unordered_map< uint32, std::string > _service_name;

    // appdata
    std::list< AppData* > _app_data;

    // serverdata
    std::list< ServerData* > _server_data;

    // commanddata
    std::list< CommandData* > _command_data;

};

static auto _deploy_manage = DeployManage::Instance();
///////////////////////////////////////////////////////////////////////

#endif
