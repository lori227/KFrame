#include "KFLoginGameManage.h"

namespace KFrame
{
    KFLoginGameManage::KFLoginGameManage()
    {

    }

    KFLoginGameManage::~KFLoginGameManage()
    {

    }

    KFLoginGame* KFLoginGameManage::AddLoginGame(  uint32 handleid )
    {
        auto kfloginnode = _kf_game_list.Create( handleid );
        kfloginnode->_game_id = handleid;
        return kfloginnode;
    }

    void KFLoginGameManage::UpdateLoginGame( uint32 handleid, uint32 clientcount )
    {
        auto kfloginnode = FindLoginGame( handleid );
        if ( kfloginnode == nullptr )
        {
            _kf_conhash.AddHashNode( "game", handleid, 100 );
            kfloginnode = AddLoginGame( handleid );
        }

        kfloginnode->_client_count = clientcount;
    }

    bool KFLoginGameManage::RemoveLoginGame( uint32 handleid )
    {
        _kf_conhash.RemoveHashNode( handleid );
        return _kf_game_list.Remove( handleid );
    }

    KFLoginGame* KFLoginGameManage::FindLoginGame( uint32 handleid )
    {
        return _kf_game_list.Find( handleid );
    }

    KFLoginGame* KFLoginGameManage::SelectMinLoginGame()
    {
        auto maxclientcount = 0xFFFFFFFF;
        KFLoginGame* kflogingame = nullptr;

        for ( auto& iter : _kf_game_list._objects )
        {
            auto temp = iter.second;

            if ( temp->_client_count < maxclientcount )
            {
                maxclientcount = temp->_client_count;
                kflogingame = temp;
            }
        }

        // 先加一个连接数量, 必选同一时间都选择同有一个服务器
        if ( kflogingame != nullptr )
        {
            ++kflogingame->_client_count;
        }

        return kflogingame;
    }

    KFLoginGame* KFLoginGameManage::HashLoginGame( uint32 playerid )
    {
        auto gameid = _kf_conhash.FindHashNode( playerid );
        return FindLoginGame( gameid );
    }
}