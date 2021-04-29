/* 
 *  Filename:    GroupComboBox.cpp
 *  Description: Group Combo Box implementation
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
#include "GroupComboBox.h"
#include "ShellDlgFont.h"
#include "MemDC.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GCB_TEXT_MARGIN 1
#define GCB_HEADER_INDENT 4
#define GCB_HEADER_REL_SIZE 2

//////////////////////////////////////////////////////////////////////////
// CGroupComboBoxHeader implementation

CGroupComboBoxHeader::CGroupComboBoxHeader(LPCTSTR caption, bool sorted /*= false*/)
	: CGroupComboBoxItem(caption, false)
	, m_isSorted(sorted)

	m_itemCompare = m_isSorted ? new CComboBoxItemCompare() : NULL;
}

template<typename TCompare>
CGroupComboBoxHeader::CGroupComboBoxHeader(LPCTSTR caption, const TCompare& compare)
	: CGroupComboBoxItem(caption, true)
	, m_isSorted(true)
	, m_itemCompare(new TCompare())
{
}

CGroupComboBoxHeader::~CGroupComboBoxHeader()
{
	delete m_itemCompare;
}

void CGroupComboBoxHeader::Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	COLORREF crOldTextColor = dc->GetTextColor();
	COLORREF crOldBkColor = dc->GetBkColor();

	RECT rect = lpDrawItemStruct->rcItem;
	dc->FillSolidRect(&rect, ::GetSysColor(COLOR_MENUBAR));

	rect.left += GCB_HEADER_INDENT;
	{
		CShellDlgFont dlgFont(dc, FW_BOLD);
		dc->DrawText(GetCaption(), &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	dc->SetTextColor(crOldTextColor);
	dc->SetBkColor(crOldBkColor);
}

CSize CGroupComboBoxHeader::GetSize(CDC* dc)
{
	CShellDlgFont dlgFont(dc, FW_BOLD);
	CSize size = dc->GetTextExtent(GetCaption());
	size.cx += 2 * GCB_HEADER_INDENT;
	size.cy *= GCB_HEADER_REL_SIZE;
	return size;
}

//////////////////////////////////////////////////////////////////////////
// CGroupComboBoxSimpleItem implementation

CGroupComboBoxSimpleItem::CGroupComboBoxSimpleItem(LPCTSTR caption) 
	: CGroupComboBoxItem(caption)
{
}

void CGroupComboBoxSimpleItem::Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct)
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

	rect.left += GCB_TEXT_MARGIN;
	{
		CShellDlgFont dlgFont(dc);
		dc->DrawText(GetCaption(), &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	dc->SetTextColor(crOldTextColor);
	dc->SetBkColor(crOldBkColor);
}

CSize CGroupComboBoxSimpleItem::GetSize(CDC* dc)
{
	CShellDlgFont dlgFont(dc);
	CSize size = dc->GetTextExtent(GetCaption());
	size.cx += GCB_TEXT_MARGIN * 2;
	return size;
}

//////////////////////////////////////////////////////////////////////////
// CGroupListBox implementation

typedef CGroupComboBox::CGroupListBox CGroupListBox;

IMPLEMENT_DYNAMIC(CGroupListBox, CListBox)

// CGroupListBox overrides

void CGroupListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	{
		CRect rect(lpDrawItemStruct->rcItem);
		KRule::CMemDC memDC(&dc, &rect);
		CGroupComboBoxItem* item = reinterpret_cast<CGroupComboBoxItem*>(lpDrawItemStruct->itemData);
		ASSERT(item != NULL);
		item->Draw(&memDC, lpDrawItemStruct);
		// to ensure that highlighted item is shown even for fast scrolling
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
			InvalidateRect(&(lpDrawItemStruct->rcItem));
	}
	dc.Detach();
}

BOOL CGroupListBox::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		CPoint pt(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
		BOOL bOutside;
		int index = ItemFromPoint(pt, bOutside);
		// if user clicks on group header item, list should remain open
		if (bOutside == FALSE && reinterpret_cast<CGroupComboBoxItem*>(GetItemData(index))->IsGroupHeader())
			return TRUE;
		break;
	}
	return CListBox::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CGroupListBox, CListBox)
END_MESSAGE_MAP()

// CGroupListBox message handlers


//////////////////////////////////////////////////////////////////////////
// CGroupComboBox implementation

IMPLEMENT_DYNAMIC(CGroupComboBox, CComboBox)

	// public members

CGroupComboBox::CGroupComboBox()
	: m_dropdownListAutoWidth(true)
	, m_autocomplete(true)
	, m_selectionUndoByEscKey(true)
{
}

CGroupComboBox::~CGroupComboBox()
{
	std::for_each(m_headers.begin(), m_headers.end(), [](const CGroupComboBoxHeader* groupHeader) { delete groupHeader; } );
}

// Appends a header to the combo-box. Method returns index of inserted header in headers 
// list. Since this index doesn't change (new items are always appended, existing items 
// are not removed), it can be used as a reference for group items
int CGroupComboBox::AddGroup(CGroupComboBoxHeader* groupHeader)
{
	m_headers.push_back(groupHeader);
	return m_headers.size() - 1;
}

// Adds an item to group. If group header has sorted flag set, item is inserted accordingly;
// otherwise it is appended to the end of the group.
int CGroupComboBox::AddItem(CGroupComboBoxItem* item, int groupIndex)
{
	ASSERT(item != NULL);
	ASSERT(groupIndex < static_cast<int>(m_headers.size()));
	CGroupComboBoxHeader* groupHeader = m_headers[groupIndex];
	ASSERT(groupHeader != NULL);
	groupHeader->AssignItem(item);
	// if group doesn't have items, it is not shown yet
	if (FindItem(groupHeader) == CB_ERR)
		ShowGroupHeader(groupHeader);
	CGroupBounds gb = GetGroupBounds(groupHeader);
	ASSERT(gb.FirstIndex >= 0);
	ASSERT(gb.GroupEnd <= GetCount());
	int index = (groupHeader->IsSorted()) ? GetInsertionIndex(groupHeader, gb.FirstIndex, gb.GroupEnd, item) : gb.GroupEnd;
	ASSERT(index <= GetCount());
	ASSERT((GetStyle() & CBS_HASSTRINGS) == 0);
	return SendMessage(CB_INSERTSTRING, index, LPARAM(item));
}

// Inserts an item to the top of the group, regardless of group header sorting flag.
int CGroupComboBox::InsertItem(CGroupComboBoxItem* item, int groupIndex)
{
	ASSERT(item != NULL);
	ASSERT(groupIndex < static_cast<int>(m_headers.size()));
	CGroupComboBoxHeader* groupHeader = m_headers[groupIndex];
	ASSERT(groupHeader != NULL && groupHeader->IsSorted() == false);
	groupHeader->AssignItem(item);
	// if group doesn't have items, it is not shown currently
	if (FindItem(groupHeader) == CB_ERR)
		ShowGroupHeader(groupHeader);
	int insertIndex = FindItem(groupHeader) + 1;
	ASSERT(insertIndex <= GetCount());
	ASSERT((GetStyle() & CBS_HASSTRINGS) == 0);
	return SendMessage(CB_INSERTSTRING, insertIndex, LPARAM(item));
}

// Deletes the item. Only non-header items can be deleted
void CGroupComboBox::DeleteItem(int index)
{
	ASSERT(index > CB_ERR && index <= GetCount());
	CGroupComboBoxItem* item = GetComboBoxItem(index);
	ASSERT(item->IsGroupHeader() == false);
	CGroupComboBoxHeader* groupHeader = item->GetGroupHeader();
	ASSERT(groupHeader != NULL);
	SendMessage(CB_DELETESTRING, index, 0);
	// if no items in the group left, hide (i.e. remove) the group header too:
	// if item removed was the last one or if there was a header below it
	if (index >= GetCount() || GetComboBoxItem(index)->IsGroupHeader())
	{
		// then check if it was preceded with another header
		--index;
		ASSERT(index >= 0);
		if (GetComboBoxItem(index)->IsGroupHeader())
			SendMessage(CB_DELETESTRING, index, 0);
	}
}

// Finds the first string that contains prefix provided
int CGroupComboBox::FindString(int nStartAfter, LPCTSTR lpszString) const
{
	int strLen = _tcslen(lpszString);
	int index = nStartAfter + 1;
	for (int i = 0; i < GetCount(); ++i)
	{
		CGroupComboBoxItem* item = GetComboBoxItem(index);
		if (item->IsGroupHeader() == false)
		{
			if (item->GetCaption().GetLength() >= strLen)
			{
				if (item->CompareCaption(lpszString, strLen) == 0)
					return index;
			}
		}
		++index;
		// if reached the end, start from top
		if (index >= GetCount())
			index = 0;
	}
	return CB_ERR;
}

// Finds item with caption that matches the string specified
int CGroupComboBox::FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
{
	int index = nIndexStart + 1;
	for (int i = 0; i < GetCount(); ++i)
	{
		CGroupComboBoxItem* item = GetComboBoxItem(index);
		if (item->IsGroupHeader() == false)
		{
			if (item->CompareCaption(lpszFind) == 0)
				return index;
		}
		++index;
		// if reached the end, start from top
		if (index >= GetCount())
			index = 0;
	}
	return CB_ERR;
}

// Finds item with matching caption in a group
int CGroupComboBox::FindStringExactInGroup(LPCTSTR lpszString, int groupIndex) const
{
	ASSERT(groupIndex < static_cast<int>(m_headers.size()));
	CGroupComboBoxHeader* groupHeader = m_headers[groupIndex];
	ASSERT(groupHeader != NULL);
	CGroupBounds groupBounds = GetGroupBounds(groupHeader);
	if (groupBounds.FirstIndex == groupBounds.GroupEnd)
		return CB_ERR;
	int index = FindStringExact(groupBounds.FirstIndex - 1, lpszString);
	if (index == CB_ERR || index >= groupBounds.GroupEnd)
		return CB_ERR;
	return index;
}

// virtual methods overrides

BOOL CGroupComboBox::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		// preprocess Up/Down arrow and PageUp/PageDown keys to avoid selection of group headers
		switch (pMsg->wParam)
		{
		case VK_DOWN:
			if (PreprocessVkDown())
				return TRUE;
			break;
		case VK_UP:
			if (PreprocessVkUp())
				return TRUE;
			break;
		case VK_NEXT:
			if (PreprocessVkPageDown())
				return TRUE;
			break;
		case VK_PRIOR:
			if (PreprocessVkPageUp())
				return TRUE;
			break;
		case VK_DELETE:
		case VK_BACK:
			if (GetDroppedState())
			{
				// to correctly handle deletion
				DWORD sel = GetEditSel();
				int start = LOWORD(sel);
				int end = HIWORD(sel);
				SetCurSel(0);
				SetEditSel(start, end);
			}
			break;
		}
		break;
	case WM_CHAR:
		if (GetDroppedState())
		{
			if (m_autocomplete && _istprint(pMsg->wParam))
			{
				// fill-up the text in edit control with matching item
				CString text;
				GetWindowText(text);
				DWORD sel = GetEditSel();
				int start = LOWORD(sel);
				int end = HIWORD(sel);
				if (start != end)
					text.Delete(start, end - start);
				text.AppendChar(TCHAR(pMsg->wParam));
				if (FindAndAutocompleteItem(text))
					return TRUE;
			}
			else if (m_selectionUndoByEscKey && TCHAR(pMsg->wParam) == VK_ESCAPE)
			{
				// revert the content of edit control
				ShowDropDown(FALSE);
				SetWindowText(m_previousString);
				SetEditSel(0, -1);
				SetCurSel(FindStringExact(-1, m_previousString));
				return TRUE;
			}
		}
		break;
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

// Corrects owner-drown combo box height
void CGroupComboBox::PreSubclassWindow()
{
	CClientDC dc(this);
	CFont* font = GetFont();
	CFont* fontOld = dc.SelectObject(font);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int border = ::GetSystemMetrics(SM_CYBORDER);
	// height of edit control
	SetItemHeight(-1, tm.tmHeight + 2 * border);
	// height of items in drop-down list
	SetItemHeight(0, tm.tmHeight);
	dc.SelectObject(fontOld);

	CComboBox::PreSubclassWindow();
}

BEGIN_MESSAGE_MAP(CGroupComboBox, CComboBox)
	ON_WM_DELETEITEM()
	ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, &CGroupComboBox::OnCbnSelchange)
	ON_CONTROL_REFLECT_EX(CBN_DROPDOWN, &CGroupComboBox::OnCbnDropdown)
	ON_MESSAGE(WM_CTLCOLORLISTBOX, &CGroupComboBox::OnCtlColorListBox)
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CGroupComboBox message handlers

// Deallocates deleted item memory
void CGroupComboBox::OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	CGroupComboBoxItem* item = reinterpret_cast<CGroupComboBoxItem*>(lpDeleteItemStruct->itemData);
	// only non-header items must be deallocated (headers are deallocated in control's destructor)
	if (item->IsGroupHeader() == false)
		delete item;
	CComboBox::OnDeleteItem(nIDCtl, lpDeleteItemStruct);
}

// Copies selected item into edit control
BOOL CGroupComboBox::OnCbnSelchange()
{
	int index = GetCurSel();
	if (index != CB_ERR)
	{
		CGroupComboBoxItem* item = GetComboBoxItem(index);
		// copy item text for regular (non-header) items only
		if (item->IsGroupHeader() == false)
		{
			SetWindowText(item->GetCaption());
			SetEditSel(0, -1);
		}
	}
	return FALSE;
}

BOOL CGroupComboBox::OnCbnDropdown()
{
	// override bug in CComboBox: when drop-down list is opened by F4 mouse pointer remains caret
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	// save text for ESC functionality
	GetWindowText(m_previousString);
	return FALSE;
}

// Subclasses drop-down list box when list-box is displayed for the first time
// and ensures that selected item is highlighted
LRESULT CGroupComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam)
{
	// subclass ListBox control
	if (m_listBox.GetSafeHwnd() == NULL)
		m_listBox.SubclassWindow(reinterpret_cast<HWND>(lParam));

	if (m_dropdownListAutoWidth)
		AdjustDropdownListWidth();

	if (GetWindowTextLength() > 0)
	{
		// check is required to prevent recursion (SetCurSel triggers new WM_CTLCOLOR message)
		if (GetCurSel() == CB_ERR)
			FindStringAndSelect();
	}
	return 0;
}

void CGroupComboBox::OnDestroy()
{
	if (m_listBox.GetSafeHwnd() != NULL)
		m_listBox.UnsubclassWindow();
	CComboBox::OnDestroy();
}

// Processes left button-up message to prevent selection change and list scroll
void CGroupComboBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	// store selected item index
	int index = GetCurSel();
	int top = GetTopIndex();
	// prevent list-box update and consequent flickering
	COMBOBOXINFO cbi = { sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(&cbi);
	::SendMessage(cbi.hwndList, WM_SETREDRAW, FALSE, 0);

	CComboBox::OnLButtonUp(nFlags, point);

	if (GetDroppedState() && index != CB_ERR)
		ChangeSelection(index, top);
	::SendMessage(cbi.hwndList, WM_SETREDRAW, TRUE, 0);
}

// protected methods

// Finds the index where item must be inserted according to sorting order
int CGroupComboBox::GetInsertionIndex(const CGroupComboBoxHeader* header, int groupFirst, int groupEnd, const CGroupComboBoxItem* newItem) const
{
	ASSERT(groupFirst >= 0 && groupFirst <= GetCount());
	ASSERT(groupEnd >= 0 && groupEnd <= GetCount());
	ASSERT(groupFirst <= groupEnd);
	ASSERT((GetStyle() & CBS_HASSTRINGS) == 0);

	int count = groupEnd - groupFirst;
	while (count > 0)
	{
		int i = groupFirst;
		int step = count / 2;
		i += step;
		CGroupComboBoxItem* item = GetComboBoxItem(i);
		ASSERT(item->IsGroupHeader() == false);
		if (header->ItemCompare(item, newItem) < 0)
		{
			groupFirst = ++i;
			count -= step + 1;
		}
		else
			count = step;
	}
	return groupFirst;
}

// Gets the group bounds (index of the first item and index of the item below last item in the group)
CGroupComboBox::CGroupBounds CGroupComboBox::GetGroupBounds(const CGroupComboBoxHeader* header) const
{
	int firstIndex = FindItem(header);
	if (firstIndex == -1)
		return CGroupBounds(-1, -1);
	++firstIndex;
	ASSERT(firstIndex > 0);
	int groupEnd = FindNextHeader(firstIndex);
	if (groupEnd == CB_ERR)
		groupEnd = GetCount();
	return CGroupBounds(firstIndex, groupEnd);
}

// private methods

// Finds the item
int CGroupComboBox::FindItem(const CGroupComboBoxItem* find) const
{
	for (int i = 0; i < GetCount(); ++i)
	{
		CGroupComboBoxItem* item = GetComboBoxItem(i);
		if (find == item)
			return i;
	}
	return CB_ERR;
}

// Finds next header below item provided
int CGroupComboBox::FindNextHeader(int start) const
{
	ASSERT(start >= 0);
	for (int i = start; i < GetCount(); ++i)
	{
		if (GetComboBoxItem(i)->IsGroupHeader())
			return i;
	}
	return CB_ERR;
}

// Finds next non-header item below item provided
int CGroupComboBox::FindNextNonHeaderItem(int index) const
{
	do 
	{
		++index;
		// reached the end
		if (index >= GetCount())
			return CB_ERR;
	} while (GetComboBoxItem(index)->IsGroupHeader());
	ASSERT(index < GetCount());
	return index;
}

// Finds previous non-header item above item provided
int CGroupComboBox::FindPreviousNonHeaderItem(int index) const
{
	do 
	{
		--index;
		// reached the top
		if (index <= 0)
			return CB_ERR;
	} while (GetComboBoxItem(index)->IsGroupHeader());
	ASSERT(index >= 0);
	return index;
}

// Inserts the header into the list
void CGroupComboBox::ShowGroupHeader(const CGroupComboBoxHeader* header)
{
	ASSERT(FindItem(header) == CB_ERR); // header must not be visible
	int index = GetCount();
	if (index > 0)
		index = FindGroupHeaderInsertPosition(header);
	ASSERT(index >= 0 && index <= GetCount());
	ASSERT((GetStyle() & CBS_HASSTRINGS) == 0);
	SendMessage(CB_INSERTSTRING, index, LPARAM(header));
}

// Finds the position where header must be inserted
int CGroupComboBox::FindGroupHeaderInsertPosition(const CGroupComboBoxHeader* header) const
{
	ASSERT(FindItem(header) == CB_ERR); // header must not be visible
	HeadersList::const_iterator headerIt = std::find(m_headers.begin(), m_headers.end(), header);
	// first header is always on the top
	if (headerIt == m_headers.begin())
		return 0;
	ASSERT(headerIt != m_headers.end());
	// locate first next visible header
	for (HeadersList::const_iterator it = ++headerIt; it != m_headers.end(); ++it)
	{
		int nextHeaderIndex = FindItem(*it);
		if (nextHeaderIndex != CB_ERR)
			return nextHeaderIndex;
	}
	// none found then append header to the end
	return GetCount();
}

// Changes selection without excessive scrolling; SetCurSel function
// often scrolls the view even if item to be selected is already visible
void CGroupComboBox::ChangeSelection(int newSelection, int top)
{
	COMBOBOXINFO cbi = { sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(&cbi);
	::SendMessage(cbi.hwndList, WM_SETREDRAW, FALSE, 0);
	SetCurSel(newSelection);
	SetTopIndex(top);
	::SendMessage(cbi.hwndList, WM_SETREDRAW, TRUE, 0);
	::RedrawWindow(cbi.hwndList, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

// Selects the item without scrolling if item is visible
void CGroupComboBox::SelectItem(int index)
{
	ASSERT(index != -1);
	if (GetDroppedState())
	{
		int top = GetTopIndex();
		if (top > index)
			top = index;
		else if (index > GetBottomForItem(top))
			top = GetTopForItem(index);
		ChangeSelection(index, top);
	}
	else
		SetCurSel(index);
}

// Selects the item and copies text to edit control
void CGroupComboBox::SelectAndCopyItem(int index)
{
	SelectItem(index);
	SetWindowText(GetComboBoxItem(index)->GetCaption());
	SetEditSel(0, -1);
}

// Finds item with matching string beginning, selects it and
// copies the item caption into edit control
bool CGroupComboBox::FindAndAutocompleteItem(const CString& text)
{
	ASSERT(GetDroppedState());
	int start = GetCurSel();
	if (start < 0)
		start = 0;
	int index = FindString(start - 1, text);
	if (index == CB_ERR)
		return false;
	SelectItem(index);
	SetWindowText(GetComboBoxItem(index)->GetCaption());
	// select only auto-filled text so that user can continue typing
	SetEditSel(text.GetLength(), -1);
	return true;
}

// Finds the item that matches string in the edit control
bool CGroupComboBox::FindStringAndSelect()
{
	ASSERT(GetWindowTextLength() > 0);
	CString text;
	GetWindowText(text);
	int index = FindStringExact(-1, text);
	if (index == CB_ERR)
		index = FindString(-1, text);
	if (index == CB_ERR)
		return false;
	SelectAndCopyItem(index);
	return true;
}

// Prevents VkDown key to select a header
bool CGroupComboBox::PreprocessVkDown()
{
	int index = GetCurSel();
	// if nothing selected yet, find first match
	if (index < 0)
	{
		if (GetWindowTextLength() > 0)
		{
			if (FindStringAndSelect())
				return true;
		}
	}
	++index;
	if (index < GetCount())
	{
		// check if next item is header to skip it
		if (GetComboBoxItem(index)->IsGroupHeader())
		{
			index = FindNextNonHeaderItem(index);
			if (index >= 0 && index < GetCount())
			{
				SelectAndCopyItem(index);
				return true;
			}
		}
	}
	return false;
}

// Prevents VkUp key to select a header
bool CGroupComboBox::PreprocessVkUp()
{
	int index = GetCurSel();
	// if nothing selected yet, find first match
	if (index < 0)
	{
		if (GetWindowTextLength() > 0)
		{
			if (FindStringAndSelect())
				return true;
		}
		// if no match, select first item
		index = FindNextNonHeaderItem(-1);
		if (index != CB_ERR)
		{
			SelectAndCopyItem(index);
			return true;
		}
	}
	if (index > 0)
	{
		--index;
		// if next selection is header, skip it
		if (GetComboBoxItem(index)->IsGroupHeader())
		{
			index = FindPreviousNonHeaderItem(index);
			if (index >= 0)
			{
				SelectAndCopyItem(index);
				return true;
			}
			// the topmost item is selected already, 
			// so just scroll up to reveal the header
			if (GetTopIndex() > 0)
				SetTopIndex(0);
			return true;
		}
	}
	return false;
}

// Prevents VkPageDown key to select a header
bool CGroupComboBox::PreprocessVkPageDown()
{
	int index = GetCurSel();
	// already at the bottom
	if (index == GetCount() - 1)
		return true;
	if (index == CB_ERR)
		index = 0;
	// if next selection is header, select item above it
	// so that current selection remains visible
	int bottom = GetBottomForItem(index);
	if (GetComboBoxItem(bottom)->IsGroupHeader())
	{
		index = FindPreviousNonHeaderItem(bottom);
		SelectAndCopyItem(index);
		return true;
	}
	return false;
}

// Prevents VkPageUp key to select a header 
bool CGroupComboBox::PreprocessVkPageUp()
{
	int index = GetCurSel();
	// theoretically, this is not possible
	if (index == 0)
		return true;
	ASSERT(GetComboBoxItem(0)->IsGroupHeader());
	// scroll up to reveal the topmost header
	if (index == 1)
	{
		if (GetTopIndex() != 0)
			SetTopIndex(0);
		return true;
	}
	if (index < 0)
		index = 0;
	// if next selection will be a header, select item below it
	// so that currently selected item remains visible, but show the header
	index = GetTopForItem(index);
	if (GetComboBoxItem(index)->IsGroupHeader())
	{
		int top = index;
		index = FindNextNonHeaderItem(index);
		ChangeSelection(index, top);
		SetWindowText(GetComboBoxItem(index)->GetCaption());
		SetEditSel(0, -1);
		return true;
	}
	return false;
}

// Finds the index of item at the top of view when
// item provided is at the bottom
int CGroupComboBox::GetTopForItem(int index)
{
	index -= GetMinVisible();
	return (index < 0) ? 0 : index + 1;
}

// Finds the index of item at the bottom of view when
// item provided is at the top
int CGroupComboBox::GetBottomForItem(int index)
{
	index += GetMinVisible() - 1;
	return (index >= GetCount()) ? GetCount() - 1 : index;
}

// Finds the largest item width
int CGroupComboBox::GetMaxItemWidth()
{
	CClientDC dc(this);
	int width = 0;
	for (int i = 0; i < GetCount(); ++i)
	{
		CGroupComboBoxItem* item = GetComboBoxItem(i);
		int itemWidth = item->GetSize(&dc).cx;
		if (width < itemWidth)
			width = itemWidth;
	}
	return width;
}

// Corrects drop-down list width to fit all items and moves it
// if it goes out of the desktop
void CGroupComboBox::AdjustDropdownListWidth()
{
	COMBOBOXINFO cbi = { sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(&cbi);
	RECT rect;
	::GetWindowRect(cbi.hwndList, &rect);
	int maxItemWidth = GetMaxItemWidth() + ::GetSystemMetrics(SM_CXEDGE) * 2;
	// extend drop-down list to right
	if (maxItemWidth > rect.right - rect.left)
	{
		rect.right = rect.left + maxItemWidth;
		// reserve place for vertical scrollbar
		if (GetCount() > GetMinVisible())
			rect.right += ::GetSystemMetrics(SM_CXVSCROLL);
		// check if extended drop-down list fits the desktop
		HMONITOR monitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(monitor, &mi);
		// it doesn't fit, move it left
		if (mi.rcWork.right < rect.right)
		{
			int delta = rect.right - mi.rcWork.right;
			rect.left -= delta;
			rect.right -= delta;
		}
		::SetWindowPos(cbi.hwndList, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
	}
}

//////////////////////////////////////////////////////////////////////////
// CGroupComboBoxVariable implementation

IMPLEMENT_DYNAMIC(CGroupComboBoxVariable, CGroupComboBox)

CGroupComboBoxVariable::CGroupComboBoxVariable()
	: m_listRect(this)
	, m_itemsOnLastPage(30)
{
}

int CGroupComboBoxVariable::AddItem(CGroupComboBoxItem* item, int groupIndex)
{
	m_listRect.SetModified();
	return CGroupComboBox::AddItem(item, groupIndex);
}

int CGroupComboBoxVariable::InsertItem(CGroupComboBoxItem* item, int groupIndex)
{
	m_listRect.SetModified();
	return CGroupComboBox::InsertItem(item, groupIndex);
}

void CGroupComboBoxVariable::PreSubclassWindow()
{
	ModifyStyle(0, CBS_NOINTEGRALHEIGHT, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	CGroupComboBox::PreSubclassWindow();
}

void CGroupComboBoxVariable::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CGroupComboBoxItem* item = reinterpret_cast<CGroupComboBoxItem*>(lpMeasureItemStruct->itemData);
	ASSERT(item != NULL);
	lpMeasureItemStruct->itemHeight = item->GetSize(GetDC()).cy;
}

BEGIN_MESSAGE_MAP(CGroupComboBoxVariable, CGroupComboBox)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CGroupComboBoxVariable::OnCbnDropdown)
	ON_WM_DELETEITEM()
END_MESSAGE_MAP()

// CGroupComboBoxVariable message handlers

void CGroupComboBoxVariable::OnCbnDropdown()
{
	CGroupComboBox::OnCbnDropdown();
	ResizeDropDownList();
}

void CGroupComboBoxVariable::OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	m_listRect.SetModified();
	CGroupComboBox::OnDeleteItem(nIDCtl, lpDeleteItemStruct);
}

// Calculates drop-down list height so that last item is bound to the bottom
// when scrolled down (valid only if CBS_NOINTEGRALHEIGHT flag is set)
RECT CGroupComboBoxVariable::CalculateDropDownListRect()
{
	ASSERT((GetStyle() & CBS_NOINTEGRALHEIGHT) == CBS_NOINTEGRALHEIGHT); 
	// get workspace area
	RECT rect;
	GetWindowRect(&rect);
	HMONITOR monitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(monitor, &mi);

	// check if drop-down box fits below edit box
	int availableHeight = mi.rcWork.bottom - rect.bottom;
	bool showBelow = true;
	// sum the heights of last m_itemsOnLastPage items
	int listHeight = 0;
	// last item
	int nBottom = GetCount() - 1;
	// item that should be on the top of the last page
	int nTop = max(nBottom - m_itemsOnLastPage + 1, 0);
	while ((nBottom >= nTop) && (listHeight + GetItemHeight(nBottom) < availableHeight))
	{
		listHeight += GetItemHeight(nBottom);
		--nBottom;
	}
	// if cannot display requested number of items below and there is more space above
	// check how many items can be displayed when list is above
	if ((nBottom > nTop) && (availableHeight < rect.top - mi.rcWork.top))
	{
		availableHeight = rect.top - mi.rcWork.top;
		showBelow = false;
		while (nBottom >= nTop && (listHeight + GetItemHeight(nBottom) < availableHeight))
		{
			listHeight += GetItemHeight(nBottom);
			--nBottom;
		}
	}

	listHeight += ::GetSystemMetrics(SM_CYEDGE);
	if (showBelow)
	{
		rect.top = rect.bottom;
		rect.bottom += listHeight;
	}
	else
	{
		rect.bottom = rect.top;
		rect.top -= listHeight;
	}
	return rect;
}

int CGroupComboBoxVariable::GetTopForItem(int index)
{
	int listHeight = m_listRect.GetRect().Height();
	CClientDC dc(this);
	int height = 0;
	while (index >= 0)
	{
		int itemHeight = GetComboBoxItem(index)->GetSize(&dc).cy;
		height += itemHeight;
		if (height > listHeight)
			break;
		--index;
	}
	return index + 1;
}

int CGroupComboBoxVariable::GetBottomForItem(int index)
{
	int listHeight = m_listRect.GetRect().Height();
	CClientDC dc(this);
	int height = 0;
	while (index < GetCount())
	{
		int itemHeight = GetComboBoxItem(index)->GetSize(&dc).cy;
		height += itemHeight;
		if (height > listHeight)
			break;
		++index;
	}
	return index - 1;
}

void CGroupComboBoxVariable::ResizeDropDownList()
{
	CRect listRect = m_listRect.GetRect();
	COMBOBOXINFO cbi = { sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(&cbi);
	::MoveWindow(cbi.hwndList, listRect.left, listRect.top, listRect.Width(), listRect.Height(), FALSE);
}

const CRect& CGroupComboBoxVariable::CDropdownListRect::GetRect()
{
	if (m_modified)
	{
		m_rect = m_comboBox->CalculateDropDownListRect();
		m_modified = false;
	}
	return m_rect;
}
