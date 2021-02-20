#ifndef __KF_TAR_H__
#define __KF_TAR_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFTar
    {
    public:
        // 压缩目录
        // path 目录路径
        // tarname 目标文件名字
        static bool TarPath( const std::string& path, const std::string& tarname );
    };

}

#endif
