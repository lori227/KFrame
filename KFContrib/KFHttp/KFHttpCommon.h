#ifndef __KF_HTTP_COMMON_H__
#define __KF_HTTP_COMMON_H__

#include "KFDefine.h"
#include "KFJson.h"

namespace KFrame
{
    class KFHttpCommon
    {
    public:
        // 设置签名
        static void MakeSignature( KFJson& json );

        // 验证签名
        static bool VerifySignature( KFJson& json );

        // 发送错误码
        static std::string SendResponseCode( uint32 code );

        // 获得错误码
        static uint32 GetResponseCode( KFJson& json );

        // 发送数据
        static std::string SendResponse( KFJson& json );
        static std::string SendResponse( KFJson& json, uint32 code );
    };
}


#endif