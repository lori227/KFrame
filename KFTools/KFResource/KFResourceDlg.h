
// KFResourceDlg.h : header file
//

#pragma once

#include "KFResourceData.h"


// CKFResourceDlg dialog
class CKFResourceDlg : public CDialogEx
{
    // Construction
public:
    CKFResourceDlg( CWnd* pParent = nullptr );	// standard constructor

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_KFRESOURCE_DIALOG };
#endif

protected:
    virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV support

protected:
    // 初始化
    void InitFrameData();

    // 初始化空间信息
    void InitDialogData();

    // 初始化逻辑
    void InitLogicData();

    // 加载资源配置
    void LoadResourceData();

    // 判断是否是资源文件
    bool IsResourceFile( const std::string& path );

    // 初始化文件列表
    void UpdateFileList();
    void RemoveFileList();

    void UpdateFileListCtrl();
    void UpdateFileListCtrl( std::set< std::string >& filelist );
    int32 FindListCtrlFile( const std::string& filename );

    // 更新文件类型
    void ChangeFileType( uint32 type );

    // 删除需要解析的文件
    bool HaveParseFile( const std::string& filename );
    void AddParseFile( const std::string& filename );
    void RemoveParseFile( const std::string& filename );

    // 浏览选择目录
    std::string BrowsePath();

    // 读取版本号
    std::string ReadVersion();
    std::string MakeVersion( const std::string& svnversion );

    // 解析文件
    void ParseTotalExcelFiles();
    bool ParseExcelFiles( const std::string& version );

    // 拷贝文件
    void CopyConfigFiles();
    void CopyConfigFiles( const std::string& path, const std::string& configpath );

    template< class...Args >
    inline void AddLog( const std::string& myfmt, Args&& ... args )
    {
        auto content = __FORMAT__( myfmt, std::forward<Args>( args )... );
        _list_log.AddString( content.c_str() );
    }
protected:
    // 有变更的文件
    std::set< std::string > _client_update_list;
    std::set< std::string > _server_update_list;

    // 仓库版本号
    std::string _store_version;

    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTimer( UINT_PTR nIDEvent );
    virtual BOOL PreTranslateMessage( MSG* pMsg );

    CListCtrl _list_file;
    CListBox _list_parse;
    afx_msg void OnBnClickedButtonAddAllParse();
    afx_msg void OnBnClickedButtonRemoveAllParse();
    afx_msg void OnLbnSelchangeListRemoveParse();
    afx_msg void OnBnClickedCheckServerType();
    CButton _check_server;
    CButton _check_client;
    afx_msg void OnBnClickedCheckClientType();
    CComboBox _combo_server;
    afx_msg void OnCbnSelchangeComboServer();
    CEdit _edit_server_path;
    CEdit _edit_client_path;
    afx_msg void OnBnClickedButtonBrowseServer();
    afx_msg void OnBnClickedButtonBrowseClient();
    afx_msg void OnBnClickedButtonParse();
    CComboBox _combo_store;
    afx_msg void OnCbnSelchangeComboStore();
    afx_msg void OnNMDblclkListFile( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnNMRClickListFile( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnChangeServerType();
    afx_msg void OnChangeClientType();
    afx_msg void OnChangeCommonType();
    afx_msg void OnLbnDblclkListRemoveParse();
    afx_msg void OnBnClickedButtonParseAndUpdate();
    CListBox _list_log;
};
