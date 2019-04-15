#ifndef __KF_DEPLOY_DEFINE_H__
#define __KF_DEPLOY_DEFINE_H__

#include "KFrame.h"
#include "stdafx.h"
using namespace KFrame;

namespace KFDeploy
{
    static const char* _deploy_file = "deploy.xml";

    static const char* _log_name[] =
    {
        "All",
        "Debug",
        "Info",
        "Warn",
        "Error",
    };
    static uint32 _log_count = std::extent<decltype( _log_name )>::value;

    static const char* _status_name[] =
    {
        "关闭",
        "运行",
    };
    static uint32 _status_count = std::extent<decltype( _status_name )>::value;

    static const char* _mode_name[] =
    {
        "Release",
        "Debug",
    };
    static uint32 _mode_count = std::extent<decltype( _mode_name )>::value;

    static const char* _net_name[] =
    {
        "请选择",
        "内网",
        "外网",
    };
    static uint32 _net_count = std::extent<decltype( _net_name )>::value;
}

static std::string& GetEditText( CEdit& edit )
{
    CString strtext;
    edit.GetWindowTextA( strtext );

    static std::string _result;
    _result = strtext.GetBuffer();
    return _result;
}



#endif
