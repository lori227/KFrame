#ifndef __KF_LOG_H__
#define __KF_LOG_H__

#include "KFInclude.h"
namespace KFrame
{
    enum KFLogEnum
    {
        None = 0,
        System,
        Init,
        Logic,
        Net,
        Sql,
        Memory,
        Login,
        Player,
        Script,
        GM,
        //TO ADD
        Max,
    };

    ///////////////////////////////////////////////////////////////////////////
    static const std::string _log_category_name[] =
    {
        "none",
        "system",
        "init",
        "logic",
        "net",
        "sql",
        "memory",
        "login",
        "player",
        "script",
        //TO ADD
    };
    ///////////////////////////////////////////////////////////////////////////
}

#endif