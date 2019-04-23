#include "CMyListBox.h"
#include "KFDeploy.h"
#include "KFDeployDlg.h"


CMyListBox::CMyListBox()
{
}


CMyListBox::~CMyListBox()
{
}
BEGIN_MESSAGE_MAP( CMyListBox, CListBox )
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


void CMyListBox::OnRButtonUp( UINT nFlags, CPoint point )
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CMenu popMenu;
    popMenu.LoadMenu( IDR_MENU1 );

    CPoint posMouse;
    GetCursorPos( &posMouse );

    CMenu* pSubMenu = NULL;
    pSubMenu = popMenu.GetSubMenu( 2 );
    pSubMenu->TrackPopupMenu( 0, posMouse.x, posMouse.y, _deploy_manage->_dlg_deploy );

    CListBox::OnRButtonUp( nFlags, point );
}
