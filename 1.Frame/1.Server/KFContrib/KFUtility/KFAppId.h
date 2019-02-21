#ifndef __KF_APPID_H__
#define __KF_APPID_H__

#include "KFInclude.h"

namespace KFrame
{
    union AppIdData;
    class KFAppId
    {
    public:
        KFAppId();
        KFAppId( uint64 appid );
        KFAppId( const std::string& strappid );
        KFAppId( const KFAppId& appid );
        KFAppId& operator = ( const KFAppId& appid );

        ~KFAppId();
        /////////////////////////////////////////////////////////////////////////////////////
        void FromUInt64( uint64 appid );
        void FromString( std::string strappid );
        /////////////////////////////////////////////////////////////////////////////////////
        // 渠道
        uint32 GetChannelId() const;
        void SetChannelId( uint32 value );

        // 类型
        uint32 GetType() const;
        void SetType( uint32 value );

        // 小区id
        uint32 GetZoneId() const;
        void SetZoneId( uint32 value );

        // 进程id
        uint32 GetWorkId() const;
        void SetWorkId( uint32 value );

        // id
        uint64 GetId() const;
        const std::string& ToString() const;

        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        // 转换
        static const std::string& ToString( uint64 appid );
        static uint64 ToUInt64( const std::string& strappid );

    private:
        AppIdData* _data = nullptr;

        // 保存id字串
        std::string _str_app_id;
    };
}

#endif // !__KF_APPID_H__
