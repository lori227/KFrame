#pragma once

#include "KFProtocol/KFProtocol.h"

// CPluginDlg 对话框

class CPluginDlg : public CDialogEx
{
    DECLARE_DYNAMIC( CPluginDlg )

public:
    CPluginDlg( CWnd* pParent = nullptr ); // 标准构造函数
    virtual ~CPluginDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG6 };
#endif

    void InitDialogData();

    void RefreshPluginList( const KFMsg::PBMySQLDatas* pbdatas );

protected:
    virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    CListCtrl _list_plugin;
    virtual BOOL PreTranslateMessage( MSG* pMsg );
    afx_msg void OnNMDblclkListPlugin( NMHDR* pNMHDR, LRESULT* pResult );
};
