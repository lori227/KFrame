
// KFResourceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KFResource.h"
#include "KFResourceDlg.h"
#include "afxdialogex.h"
#include "KFResourceConfig.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/Exception.h"
#include "KFParse.h"
#include <windows.h>
#include <direct.h>

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

#define __REMOVE_TIMER__ 700
#define __UPDATE_TIMER__ 701

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx( IDD_ABOUTBOX )
{
}

void CAboutDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CAboutDlg, CDialogEx )
END_MESSAGE_MAP()


// CKFResourceDlg dialog



CKFResourceDlg::CKFResourceDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_KFRESOURCE_DIALOG, pParent )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CKFResourceDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST1, _list_file );
    DDX_Control( pDX, IDC_LIST2, _list_parse );
    DDX_Control( pDX, IDC_CHECK1, _check_server );
    DDX_Control( pDX, IDC_CHECK2, _check_client );
    DDX_Control( pDX, IDC_COMBO1, _combo_server );
    DDX_Control( pDX, IDC_EDIT1, _edit_server_path );
    DDX_Control( pDX, IDC_EDIT2, _edit_client_path );
    DDX_Control( pDX, IDC_COMBO2, _combo_store );
    DDX_Control( pDX, IDC_LIST3, _list_log );
}

BEGIN_MESSAGE_MAP( CKFResourceDlg, CDialogEx )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED( IDC_BUTTON1, &CKFResourceDlg::OnBnClickedButtonAddAllParse )
    ON_BN_CLICKED( IDC_BUTTON3, &CKFResourceDlg::OnBnClickedButtonRemoveAllParse )
    ON_BN_CLICKED( IDC_CHECK1, &CKFResourceDlg::OnBnClickedCheckServerType )
    ON_BN_CLICKED( IDC_CHECK2, &CKFResourceDlg::OnBnClickedCheckClientType )
    ON_CBN_SELCHANGE( IDC_COMBO1, &CKFResourceDlg::OnCbnSelchangeComboServer )
    ON_BN_CLICKED( IDC_BUTTON4, &CKFResourceDlg::OnBnClickedButtonBrowseServer )
    ON_BN_CLICKED( IDC_BUTTON2, &CKFResourceDlg::OnBnClickedButtonBrowseClient )
    ON_BN_CLICKED( IDC_BUTTON5, &CKFResourceDlg::OnBnClickedButtonParse )
    ON_CBN_SELCHANGE( IDC_COMBO2, &CKFResourceDlg::OnCbnSelchangeComboStore )
    ON_NOTIFY( NM_DBLCLK, IDC_LIST1, &CKFResourceDlg::OnNMDblclkListFile )
    ON_NOTIFY( NM_RCLICK, IDC_LIST1, &CKFResourceDlg::OnNMRClickListFile )
    ON_COMMAND( ID_32771, &CKFResourceDlg::OnChangeServerType )
    ON_COMMAND( ID_32772, &CKFResourceDlg::OnChangeClientType )
    ON_COMMAND( ID_32773, &CKFResourceDlg::OnChangeCommonType )
    ON_LBN_DBLCLK( IDC_LIST2, &CKFResourceDlg::OnLbnDblclkListRemoveParse )
    ON_BN_CLICKED( IDC_BUTTON6, &CKFResourceDlg::OnBnClickedButtonParseAndUpdate )
    ON_BN_CLICKED( IDC_BUTTON7, &CKFResourceDlg::OnBnClickedButtonRefreshFile )
END_MESSAGE_MAP()


// CKFResourceDlg message handlers

BOOL CKFResourceDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT( ( IDM_ABOUTBOX & 0xFFF0 ) == IDM_ABOUTBOX );
    ASSERT( IDM_ABOUTBOX < 0xF000 );

    CMenu* pSysMenu = GetSystemMenu( FALSE );
    if ( pSysMenu != nullptr )
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString( IDS_ABOUTBOX );
        ASSERT( bNameValid );
        if ( !strAboutMenu.IsEmpty() )
        {
            pSysMenu->AppendMenu( MF_SEPARATOR );
            pSysMenu->AppendMenu( MF_STRING, IDM_ABOUTBOX, strAboutMenu );
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon( m_hIcon, TRUE );			// Set big icon
    SetIcon( m_hIcon, FALSE );		// Set small icon

    // TODO: Add extra initialization here
    InitFrameData();
    LoadResourceData();
    InitDialogData();
    InitLogicData();
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKFResourceDlg::OnSysCommand( UINT nID, LPARAM lParam )
{
    if ( ( nID & 0xFFF0 ) == IDM_ABOUTBOX )
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand( nID, lParam );
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKFResourceDlg::OnPaint()
{
    if ( IsIconic() )
    {
        CPaintDC dc( this ); // device context for painting

        SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>( dc.GetSafeHdc() ), 0 );

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics( SM_CXICON );
        int cyIcon = GetSystemMetrics( SM_CYICON );
        CRect rect;
        GetClientRect( &rect );
        int x = ( rect.Width() - cxIcon + 1 ) / 2;
        int y = ( rect.Height() - cyIcon + 1 ) / 2;

        // Draw the icon
        dc.DrawIcon( x, y, m_hIcon );
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKFResourceDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>( m_hIcon );
}

BOOL CKFResourceDlg::PreTranslateMessage( MSG* pMsg )
{
    //屏蔽ESC关闭窗体/
    if ( pMsg->message == WM_KEYDOWN )
    {
        if ( pMsg->wParam == VK_ESCAPE ||
                pMsg->wParam == VK_RETURN )
        {
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage( pMsg );
}

void CKFResourceDlg::InitFrameData()
{
    KFGlobal::Initialize( nullptr );
    KFMalloc::Initialize( nullptr );
}

void CKFResourceDlg::InitDialogData()
{
    // file list
    {
        auto style = _list_file.GetExtendedStyle();
        _list_file.SetExtendedStyle( style | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

        RECT rect;
        _list_file.GetClientRect( &rect );
        auto width = rect.right / 10.f;

        int index = 0;
        _list_file.InsertColumn( index++, "", LVCFMT_CENTER, width * 2 );
        _list_file.InsertColumn( index++, "文件名称", LVCFMT_CENTER, width * 4.8 );
        _list_file.InsertColumn( index++, "更新时间", LVCFMT_CENTER, width * 3 );
        _list_file.InsertColumn( index++, "文件类型", LVCFMT_CENTER, width * 2 );
        _list_file.DeleteColumn( 0 );
    }

    // store
    {
        _combo_store.ResetContent();

        for ( auto i = 0; i < KFResource::_store_count; ++i )
        {
            _combo_store.AddString( KFResource::_store_name[i] );
            if ( _resource_config->_store_name == KFResource::_store_name[ i ] )
            {
                _combo_store.SetCurSel( i );
            }
        }
    }

    // severlist
    {
        _combo_server.ResetContent();
        for ( auto& iter : _resource_config->_servers._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_ip.empty() )
            {
                auto text = __FORMAT__( "{}|{}", kfsetting->_name, kfsetting->_ip );
                _combo_server.AddString( kfsetting->_name.c_str() );
            }
            else
            {
                auto text = __FORMAT__( "{}|{}", kfsetting->_name, kfsetting->_ip );
                _combo_server.AddString( text.c_str() );
            }
        }
        _combo_server.SetCurSel( _resource_config->_server_select_id );
    }

    // check
    {
        if ( KFUtility::HaveBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Client ) )
        {
            _check_client.SetCheck( TRUE );
        }

        if ( KFUtility::HaveBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Server ) )
        {
            _check_server.SetCheck( TRUE );
        }
    }

    // path
    _edit_server_path.SetWindowTextA( _resource_config->_server_path.c_str() );
    _edit_client_path.SetWindowTextA( _resource_config->_client_path.c_str() );
}

void CKFResourceDlg::LoadResourceData()
{
    _resource_config->LoadFile( KFResource::_resource_file );
    _resource_config->LoadSetting( KFResource::_setting_file );
}

void CKFResourceDlg::InitLogicData()
{
    // 初始化文件列表
    RemoveFileList();
    SetTimer( __REMOVE_TIMER__, 30000, nullptr );

    // 更新文件
    UpdateFileList();
    SetTimer( __UPDATE_TIMER__, 5000, nullptr );
}

bool HaveFile( std::vector< Poco::File >& filelist, const std::string& filename )
{
    for ( auto& file : filelist )
    {
        auto path = KFConvert::ToAscii( file.path() );
        if ( path == filename )
        {
            return true;
        }
    }

    return false;
}

bool CKFResourceDlg::IsResourceFile( const std::string& path )
{
    if ( path.at( 0 ) == '~' )
    {
        return false;
    }

    Poco::Path pocopath( path );
    auto& extension = pocopath.getExtension();
    return extension == "xlsx" || extension == "xls";
}

void CKFResourceDlg::UpdateFileList()
{
    Poco::File file( "./" );
    std::vector< Poco::File > filelist;
    file.list( filelist );

    _client_update_list.clear();
    _server_update_list.clear();

    // 文件信息
    for ( auto& file : filelist )
    {
        auto path = KFConvert::ToAscii( file.path() );
        auto ok = IsResourceFile( path );
        if ( !ok )
        {
            continue;
        }

        auto kfsetting = _resource_config->_files.Create( path );
        if ( kfsetting->_name.empty() )
        {
            kfsetting->_name = path;
        }

        kfsetting->_md5_now = KFCrypto::Md5File( path );
        kfsetting->_now_time = file.getLastModified().epochTime();
        if ( kfsetting->IsClientFile() && kfsetting->_last_md5_client != kfsetting->_md5_now )
        {
            _client_update_list.insert( kfsetting->_name );
        }

        if ( kfsetting->IsServerFile() && kfsetting->_last_md5_server != kfsetting->_md5_now )
        {
            _server_update_list.insert( kfsetting->_name );
        }
    }

    UpdateFileListCtrl();
}

void CKFResourceDlg::RemoveFileList()
{
    Poco::File file( "./" );
    std::vector< Poco::File > filelist;
    file.list( filelist );

    // 删除无效的文件
    std::list< std::string > removes;
    for ( auto& iter : _resource_config->_files._objects )
    {
        auto isexist = HaveFile( filelist, iter.first );
        if ( !isexist )
        {
            removes.push_back( iter.first );
        }
    }

    if ( removes.empty() )
    {
        return;
    }

    bool removeok = false;
    for ( auto& filename : removes )
    {
        RemoveParseFile( filename );
        removeok |= _client_update_list.erase( filename ) > 0u;
        removeok |= _server_update_list.erase( filename ) > 0u;
        _resource_config->_files.Remove( filename );
    }

    if ( removeok )
    {
        UpdateFileListCtrl();
    }
    _resource_config->SaveFile( KFResource::_resource_file );
}

void CKFResourceDlg::AddParseFile( const std::string& filename )
{
    auto ok = HaveParseFile( filename );
    if ( !ok )
    {
        _list_parse.AddString( filename.c_str() );
    }
}

void CKFResourceDlg::RemoveParseFile( const std::string& filename )
{
    for ( auto i = 0; i < _list_parse.GetCount(); ++i )
    {
        CString strtext;
        _list_parse.GetText( i, strtext );

        if ( filename == strtext.GetBuffer() )
        {
            _list_parse.DeleteString( i );
            break;
        }
    }
}

bool CKFResourceDlg::HaveParseFile( const std::string& filename )
{
    for ( auto i = 0; i < _list_parse.GetCount(); ++i )
    {
        CString strtext;
        _list_parse.GetText( i, strtext );

        if ( filename == strtext.GetBuffer() )
        {
            return true;
        }
    }

    return false;
}

void CKFResourceDlg::UpdateFileListCtrl()
{
    if ( _resource_config->_file_type == KFResource::FileType::Client )
    {
        UpdateFileListCtrl( _client_update_list );
    }
    else if ( _resource_config->_file_type == KFResource::FileType::Server )
    {
        UpdateFileListCtrl( _server_update_list );
    }
    else if ( _resource_config->_file_type == KFResource::FileType::All )
    {
        std::set< std::string > totalfilelsit = _client_update_list;
        for ( auto& filename : _server_update_list )
        {
            totalfilelsit.insert( filename );
        }
        UpdateFileListCtrl( totalfilelsit );
    }
    else
    {
        _list_file.DeleteAllItems();
    }
}

int32 CKFResourceDlg::FindListCtrlFile( const std::string& filename )
{
    auto count = _list_file.GetItemCount();
    for ( auto i = 0; i < count; ++i )
    {
        std::string text = _list_file.GetItemText( i, 0 ).GetBuffer();
        if ( text == filename )
        {
            return i;
        }
    }

    return -1;
}


void CKFResourceDlg::UpdateFileListCtrl( std::set<std::string>& filelist )
{
    // 先删除没有的
    {
        std::set<std::string> removes;
        auto count = _list_file.GetItemCount();
        for ( auto i = 0; i < count; ++i )
        {
            std::string filename = _list_file.GetItemText( i, 0 ).GetBuffer();
            auto iter = filelist.find( filename );
            if ( iter == filelist.end() )
            {
                removes.insert( filename );
            }
        }

        for ( auto& filename : removes )
        {
            auto index = FindListCtrlFile( filename );
            if ( index != -1 )
            {
                _list_file.DeleteItem( index );
            }
        }
    }

    for ( auto& filename : filelist )
    {
        auto kfsetting = _resource_config->_files.Find( filename );
        if ( kfsetting == nullptr  )
        {
            continue;
        }

        auto newadd = false;
        auto index = FindListCtrlFile( filename );
        if ( index == -1 )
        {
            newadd = true;
            index = _list_file.GetItemCount();
            _list_file.InsertItem( index, filename.c_str() );
        }
        if ( newadd || kfsetting->_md5_show != kfsetting->_md5_now )
        {
            kfsetting->_md5_show = kfsetting->_md5_now;
            _list_file.SetItemText( index, 1, KFDate::GetTimeString( kfsetting->_now_time ).c_str() );
        }
        if ( newadd || kfsetting->_show_type != kfsetting->_type )
        {
            kfsetting->_show_type = kfsetting->_type;
            _list_file.SetItemText( index, 2, KFResource::_file_type_name[ kfsetting->_type ] );
        }
    }
}

void CKFResourceDlg::OnTimer( UINT_PTR nIDEvent )
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    switch ( nIDEvent )
    {
    case __REMOVE_TIMER__:
        RemoveFileList();
        break;
    case __UPDATE_TIMER__:
        UpdateFileList();
        break;
    default:
        break;
    }

    CDialogEx::OnTimer( nIDEvent );
}

void CKFResourceDlg::OnBnClickedButtonAddAllParse()
{
    // TODO: Add your control notification handler code here
    _list_parse.ResetContent();

    if ( KFUtility::HaveBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Client ) )
    {
        for ( auto& filename : _client_update_list )
        {
            AddParseFile( filename );
        }
    }

    if ( KFUtility::HaveBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Server ) )
    {
        for ( auto& filename : _server_update_list )
        {
            AddParseFile( filename );
        }
    }
}



void CKFResourceDlg::OnBnClickedButtonRefreshFile()
{
    // TODO: Add your control notification handler code here
    _resource_config->LoadFile( KFResource::_resource_file );
    RemoveFileList();
    UpdateFileList();
}


void CKFResourceDlg::OnBnClickedButtonRemoveAllParse()
{
    // TODO: Add your control notification handler code here
    _list_parse.ResetContent();
}

void CKFResourceDlg::OnLbnDblclkListRemoveParse()
{
    // TODO: Add your control notification handler code here
    auto index = _list_parse.GetCurSel();
    if ( index != -1 )
    {
        _list_parse.DeleteString( index );
    }
}


void CKFResourceDlg::OnNMDblclkListFile( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast< LPNMITEMACTIVATE >( pNMHDR );
    // TODO: Add your control notification handler code here
    if ( pNMItemActivate->iItem != -1 )
    {
        auto strtext = _list_file.GetItemText( pNMItemActivate->iItem, 0 );
        AddParseFile( strtext.GetBuffer() );
    }

    *pResult = 0;
}

void CKFResourceDlg::OnBnClickedCheckServerType()
{
    // TODO: Add your control notification handler code here
    auto check = _check_server.GetCheck();
    if ( check == TRUE )
    {
        KFUtility::AddBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Server );
    }
    else
    {
        KFUtility::ClearBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Server );
    }

    UpdateFileList();
    _resource_config->SaveSetting( KFResource::_setting_file );
}

void CKFResourceDlg::OnBnClickedCheckClientType()
{
    // TODO: Add your control notification handler code here
    auto check = _check_client.GetCheck();
    if ( check == TRUE )
    {
        KFUtility::AddBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Client );
    }
    else
    {
        KFUtility::ClearBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Client );
    }

    UpdateFileList();
    _resource_config->SaveSetting( KFResource::_setting_file );
}


void CKFResourceDlg::OnCbnSelchangeComboServer()
{
    // TODO: Add your control notification handler code here
    _resource_config->_server_select_id = _combo_server.GetCurSel();
    _resource_config->SaveSetting( KFResource::_setting_file );
}

std::string CKFResourceDlg::BrowsePath()
{
    std::string result = "";

    BROWSEINFO bi;
    CHAR buffer[ 512 ];
    //初始化入口参数bi开始
    bi.hwndOwner = NULL;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = buffer;//此参数如为NULL则不能显示对话框
    bi.lpszTitle = "选择路径";
    bi.ulFlags = BIF_RETURNONLYFSDIRS;
    bi.lpfn = NULL;
    bi.iImage = 0;
    //初始化入口参数bi结束
    LPITEMIDLIST pIDList = SHBrowseForFolder( &bi );//调用显示选择对话框
    if ( pIDList )//选择到路径(即：点了确定按钮)
    {
        SHGetPathFromIDList( pIDList, buffer );
        result = buffer;
    }

    return result;
}

void CKFResourceDlg::OnBnClickedButtonBrowseServer()
{
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _edit_server_path.SetWindowTextA( path.c_str() );
        _resource_config->_server_path = path;
        _resource_config->SaveSetting( KFResource::_setting_file );
    }
}


void CKFResourceDlg::OnBnClickedButtonBrowseClient()
{
    // TODO: Add your control notification handler code here
    auto path = BrowsePath();
    if ( !path.empty() )
    {
        _edit_client_path.SetWindowTextA( path.c_str() );
        _resource_config->_client_path = path;
        _resource_config->SaveSetting( KFResource::_setting_file );
    }
}

void CKFResourceDlg::OnCbnSelchangeComboStore()
{
    // TODO: Add your control notification handler code here
    CString strtext;
    _combo_store.GetWindowTextA( strtext );

    if ( _resource_config->_store_name != strtext.GetBuffer() )
    {
        _resource_config->_store_name = strtext.GetBuffer();
        _resource_config->SaveFile( KFResource::_resource_file );
    }
}


void CKFResourceDlg::OnNMRClickListFile( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast< LPNMITEMACTIVATE >( pNMHDR );
    // TODO: Add your control notification handler code here

    CMenu popMenu;
    popMenu.LoadMenu( IDR_MENU2 );

    CPoint posMouse;
    GetCursorPos( &posMouse );

    CMenu* pSubMenu = NULL;
    pSubMenu = popMenu.GetSubMenu( 0 );
    pSubMenu->TrackPopupMenu( 0, posMouse.x, posMouse.y, this );

    *pResult = 0;
}


void CKFResourceDlg::OnChangeServerType()
{
    // TODO: Add your command handler code here
    ChangeFileType( KFResource::FileType::Server );
}


void CKFResourceDlg::OnChangeClientType()
{
    // TODO: Add your command handler code here
    ChangeFileType( KFResource::FileType::Client );
}


void CKFResourceDlg::OnChangeCommonType()
{
    // TODO: Add your command handler code here
    ChangeFileType( KFResource::FileType::All );
}

void CKFResourceDlg::ChangeFileType( uint32 type )
{
    auto index = _list_file.GetSelectionMark();
    if ( index == -1 )
    {
        return;
    }

    auto strtext = _list_file.GetItemText( index, 0 );
    auto kfsetting = _resource_config->FindFileSetting( strtext.GetBuffer() );
    if ( kfsetting == nullptr )
    {
        return;
    }

    kfsetting->_type = type;
    _list_file.SetItemText( index, 2, KFResource::_file_type_name[ type ] );
    _resource_config->SaveFile( KFResource::_resource_file );
}

void CKFResourceDlg::OnBnClickedButtonParse()
{
    // TODO: Add your control notification handler code here
    // 先删除配置文件
    _list_log.ResetContent();

    ParseTotalExcelFiles();
}

void CKFResourceDlg::ParseTotalExcelFiles()
{
    // 调用更新脚本
    //_store_version = ReadVersion();
    _store_version = "00000";

    // 解析文件
    auto ok = ParseExcelFiles( _store_version );
    if ( !ok )
    {
        return;
    }

    // 拷贝文件
    CopyConfigFiles();

    // 保存纪录
    _list_parse.ResetContent();
    _resource_config->SaveFile( KFResource::_resource_file );
}

std::string CKFResourceDlg::ReadVersion()
{
    {
        auto batfile = __FORMAT__( "..\\{}_update.bat ..", _resource_config->_store_name );
        system( batfile.c_str() );
    }

    // 调用脚本上传
    static const std::string _file = "..\\version.txt";
    std::ifstream versionfile( _file );
    if ( !versionfile )
    {
        return "0";
    }

    std::string strversion = "";
    getline( versionfile, strversion );
    versionfile.close();

    Poco::File file( _file );
    if ( file.exists() )
    {
        file.remove();
    }

    KFUtility::ReplaceString( strversion, " ", "" );
    auto version = KFUtility::SplitString( strversion, "." );
    return version;
}

std::string CKFResourceDlg::MakeVersion( const std::string& svnversion )
{
    auto date = KFDate::GetDate();
    auto day = ( date.GetTime() - _resource_config->_project_time ) / KFTimeEnum::OneDaySecond;

    auto version = __FORMAT__( "1.0.{}.{}", day, svnversion );
    return version;
}

bool CKFResourceDlg::ParseExcelFiles( const std::string& version )
{
    std::set< std::string > files;

    auto count = _list_parse.GetCount();
    if ( count != 0 )
    {
        // 获得需要解析的所有文件
        for ( auto i = 0; i < count; ++i )
        {
            CString strtext;
            _list_parse.GetText( i, strtext );
            files.insert( strtext.GetBuffer() );
        }
    }
    else
    {
        count = _list_file.GetItemCount();
        if ( count != 0 )
        {
            for ( auto i = 0; i < count; ++i )
            {
                CString strtext = _list_file.GetItemText( i, 0 );
                files.insert( strtext.GetBuffer() );
            }
        }
        else
        {
            for ( auto& iter : _resource_config->_files._objects )
            {
                files.insert( iter.first );
            }
        }
    }

    if ( KFUtility::HaveBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Server ) )
    {
        for ( auto& file : files )
        {
            auto kfsetting = _resource_config->FindFileSetting( file );
            if ( kfsetting == nullptr || !kfsetting->IsServerFile() )
            {
                continue;
            }

            bool ok = _kf_parse->LoadFromExcel( file.c_str(), true, KFResource::FileType::Server );
            if ( ok )
            {
                AddLog( "parse file=[{}] ok", file );
                ok = _kf_parse->SaveToXml( "..\\config\\server", version );
                if ( ok )
                {
                    kfsetting->_last_md5_server = kfsetting->_md5_now;
                }
                else
                {
                    AddLog( _kf_parse->_error );
                }
            }
            else
            {
                AddLog( _kf_parse->_error );
            }
        }
    }

    if ( KFUtility::HaveBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Client ) )
    {
        for ( auto& file : files )
        {
            auto kfsetting = _resource_config->FindFileSetting( file );
            if ( kfsetting == nullptr || !kfsetting->IsClientFile() )
            {
                continue;
            }

            bool ok = _kf_parse->LoadFromExcel( file.c_str(), true, KFResource::FileType::Client );
            if ( ok )
            {
                AddLog( "parse file=[{}] ok", file );
                ok = _kf_parse->SaveToXml( "..\\config\\client", version );
                if ( ok )
                {
                    kfsetting->_last_md5_client = kfsetting->_md5_now;
                }
                else
                {
                    AddLog( _kf_parse->_error );
                }
            }
            else
            {
                AddLog( _kf_parse->_error );
            }
        }
    }

    return true;
}

void CKFResourceDlg::CopyConfigFiles()
{
    if ( KFUtility::HaveBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Server ) )
    {
        if ( !_resource_config->_server_path.empty() )
        {
            CopyConfigFiles( "server", _resource_config->_server_path );
        }
    }

    if ( KFUtility::HaveBitMask( _resource_config->_file_type, ( uint32 )KFResource::FileType::Client ) )
    {
        if ( !_resource_config->_client_path.empty() )
        {
            CopyConfigFiles( "client", _resource_config->_client_path );
        }
    }
}

void CKFResourceDlg::CopyConfigFiles( const std::string& path, const std::string& configpath )
{
    Poco::File pocofile( "..\\config" );
    std::vector< Poco::File > configfile;
    pocofile.list( configfile );
    for ( auto& file : configfile )
    {
        if ( file.isFile() )
        {
            file.copyTo( configpath );
        }
        else
        {
            auto npos = file.path().find( path );
            if ( npos != std::string::npos )
            {
                std::vector< Poco::File > childconfigfile;
                file.list( childconfigfile );
                for ( auto& clientfile : childconfigfile )
                {
                    clientfile.copyTo( configpath );
                }
            }
        }
    }
}

void CKFResourceDlg::OnBnClickedButtonParseAndUpdate()
{
    // TODO: Add your control notification handler code here
    _list_log.ResetContent();
    ParseTotalExcelFiles();

    {
        auto batfile = __FORMAT__( "..\\{}_commit.bat ..", _resource_config->_store_name );
        system( batfile.c_str() );
    }

    auto kfserversetting = _resource_config->FindServerSetting( _resource_config->_server_select_id );
    if ( kfserversetting == nullptr || kfserversetting->_url.empty() )
    {
        return;
    }

    // 读取版本号
    auto version = MakeVersion( _store_version );
    auto resourcefile = __FORMAT__( "{}_resource_{}.tar.gz", _resource_config->_project_name, version );

    try
    {
        // 拷贝文件
        static std::string _output_path = "..\\_gcm\\conf_output";
        {
            Poco::File outputfile( _output_path );
            if ( outputfile.exists() )
            {
                outputfile.remove( true );
            }

            outputfile.createDirectory();
        }

        // server
        {
            Poco::File pocofile( "..\\config\\server" );
            pocofile.copyTo( _output_path );
            Poco::File configfile( _output_path + "\\server" );
            configfile.renameTo( _output_path + "\\config" );
        }

        // config
        {
            Poco::File pocofile( "..\\config" );
            std::vector< Poco::File > files;
            pocofile.list( files );
            for ( auto& file : files )
            {
                if ( file.isFile() )
                {
                    file.copyTo( _output_path + "\\config" );
                }
            }
        }

        // script
        {
            Poco::File pocofile( "..\\script" );
            pocofile.copyTo( _output_path );
        }
    }
    catch ( Poco::Exception& ex )
    {

    }

    // 打包文件
    {
        auto batfile = __FORMAT__( "..\\package.bat .. {} {} {} {} {}",
                                   _resource_config->_project_name, _store_version, version, resourcefile, kfserversetting->_url );
        system( batfile.c_str() );
    }
}
