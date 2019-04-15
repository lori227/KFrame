#include "KFStateManage.hpp"
#include "KFStateWait.hpp"
#include "KFStateAuth.hpp"
#include "KFStateLogin.hpp"

namespace KFrame
{
    KFStateManage::KFStateManage()
    {
        _states.clear();
    }

    KFStateManage::~KFStateManage()
    {
        for ( auto& iter : _states )
        {
            delete iter.second;
        }
        _states.clear();
    }

    void KFStateManage::Initialize()
    {
        // 在这里添加所有状态信息
        AddState( KFrame::StateWait, new KFStateWait() );
        AddState( KFrame::StateAuth, new KFStateAuth() );
        AddState( KFrame::StateLogin, new KFStateLogin() );
    }

    void KFStateManage::AddState( uint32 state, KFState* kfstate )
    {
        kfstate->_state = state;
        _states[ state ] = kfstate;
    }

    KFState* KFStateManage::FindState( uint32 state )
    {
        auto iter = _states.find( state );
        if ( iter == _states.end() )
        {
            return nullptr;
        }

        return iter->second;
    }
}