// CPluginDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "KFDeploy.h"
#include "CPluginDlg.h"
#include "afxdialogex.h"
#include "KFDeployDlg.h"


// CPluginDlg 对话框

IMPLEMENT_DYNAMIC( CPluginDlg, CDialogEx )

CPluginDlg::CPluginDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_DIALOG6, pParent )
{

}

CPluginDlg::~CPluginDlg()
{
}

void CPluginDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST1, _list_plugin );
}


BEGIN_MESSAGE_MAP( CPluginDlg, CDialogEx )
    ON_NOTIFY( NM_DBLCLK, IDC_LIST1, &CPluginDlg::OnNMDblclkListPlugin )
END_MESSAGE_MAP()


// CPluginDlg 消息处理程序
BOOL CPluginDlg::PreTranslateMessage( MSG* pMsg )
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

void CPluginDlg::InitDialogData()
{
    {
        auto style = _list_plugin.GetExtendedStyle();
        _list_plugin.SetExtendedStyle( style | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

        RECT rect;
        _list_plugin.GetClientRect( &rect );
        auto width = rect.right / 20.f;

        int index = 0;
        _list_plugin.InsertColumn( index++, "", LVCFMT_CENTER, width * 2 );
        _list_plugin.InsertColumn( index++, "文件名", LVCFMT_CENTER, width * 5 );
        _list_plugin.InsertColumn( index++, "插件", LVCFMT_CENTER, width * 3 );
        _list_plugin.InsertColumn( index++, "路径", LVCFMT_CENTER, width * 4 );
        _list_plugin.InsertColumn( index++, "上传时间", LVCFMT_CENTER, width * 4 );
        _list_plugin.InsertColumn( index++, "MD5", LVCFMT_CENTER, width * 4 );
        _list_plugin.DeleteColumn( 0 );
    }
}

class PluginData
{
public:
    std::string _time;
    std::string _path;
    std::string _file;
    std::string _md5;
    std::string _plugin;
};

void CPluginDlg::RefreshPluginList( const KFMsg::PBMySQLDatas* pbdatas )
{
    _list_plugin.DeleteAllItems();

    std::map< std::string, PluginData > filelist;
    for ( auto i = 0; i < pbdatas->data_size(); ++i )
    {
        if ( i >= 50 )
        {
            break;
        }

        auto pbdata = &pbdatas->data( i );
        auto pbvalues = pbdata->values();

        PluginData data;
        data._time = pbvalues[ "file_time" ];
        data._file = pbvalues[ "file_name" ];
        data._path = pbvalues[ "file_path" ];
        data._md5 = pbvalues[ "file_md5" ];
        data._plugin = pbvalues[ "plugin_name" ];
        filelist[ data._time ] = data;
    }

    auto i = 0;
    for ( auto iter = filelist.rbegin(); iter != filelist.rend(); ++iter, ++i )
    {
        auto data = &iter->second;

        // name
        auto column = 0;
        _list_plugin.InsertItem( i, data->_file.c_str() );

        // plugin
        _list_plugin.SetItemText( i, ++column, data->_plugin.c_str() );

        // path
        _list_plugin.SetItemText( i, ++column, data->_path.c_str() );

        // time
        _list_plugin.SetItemText( i, ++column, data->_time.c_str() );

        // md5
        _list_plugin.SetItemText( i, ++column, data->_md5.c_str() );
    }
}

void CPluginDlg::OnNMDblclkListPlugin( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMLISTVIEW pNMLV = reinterpret_cast< LPNMLISTVIEW >( pNMHDR );
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    auto index = pNMLV->iItem;
    if ( index == -1 )
    {
        return;
    }

    CString strfile = _list_plugin.GetItemText( index, 0 );
    CString strplugin = _list_plugin.GetItemText( index, 1 );
    CString strpath = _list_plugin.GetItemText( index, 2 );
    if ( strfile.IsEmpty() || strpath.IsEmpty() || strplugin.IsEmpty() )
    {
        return;
    }

    CString strtext;
    strtext.Format( "%s=%s=%s", strplugin.GetBuffer(), strfile.GetBuffer(), strpath.GetBuffer() );
    _deploy_manage->_dlg_deploy->_edit_param.SetWindowTextA( strtext );
    ShowWindow( SW_HIDE );
}

