#ifndef __KF_INCLUDE_H__
#define __KF_INCLUDE_H__

#include "stdarg.h"
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <algorithm>
#include <math.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <algorithm>
#include <time.h>
#include <mutex>
#include <atomic>
#include <cctype>
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
typedef void*	voidptr;
typedef char	int8;
typedef int16_t	int16;
typedef int32_t	int32;
typedef int64_t	int64;

typedef unsigned char  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
static const uint64 _invalid_int = 0u;
static const std::string _invalid_string = "";
static const std::string _globbing_string = "*";
static const std::string _split_string = "|";
static const std::string _range_string = "-";
static const std::string _domain_string = ":";
static const std::string _and_string = "&";
static const std::string _or_string = "|";
/////////////////////////////////////////////////////////////////////////
typedef std::list< std::string > ListString;
typedef std::vector< std::string > VectorString;
typedef std::unordered_map< std::string, std::string > MapString;
typedef std::list< MapString > ListMapString;

typedef std::set< uint32 > SetUInt32;
typedef std::list< uint32 > ListUInt32;
typedef std::vector< uint32 > VectorUInt32;

typedef std::set< uint64 > SetUInt64;
typedef std::list< uint64 > ListUInt64;
typedef std::vector< uint64 > VectorUInt64;

typedef std::map< uint32, uint32 > MapUInt32;
typedef std::map< uint64, uint64 > MapUInt64;

typedef std::map< std::string, uint64 > KeyValue;
/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif


