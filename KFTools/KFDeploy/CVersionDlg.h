#pragma once

#include "KFProtocol/KFProtocol.h"

// CVersionDlg 对话框

class CVersionDlg : public CDialogEx
{
    DECLARE_DYNAMIC( CVersionDlg )

public:
    CVersionDlg( CWnd* pParent = nullptr ); // 标准构造函数
    virtual ~CVersionDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG3 };
#endif
public:
    // 初始化界面
    void InitDialogData();

    void RefreshVersionList( const KFMsg::PBMySQLDatas* pbdatas );

protected:
    virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage( MSG* pMsg );

    CListCtrl _list_version;
    afx_msg void OnNMDblclkListVersion( NMHDR* pNMHDR, LRESULT* pResult );
};
