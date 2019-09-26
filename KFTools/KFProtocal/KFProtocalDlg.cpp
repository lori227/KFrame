
// KFProtocalDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "KFProtocal.h"
#include "KFProtocalDlg.h"
#include "afxdialogex.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/Exception.h"
#include "google/protobuf/compiler/parser.h"
#include "google/protobuf/compiler/command_line_interface.h"
#include "google/protobuf/compiler/cpp/cpp_generator.h"
#include "KFProtocalConfig.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

#define __OPERATOR_TIMER__ 500
// CKFProtocalDlg 对话框



CKFProtocalDlg::CKFProtocalDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_KFPROTOCAL_DIALOG, pParent )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CKFProtocalDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_EDIT1, _server_cpp_path );
    DDX_Control( pDX, IDC_EDIT2, _server_lua_path );
    DDX_Control( pDX, IDC_EDIT3, _client_cpp_path );
    DDX_Control( pDX, IDC_EDIT4, _client_lua_path );
    DDX_Control( pDX, IDC_BUTTON5, _operate_btn );
}

BEGIN_MESSAGE_MAP( CKFProtocalDlg, CDialogEx )
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED( IDC_BUTTON1, &CKFProtocalDlg::OnBnClickedButton1 )
    ON_BN_CLICKED( IDC_BUTTON3, &CKFProtocalDlg::OnBnClickedButton3 )
    ON_BN_CLICKED( IDC_BUTTON2, &CKFProtocalDlg::OnBnClickedButton2 )
    ON_BN_CLICKED( IDC_BUTTON4, &CKFProtocalDlg::OnBnClickedButton4 )
    ON_BN_CLICKED( IDC_BUTTON5, &CKFProtocalDlg::OnBnClickedButton5 )
    ON_EN_CHANGE( IDC_EDIT1, &CKFProtocalDlg::OnEnChangeEdit1 )
    ON_WM_TIMER()
    ON_EN_CHANGE( IDC_EDIT2, &CKFProtocalDlg::OnEnChangeEdit2 )
    ON_EN_CHANGE( IDC_EDIT3, &CKFProtocalDlg::OnEnChangeEdit3 )
    ON_EN_CHANGE( IDC_EDIT4, &CKFProtocalDlg::OnEnChangeEdit4 )
END_MESSAGE_MAP()


void CKFProtocalDlg::LoadPathFile()
{
    _protocal_config->LoadFile( KFrame::KFProtocalConfig::_setting_file );
}

void CKFProtocalDlg::InitDialogData()
{
    _server_cpp_path.SetWindowTextA( _protocal_config->_server_cpp_path.c_str() );
    _server_lua_path.SetWindowTextA( _protocal_config->_server_lua_path.c_str() );
    _client_cpp_path.SetWindowTextA( _protocal_config->_client_cpp_path.c_str() );
    _client_lua_path.SetWindowTextA( _protocal_config->_client_lua_path.c_str() );
}

std::string CKFProtocalDlg::BrowsePath()
{
    std::string result = "";

    BROWSEINFO bi;
    CHAR buffer[512];
    //初始化入口参数bi开始
    bi.hwndOwner = NULL;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = buffer;//此参数如为NULL则不能显示对话框
    bi.lpszTitle = "选择路径";
    bi.ulFlags = BIF_RETURNONLYFSDIRS;
    bi.lpfn = NULL;
    bi.iImage = 0;
    //初始化入口参数bi结束
    LPITEMIDLIST pIDList = SHBrowseForFolder( &bi ); //调用显示选择对话框
    if ( pIDList ) //选择到路径(即：点了确定按钮)
    {
        SHGetPathFromIDList( pIDList, buffer );
        result = buffer;
    }

    return result;
}

// CKFProtocalDlg 消息处理程序

BOOL CKFProtocalDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon( m_hIcon, TRUE );			// 设置大图标
    SetIcon( m_hIcon, FALSE );		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    LoadPathFile();
    InitDialogData();

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CKFProtocalDlg::OnPaint()
{
    if ( IsIconic() )
    {
        CPaintDC dc( this ); // 用于绘制的设备上下文

        SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>( dc.GetSafeHdc() ), 0 );

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics( SM_CXICON );
        int cyIcon = GetSystemMetrics( SM_CYICON );
        CRect rect;
        GetClientRect( &rect );
        int x = ( rect.Width() - cxIcon + 1 ) / 2;
        int y = ( rect.Height() - cyIcon + 1 ) / 2;

        // 绘制图标
        dc.DrawIcon( x, y, m_hIcon );
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CKFProtocalDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>( m_hIcon );
}



void CKFProtocalDlg::OnBnClickedButton1()
{
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _server_cpp_path.SetWindowTextA( path.c_str() );
        _protocal_config->_server_cpp_path = path;
        _protocal_config->SaveFile( KFrame::KFProtocalConfig::_setting_file );
    }
}


void CKFProtocalDlg::OnBnClickedButton2()
{
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _server_lua_path.SetWindowTextA( path.c_str() );
        _protocal_config->_server_lua_path = path;
        _protocal_config->SaveFile( KFrame::KFProtocalConfig::_setting_file );
    }
}


void CKFProtocalDlg::OnBnClickedButton3()
{
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _client_cpp_path.SetWindowTextA( path.c_str() );
        _protocal_config->_client_cpp_path = path;
        _protocal_config->SaveFile( KFrame::KFProtocalConfig::_setting_file );
    }
}


void CKFProtocalDlg::OnBnClickedButton4()
{
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _client_lua_path.SetWindowTextA( path.c_str() );
        _protocal_config->_client_lua_path = path;
        _protocal_config->SaveFile( KFrame::KFProtocalConfig::_setting_file );
    }
}

void CKFProtocalDlg::OnBnClickedButton5()
{
    _operate_btn.EnableWindow( false );

    ProtocConfigFiles();	// 在当前文件夹内生成所有的proto相关文件
    CopyConfigFiles();		// 拷贝到所有对应的目录并删除所有生成文件
    BuildProtocolProj();	// 编译Protocol项目

    SetTimer( __OPERATOR_TIMER__, 4000, nullptr );
}

void CKFProtocalDlg::OnTimer( UINT_PTR nIDEvent )
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    switch ( nIDEvent )
    {
    case __OPERATOR_TIMER__:
        _operate_btn.EnableWindow( true );
        break;
    default:
        break;
    }

    CDialogEx::OnTimer( nIDEvent );
}

void CKFProtocalDlg::ProtocConfigFiles()
{
    Poco::File pocofile( "" );
    std::vector< Poco::File > configfile;
    pocofile.list( configfile );
    for ( auto& file : configfile )
    {
        if ( file.isFile() )
        {
            auto npos = file.path().find( "Message.proto" );
            if ( npos != std::string::npos )
            {
                // 将文件拷贝到客户端lua文件目录
                if ( !_protocal_config->_client_lua_path.empty() )
                {
                    file.copyTo( _protocal_config->_client_lua_path.c_str() );

                    OnProtocClientLuaFile( file.path() );
                }

                if ( !_protocal_config->_server_cpp_path.empty() ||
                        !_protocal_config->_server_lua_path.empty() ||
                        !_protocal_config->_client_cpp_path.empty() )
                {
                    OnProtocFile( file.path() );
                }
            }
        }
    }
}

void CKFProtocalDlg::OnProtocFile( const std::string& filename )
{
    google::protobuf::compiler::CommandLineInterface cli;
    cli.AllowPlugins( "protoc-" );

    // Proto2 C++
    google::protobuf::compiler::cpp::CppGenerator cpp_generator;
    cli.RegisterGenerator( "--cpp_out", "--cpp_opt", &cpp_generator,
                           "Generate C++ header and source." );


    const char* argv[3] = { "-I=./", "--cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./", filename.c_str() };
    cli.Run( 3, argv );
}

void CKFProtocalDlg::OnProtocClientLuaFile( const std::string& filename )
{
    google::protobuf::compiler::CommandLineInterface cli;
    cli.AllowPlugins( "protoc-" );

    // Proto2 C++
    google::protobuf::compiler::cpp::CppGenerator cpp_generator;
    cli.RegisterGenerator( "--cpp_out", "--cpp_opt", &cpp_generator,
                           "Generate C++ header and source." );


    auto npos = filename.find( "proto" );
    if ( npos == std::string::npos )
    {
        return;
    }

    auto pbfile = filename.substr( 0, npos ) + "pb";

    const char* argv[4] = { "-I", "--descriptor_set_out", pbfile.c_str(), filename.c_str() };
    cli.Run( 4, argv );
}

void CKFProtocalDlg::CopyConfigFiles()
{
    if ( !_protocal_config->_client_lua_path.empty() )
    {
        // 生成pb文件夹
        static auto _client_lua_pb_path = _protocal_config->_client_lua_path + "\\pb";
        Poco::File luapbfile( _client_lua_pb_path );
        if ( luapbfile.exists() )
        {
            luapbfile.remove( true );
        }

        luapbfile.createDirectory();
    }

    Poco::File pocofile( "" );
    std::vector< Poco::File > configfile;
    pocofile.list( configfile );
    for ( auto& file : configfile )
    {
        if ( file.isFile() )
        {
            auto npos = file.path().find( ".pb." );
            if ( npos != std::string::npos )
            {
                if ( !_protocal_config->_server_cpp_path.empty() )
                {
                    // 拷贝到服务器cpp目录
                    file.copyTo( _protocal_config->_server_cpp_path.c_str() );
                }

                if ( !_protocal_config->_server_lua_path.empty() )
                {
                    // 拷贝到服务器lua目录
                    file.copyTo( _protocal_config->_server_lua_path.c_str() );
                }

                if ( !_protocal_config->_client_cpp_path.empty() )
                {
                    file.copyTo( _protocal_config->_client_cpp_path.c_str() );
                }

                // 拷贝完成后删除生成的文件
                file.remove();
            }
            else
            {
                auto npos = file.path().find( ".pb" );
                if ( npos != std::string::npos )
                {
                    if ( !_protocal_config->_client_lua_path.empty() )
                    {
                        static auto _client_lua_pb_path = _protocal_config->_client_lua_path + "\\pb";

                        file.copyTo( _client_lua_pb_path.c_str() );

                        // 拷贝完成后删除生成的文件
                        file.remove();
                    }
                }
            }
        }
    }
}

void CKFProtocalDlg::BuildProtocolProj()
{
    if ( _protocal_config->_server_cpp_path.empty() )
    {
        return;
    }

    // 调用bat编译工程
    auto batfile = __FORMAT__( "_ProtocolBuild.bat {}", _protocal_config->_server_cpp_path );
    system( batfile.c_str() );
}

void CKFProtocalDlg::OnEnChangeEdit1()
{
    CString path_str;
    _server_cpp_path.GetWindowTextA( path_str );

    _protocal_config->_server_cpp_path = path_str;
    _protocal_config->SaveFile( KFrame::KFProtocalConfig::_setting_file );
}

void CKFProtocalDlg::OnEnChangeEdit2()
{
    CString path_str;
    _server_lua_path.GetWindowTextA( path_str );

    _protocal_config->_server_lua_path = path_str;
    _protocal_config->SaveFile( KFrame::KFProtocalConfig::_setting_file );
}


void CKFProtocalDlg::OnEnChangeEdit3()
{
    CString path_str;
    _client_cpp_path.GetWindowTextA( path_str );

    _protocal_config->_client_cpp_path = path_str;
    _protocal_config->SaveFile( KFrame::KFProtocalConfig::_setting_file );
}


void CKFProtocalDlg::OnEnChangeEdit4()
{
    CString path_str;
    _client_lua_path.GetWindowTextA( path_str );

    _protocal_config->_client_lua_path = path_str;
    _protocal_config->SaveFile( KFrame::KFProtocalConfig::_setting_file );
}
