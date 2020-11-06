#include "KFMySQLConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMySQLList::Reset()
    {
        _mysql_list.clear();
        _kf_seting = nullptr;
    }

    void KFMySQLList::AddSetting( KFMySQLSetting& kfsetting )
    {
        _mysql_list.push_back( kfsetting );
    }

    const KFMySQLSetting* KFMySQLList::FindSetting()
    {
        if ( _kf_seting == nullptr )
        {
            auto index = KFGlobal::Instance()->_app_id->GetId() % static_cast< uint32 >( _mysql_list.size() );
            _kf_seting = &_mysql_list[ index ];
        }

        return _kf_seting;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    KFMySQLList* KFMySQLType::FindMySQLList( uint32 type )
    {
        return _mysql_list.Find( type );
    }

    KFMySQLList* KFMySQLType::AddMySQLList( uint32 type )
    {
        return _mysql_list.Create( type );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    KFMySQLType* KFMySQLConfig::FindMySQLType( const std::string& module, uint32 logicid )
    {
        auto key = ModuleKey( module, logicid );
        auto mysqltype = _mysql_type.Find( key );
        if ( mysqltype == nullptr )
        {
            mysqltype = _mysql_type.First();
        }

        return mysqltype;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMySQLConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        auto _mysql_id = 0u;
        _mysql_type.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();

        auto modulenode = config.FindNode( "Module" );
        while ( modulenode.IsValid() )
        {
            auto name = modulenode.ReadString( "Name" );

            auto mysqlnode = modulenode.FindNode( "MySQL" );
            while ( mysqlnode.IsValid() )
            {
                auto minlogicid = mysqlnode.ReadUInt32( "MinLogicId" );
                auto maxlogicid = mysqlnode.ReadUInt32( "MaxLogicId" );

                for ( auto i = minlogicid; i <= maxlogicid; ++i )
                {
                    auto kfmysqltype = _mysql_type.Create( ModuleKey( name, i ) );
                    kfmysqltype->_id = ++_mysql_id;

                    // write
                    auto writenode = mysqlnode.FindNode( "Write" );
                    while ( writenode.IsValid() )
                    {
                        KFMySQLSetting kfsetting;

                        kfsetting._ip = writenode.ReadString( "IP" );
                        kfsetting._port = writenode.ReadUInt32( "Port" );
                        kfsetting._database = writenode.ReadString( "Database" );
                        kfsetting._user = writenode.ReadString( "User" );
                        kfsetting._password = writenode.ReadString( "Password" );

                        auto kfmysqllist = kfmysqltype->AddMySQLList( KFDatabaseEnum::Write );
                        kfmysqllist->AddSetting( kfsetting );

                        writenode.NextNode( "Write" );
                    }

                    // read
                    auto readnode = mysqlnode.FindNode( "Read" );
                    while ( readnode.IsValid() )
                    {

                        KFMySQLSetting kfsetting;
                        kfsetting._ip = readnode.ReadString( "IP" );
                        kfsetting._port = readnode.ReadUInt32( "Port" );
                        kfsetting._database = readnode.ReadString( "Database" );
                        kfsetting._user = readnode.ReadString( "User" );
                        kfsetting._password = readnode.ReadString( "Password" );

                        auto kfmysqllist = kfmysqltype->AddMySQLList( KFDatabaseEnum::Read );
                        kfmysqllist->AddSetting( kfsetting );

                        readnode.NextNode( "Read" );
                    }
                }

                mysqlnode.NextNode();
            }

            modulenode.NextNode();
        }

        return true;
    }
}
