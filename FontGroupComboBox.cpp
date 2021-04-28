/* 
 *  Filename:    FontGroupComboBox.cpp
 *  Description: Adapter to CGroupComboBox class for displaying fonts
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
#include "FontGroupComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GCB_ICON_MARGIN 1
#define GCB_FONTNAME_MARGIN 2
#define GCB_ICON_SIZE 16

//////////////////////////////////////////////////////////////////////////
// CGroupComboBoxFontItem implementation

CGroupComboBoxFontItem::CGroupComboBoxFontItem(LOGFONT* font) 
	: CGroupComboBoxItem(font->lfFaceName)
{
	m_font = new CFont();
	m_font->CreateFontIndirect(font);
}

CGroupComboBoxFontItem::~CGroupComboBoxFontItem()
{
	m_font->DeleteObject();
	delete m_font;
}

void CGroupComboBoxFontItem::Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct)
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

	CFont* oldFont = dc->SelectObject(m_font);
	rect.left += GCB_FONTNAME_MARGIN;
	dc->DrawText(GetCaption(), &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	dc->SelectObject(oldFont);
	dc->SetTextColor(crOldTextColor);
	dc->SetBkColor(crOldBkColor);
}

CSize CGroupComboBoxFontItem::GetSize(CDC* dc)
{
	RECT rect = { 0 };
	CFont* oldFont = dc->SelectObject(m_font);
	CSize size = dc->GetTextExtent(GetCaption());
	size.cx += 2 * GCB_FONTNAME_MARGIN;
	dc->SelectObject(oldFont);
	return size;
}

// CGroupComboBoxFontItemWithIcon

CGroupComboBoxFontItemWithIcon::CGroupComboBoxFontItemWithIcon(LOGFONT* font, CImageList* imageList, int imageIndex)
	: CGroupComboBoxItem(font->lfFaceName)
	, m_imageList(imageList)
	, m_imageIndex(imageIndex)
{
	m_font = new CFont();
	m_font->CreateFontIndirect(font);
}

CGroupComboBoxFontItemWithIcon::~CGroupComboBoxFontItemWithIcon()
{
	m_font->DeleteObject();
	delete m_font;
}

void CGroupComboBoxFontItemWithIcon::Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct)
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

	POINT pt = { rect.left + GCB_ICON_MARGIN, rect.top + (rect.bottom - rect.top - GCB_ICON_SIZE) / 2 };
	m_imageList->Draw(dc, m_imageIndex, pt, ILD_NORMAL);
	rect.left += 2 * GCB_ICON_MARGIN + GCB_ICON_SIZE + GCB_FONTNAME_MARGIN;

	CFont* oldFont = dc->SelectObject(m_font);
	dc->DrawText(GetCaption(), &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	dc->SelectObject(oldFont);

	dc->SetTextColor(crOldTextColor);
	dc->SetBkColor(crOldBkColor);
}

CSize CGroupComboBoxFontItemWithIcon::GetSize(CDC* dc)
{
	RECT rect = { 0 };
	CFont* oldFont = dc->SelectObject(m_font);
	CSize size = dc->GetTextExtent(GetCaption());
	if (size.cy < GCB_ICON_SIZE)
		size.cy = GCB_ICON_SIZE;
	size.cx += 2 * GCB_ICON_MARGIN + GCB_ICON_SIZE + 2 * GCB_FONTNAME_MARGIN;
	dc->SelectObject(oldFont);
	return size;
}

// CFontGroupComboBox

IMPLEMENT_DYNAMIC(CFontGroupComboBox, CGroupComboBox)

int CFontGroupComboBox::AddGroup(LPCTSTR caption, bool isSorted)
{
	return CGroupComboBox::AddGroup(new CGroupComboBoxHeader(caption, isSorted));
}

int CFontGroupComboBox::AddItem(LOGFONT* font, int groupIndex)
{
	return CGroupComboBox::AddItem(new CGroupComboBoxFontItem(font), groupIndex);
}

int CFontGroupComboBox::AddItem(LOGFONT* font, CImageList* imageList, int imageIndex, int groupIndex)
{
	return CGroupComboBox::AddItem(new CGroupComboBoxFontItemWithIcon(font, imageList, imageIndex), groupIndex);
}

int CFontGroupComboBox::InsertItem(LOGFONT* font, int groupIndex)
{
	return CGroupComboBox::InsertItem(new CGroupComboBoxFontItem(font), groupIndex);
}

int CFontGroupComboBox::InsertRecentItem(LPCTSTR caption)
{
	return CGroupComboBox::InsertItem(new CGroupComboBoxSimpleItem(caption), 0);
}

BEGIN_MESSAGE_MAP(CFontGroupComboBox, CGroupComboBox)
END_MESSAGE_MAP()


// CFontGroupComboBoxVariable

IMPLEMENT_DYNAMIC(CFontGroupComboBoxVariable, CGroupComboBoxVariable)

int CFontGroupComboBoxVariable::AddGroup(LPCTSTR caption, bool isSorted)
{
	return CGroupComboBoxVariable::AddGroup(new CGroupComboBoxHeader(caption, isSorted));
}

int CFontGroupComboBoxVariable::AddItem(LOGFONT* font, int groupIndex)
{
	return CGroupComboBoxVariable::AddItem(new CGroupComboBoxFontItem(font), groupIndex);
}

int CFontGroupComboBoxVariable::AddItem(LOGFONT* font, CImageList* imageList, int imageIndex, int groupIndex)
{
	return CGroupComboBoxVariable::AddItem(new CGroupComboBoxFontItemWithIcon(font, imageList, imageIndex), groupIndex);
}

int CFontGroupComboBoxVariable::InsertItem(LOGFONT* font, int groupIndex)
{
	return CGroupComboBoxVariable::InsertItem(new CGroupComboBoxFontItem(font), groupIndex);
}

int CFontGroupComboBoxVariable::InsertRecentItem(LPCTSTR caption)
{
	return CGroupComboBoxVariable::InsertItem(new CGroupComboBoxSimpleItem(caption), 0);
}

BEGIN_MESSAGE_MAP(CFontGroupComboBoxVariable, CGroupComboBoxVariable)
END_MESSAGE_MAP()
