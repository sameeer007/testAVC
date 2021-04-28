/* 
 *  Filename:    GroupComboBoxItemWithIcon.cpp
 *  Description: Adapter to CGroupComboBox class for displaying items with icon
 *  Copyright:   Julijan Sribar, 2012-2013
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the author(s) be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "stdafx.h"
#include "TestApplication.h"
#include "GroupComboBoxWithIcons.h"
#include "ShellDlgFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GCB_ICON_SIZE 16
#define GCB_ICON_LEFTMARGIN 2
#define GCB_ICON_TOPMARGIN 1
#define GCB_ICONTOTEXT_MARGIN 4

// CGroupComboBoxItemWithIcon
CGroupComboBoxItemWithIcon::CGroupComboBoxItemWithIcon(LPCTSTR caption, UINT iconID) 
	: CGroupComboBoxItem(caption)
	, m_iconId(iconID)
{
}

void CGroupComboBoxItemWithIcon::Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	COLORREF crOldTextColor = dc->GetTextColor();
	COLORREF crOldBkColor = dc->GetBkColor();

	RECT rect = lpDrawItemStruct->rcItem;
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		dc->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		dc->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		dc->FillSolidRect(&rect, ::GetSysColor(COLOR_HIGHLIGHT));
	}
	else
		dc->FillSolidRect(&rect, crOldBkColor);

	HICON icon = (HICON)::LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(m_iconId), IMAGE_ICON, GCB_ICON_SIZE, GCB_ICON_SIZE, 0);
	::DrawIconEx(dc->m_hDC, rect.left + GCB_ICON_LEFTMARGIN, rect.top + GCB_ICON_TOPMARGIN, icon, GCB_ICON_SIZE, GCB_ICON_SIZE, NULL, NULL, DI_NORMAL);
	{
		CShellDlgFont shellFont(dc);
		rect.left += GCB_ICON_LEFTMARGIN + GCB_ICON_SIZE + GCB_ICONTOTEXT_MARGIN;
		dc->DrawText(GetCaption(), &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	::DestroyIcon(icon);

	dc->SetTextColor(crOldTextColor);
	dc->SetBkColor(crOldBkColor);
}

CSize CGroupComboBoxItemWithIcon::GetSize(CDC* dc)
{
	RECT rect = { 0 };
	CShellDlgFont shellFont(dc);
	CSize size = dc->GetTextExtent(GetCaption());
	if (size.cy < GCB_ICON_SIZE)
		size.cy = GCB_ICON_SIZE;
	size.cx += 2 * GCB_ICONTOTEXT_MARGIN + GCB_ICON_LEFTMARGIN + GCB_ICON_SIZE;
	return size;
}

// CGroupComboBoxWithIcons

IMPLEMENT_DYNAMIC(CGroupComboBoxWithIcons, CGroupComboBox)

CGroupComboBoxWithIcons::CGroupComboBoxWithIcons()
{
}

CGroupComboBoxWithIcons::~CGroupComboBoxWithIcons()
{
}

int CGroupComboBoxWithIcons::AddGroup(LPCTSTR caption, bool isSorted/* = false*/)
{
	return CGroupComboBox::AddGroup(new CGroupComboBoxHeader(caption, isSorted));
}

int CGroupComboBoxWithIcons::AddItem(LPCTSTR caption, UINT iconID, int groupIndex)
{
	return CGroupComboBox::AddItem(new CGroupComboBoxItemWithIcon(caption, iconID), groupIndex);
}

BEGIN_MESSAGE_MAP(CGroupComboBoxWithIcons, CGroupComboBox)
END_MESSAGE_MAP()

// CGroupComboBoxWithIcons message handlers
