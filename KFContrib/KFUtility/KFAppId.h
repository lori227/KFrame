#ifndef __KF_APPID_H__
#define __KF_APPID_H__

#include "KFDefine.h"

namespace KFrame
{
    union AppIdUnion;
    class KFAppId
    {
    public:
        KFAppId();
        KFAppId( uint64 app_id );
        KFAppId( const std::string& app_id );
        KFAppId( const KFAppId& app_id );
        KFAppId& operator = ( const KFAppId& app_id );

        ~KFAppId();
        /////////////////////////////////////////////////////////////////////////////////////
        void FromUInt64( uint64 app_id );
        void FromString( std::string app_id );
        /////////////////////////////////////////////////////////////////////////////////////
        // 类型
        uint32 GetType() const;

        // 小区id
        uint32 GetZoneId() const;

        // 进程id
        uint32 GetWorkId() const;

        // id
        uint64 GetId() const;
        const std::string& ToString() const;

        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        // 转换
        static std::string ToString( uint64 app_id );
        static uint64 ToUInt64( const std::string& app_id );

    private:
        AppIdUnion* _union = nullptr;

        // 保存id字串
        std::string _app_id;
    };
}

#endif // !__KF_APPID_H__
