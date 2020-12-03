#include "KFGenerateGit.h"
#include "KFGenerateSvn.h"
#include "KFGenerateEvent.h"
#include "KFGenerateLogic.h"
#include "KFGenerateRepository.h"

namespace KFrame
{
    void KFGenerateRepository::Shutdown()
    {
        if ( _git_svn != nullptr )
        {
            _git_svn->Shutdown();
            __DELETE_OBJECT__( _git_svn );
        }
    }

    void KFGenerateRepository::CreateRepository( const std::string& type )
    {
        _type = type;
        __DELETE_OBJECT__( _git_svn );
        if ( _type == "git" )
        {
            _git_svn = __NEW_OBJECT__( KFGenerateGit );
        }
        else
        {
            _git_svn = __NEW_OBJECT__( KFGenerateSvn );
        }

        _git_svn->Initialize();
    }

    bool KFGenerateRepository::Open( const std::string& type )
    {
        if ( type.empty() )
        {
            return true;
        }

        if ( _type.empty() || type != _type )
        {
            CreateRepository( type );
        }

        if ( _git_svn->_data == nullptr )
        {
            auto data = _logic->_repository_list.Find( type );
            if ( data == nullptr )
            {
                return false;
            }

            return _git_svn->Open( data );
        }

        return true;
    }

    bool KFGenerateRepository::IsOpen() const
    {
        return _git_svn != nullptr && _git_svn->_data != nullptr;
    }


    bool KFGenerateRepository::Push( const StringList& filelist, const std::string& message )
    {
        if ( !IsOpen() )
        {
            _event->ShowEventMessage( "仓库打开失败, 请检查配置和网络!" );
            return false;
        }

        return _git_svn->Push( filelist, message );
    }

    bool KFGenerateRepository::Pull( bool commit, const std::string& message )
    {
        if ( !IsOpen() )
        {
            _event->ShowEventMessage( "仓库打开失败, 请检查配置和网络!" );
            return false;
        }

        return _git_svn->Pull( commit, message );
    }
}
