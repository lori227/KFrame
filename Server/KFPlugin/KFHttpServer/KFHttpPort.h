#ifndef __KF_HTTP_PORT_H__
#define __KF_HTTP_PORT_H__

#include "KFrame.h"
#include "KFThread/KFMutex.h"
#include "KFMemory/KFShareMemory.h"

namespace KFrame
{
    class KFPort
    {
    public:
        KFPort()
        {
            _id = 0;
            _port = 0;
        }

    public:
        uint32 _id;
        uint32 _port;
    };

#define __MAX_PORT_COUNT__ 200
    class KFramePort
    {
    public:
        KFramePort()
        {
            _is_inited = 0;
        }

        // 查找端口
        KFPort* FindPort( uint32 id );

    public:
        // 是否初始化
        uint32 _is_inited;

        // 互斥量
        KFMutex _kf_mutex;

        // 端口列表
        KFPort _kf_port[ __MAX_PORT_COUNT__ ];
    };

    ///////////////////////////////////////////////////////////////////////
    class KFHttpProt
    {
    public:
        KFHttpProt();
        ~KFHttpProt();

        // 计算一个监听端口
        uint32 CalcListenPort( const std::string& appname, uint32 appid, uint32 baseport );

    private:
        // 共享内存
        KFShareMemory _kf_share_memory;
    };
}
#endif

