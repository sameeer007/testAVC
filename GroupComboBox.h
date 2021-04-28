/* 
 *  Filename:    GroupComboBox.h
 *  Description: Group Combo Box class definitions
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

#pragma once

#include <vector>

// CGroupComboBoxItem abstract class
// Base class for all item classes
class CGroupComboBoxItem
{
	friend class CGroupComboBoxHeader;

protected:
	CGroupComboBoxItem(LPCTSTR caption, bool isHeader = false)
		: m_caption(caption)
		, m_isHeader(isHeader)
		, m_groupHeader(NULL)
	{ }

public:
	virtual ~CGroupComboBoxItem() { }

	CString GetCaption() const
	{
		return m_caption;
	}

	bool IsGroupHeader() const
	{
		return m_isHeader;
	}

	CGroupComboBoxHeader* GetGroupHeader() const
	{
		return m_groupHeader;
	}

	// compare captions ignoring case, using user default locale 
	int CompareCaption(LPCTSTR lpStringOther, int cchCount = -1) const
	{
		int result = ::CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, m_caption, cchCount, lpStringOther, cchCount);
		ASSERT(result != 0);
		return result - CSTR_EQUAL;
	}

	virtual void Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct) = 0;
	virtual CSize GetSize(CDC* dc) = 0;

private:
	CString m_caption;
	bool m_isHeader;
	// header to which this item belongs
	CGroupComboBoxHeader* m_groupHeader;
};

// Item comparer class used for sorting
// Default implementation compares items lexically, ignoring case
class CComboBoxItemCompare
{
public:
	virtual int Compare(const CGroupComboBoxItem* item1, const CGroupComboBoxItem* item2) const
	{
		return item1->CompareCaption(item2->GetCaption());
	}
};

// CGroupComboBoxHeader
// Item representing group header
class CGroupComboBoxHeader : public CGroupComboBoxItem
{
public:
	// create group header with (optionally) alphabetically sorted items
	CGroupComboBoxHeader(LPCTSTR caption, bool sorted = false);
	// create group header, passing comparison class for custom sorting of items
	template<typename TCompare> CGroupComboBoxHeader(LPCTSTR caption, const TCompare& compare);
	// destructor
	~CGroupComboBoxHeader();

	// assign an item to the group
	void AssignItem(CGroupComboBoxItem* item)
	{
		item->m_groupHeader = this;
	}

	bool IsSorted() const
	{
		return m_isSorted;
	}

	// Items comparison method called when sorting items
	int ItemCompare(const CGroupComboBoxItem* item1, const CGroupComboBoxItem* item2) const
	{
		return m_itemCompare->Compare(item1, item2);
	}

	// virtual methods called by owner-drawn combo-box
	virtual void Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual CSize GetSize(CDC* dc);

private:
	// flags indicating if items added should be sorted
	bool m_isSorted;
	// object with item comparison method
	CComboBoxItemCompare* m_itemCompare;
};

// CGroupComboBoxSimpleItem definition
// Simple item with text only
class CGroupComboBoxSimpleItem : public CGroupComboBoxItem
{
public:
	CGroupComboBoxSimpleItem(LPCTSTR caption);

	virtual void Draw(CDC* dc, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual CSize GetSize(CDC* dc);
};

// CGroupComboBox definition
// Combo-box with items separated by headers in different groups

class CGroupComboBox : public CComboBox
{
public:
	// CGroupListBox definition
	// Subclassed listbox
	class CGroupListBox : public CListBox
	{
		DECLARE_DYNAMIC(CGroupListBox)

	public:
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	protected:
		DECLARE_MESSAGE_MAP()
	};

private:
	DECLARE_DYNAMIC(CGroupComboBox)

public:
	CGroupComboBox();
	virtual ~CGroupComboBox();

	// item operations
	int AddGroup(CGroupComboBoxHeader* groupHeader);
	int AddItem(CGroupComboBoxItem* item, int groupIndex);
	int InsertItem(CGroupComboBoxItem* item, int groupIndex);
	void DeleteItem(int index);

	int FindString(int nStartAfter, LPCTSTR lpszString) const;
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const;
	int FindStringExactInGroup(LPCTSTR lpszString, int groupIndex) const;

	// Enables/disables automatic drop-down list adjustment
	void EnableDropdownListAutoWidth(bool autoWidth = true)
	{
		m_dropdownListAutoWidth = autoWidth;
	}
	
	// Enables/disables autocomplete option
	void EnableAutocomplete(bool autocomplete = true)
	{
		m_autocomplete = autocomplete;
	}
	
	// Enables/disables selection undo by Esc key
	void EnableSelectionUndoByEscKey(bool undoByEscKey = true)
	{
		m_selectionUndoByEscKey = undoByEscKey;
	}

	// disabled CComboBox public members 
	int AddString(LPCTSTR lpszString) { ASSERT(FALSE); }
	int InsertString(int nIndex, LPCTSTR lpszString) { ASSERT(FALSE); }
	int DeleteString(UINT nIndex) { ASSERT(FALSE); }

protected:
	CGroupComboBoxItem* GetComboBoxItem(int i) const 
	{
		return reinterpret_cast<CGroupComboBoxItem*>(CComboBox::GetItemData(i));
	}

	struct CGroupBounds
	{
		CGroupBounds(int firstIndex, int groupEnd) : FirstIndex(firstIndex), GroupEnd(groupEnd) {}
		int FirstIndex; // index of the first item in the group
		int GroupEnd;   // index of the item after last one in the group
	};

	// gets bounds (index of first and of item after last) for provided header
	CGroupBounds GetGroupBounds(const CGroupComboBoxHeader* header) const;

	virtual int GetTopForItem(int index);
	virtual int GetBottomForItem(int index);

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PreSubclassWindow();

	afx_msg void OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct);
	afx_msg BOOL OnCbnSelchange();
	afx_msg BOOL OnCbnDropdown();
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

private:
	CGroupListBox m_listBox; // subclassed listbox

	typedef std::vector<CGroupComboBoxHeader*> HeadersList;
	HeadersList m_headers;

	CString m_previousString;
	bool m_dropdownListAutoWidth;
	bool m_autocomplete;
	bool m_selectionUndoByEscKey;

	int GetInsertionIndex(const CGroupComboBoxHeader* header, int groupFirst, int groupEnd, const CGroupComboBoxItem* newItem) const;
	int FindItem(const CGroupComboBoxItem* item) const;
	int FindNextHeader(int start) const;
	int FindNextNonHeaderItem(int index) const;
	int FindPreviousNonHeaderItem(int index) const;
	void ShowGroupHeader(const CGroupComboBoxHeader* header);
	int FindGroupHeaderInsertPosition(const CGroupComboBoxHeader* header) const;

	void ChangeSelection(int newSelection, int top);
	void SelectItem(int index);
	void SelectAndCopyItem(int index);
	bool FindAndAutocompleteItem(const CString& text);
	bool FindStringAndSelect();

	bool PreprocessVkDown();
	bool PreprocessVkUp();
	bool PreprocessVkPageDown();
	bool PreprocessVkPageUp();

	int GetMaxItemWidth();
	void AdjustDropdownListWidth();
};


// CGroupComboBoxVariable definition
// Combo-box with items separated by headers in different groups
// and variable item height

class CGroupComboBoxVariable : public CGroupComboBox
{
	DECLARE_DYNAMIC(CGroupComboBoxVariable)

public:
	CGroupComboBoxVariable();

	int AddItem(CGroupComboBoxItem* item, int groupIndex);
	int InsertItem(CGroupComboBoxItem* item, int groupIndex);

	// redefinition of SetMinVisibleItems which actually sets the
	// number of items displayed on the last page
	void SetMinVisibleItems(int items)
	{
		m_itemsOnLastPage = items;
		m_listRect.SetModified();
	}

	virtual void PreSubclassWindow();
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCbnDropdown();
	afx_msg void OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct);

protected:
	virtual int GetTopForItem(int index);
	virtual int GetBottomForItem(int index);

private:
	int m_itemsOnLastPage;

	RECT CalculateDropDownListRect();
	void ResizeDropDownList();
	CRect GetDropdownListRect();

	class CDropdownListRect
	{
	public:
		CDropdownListRect(CGroupComboBoxVariable* comboBox)
			: m_comboBox(comboBox), m_rect(0, 0, 0, 0), m_modified(false)
		{
		}
		
		void SetModified()
		{
			m_modified = true;
		}
		
		const CRect& GetRect();
	
	private:
		CGroupComboBoxVariable* m_comboBox;
		CRect m_rect;
		bool m_modified;
	} m_listRect;
};
