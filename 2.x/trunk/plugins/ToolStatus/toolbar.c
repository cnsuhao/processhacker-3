/*
 * Process Hacker ToolStatus -
 *   main toolbar
 *
 * Copyright (C) 2011-2015 dmex
 *
 * This file is part of Process Hacker.
 *
 * Process Hacker is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Process Hacker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Process Hacker.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "toolstatus.h"

HIMAGELIST ToolBarImageList = NULL;

TBBUTTON ToolbarButtons[MAX_TOOLBAR_ITEMS] =
{
    // Default toolbar buttons (displayed)
    { 0, PHAPP_ID_VIEW_REFRESH, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, { 0 }, 0, (INT_PTR)L"Refresh" },
    { 1, PHAPP_ID_HACKER_OPTIONS, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, { 0 }, 0, (INT_PTR)L"Options" },
    { 0, 0, 0, BTNS_SEP, { 0 }, 0, 0 },
    { 2, PHAPP_ID_HACKER_FINDHANDLESORDLLS, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, { 0 }, 0, (INT_PTR)L"Find Handles or DLLs" },
    { 3, PHAPP_ID_VIEW_SYSTEMINFORMATION, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, { 0 }, 0, (INT_PTR)L"System Information" },
    { 0, 0, 0, BTNS_SEP, { 0 }, 0, 0 },
    { 4, TIDC_FINDWINDOW, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, { 0 }, 0, (INT_PTR)L"Find Window" },
    { 5, TIDC_FINDWINDOWTHREAD, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, { 0 }, 0, (INT_PTR)L"Find Window and Thread" },
    { 6, TIDC_FINDWINDOWKILL, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, { 0 }, 0, (INT_PTR)L"Find Window and Kill" },
    // Available toolbar buttons (hidden)
    { 7, PHAPP_ID_VIEW_ALWAYSONTOP, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, { 0 }, 0, (INT_PTR)L"Always on Top" },
    { 8, TIDC_POWERMENUDROPDOWN, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT,{ 0 }, 0, (INT_PTR)L"Computer" },
};

VOID RebarBandInsert(
    _In_ UINT BandID,
    _In_ HWND HwndChild,
    _In_ UINT cxMinChild,
    _In_ UINT cyMinChild
    )
{
    REBARBANDINFO rebarBandInfo = { REBARBANDINFO_V6_SIZE };
    rebarBandInfo.fMask = RBBIM_STYLE | RBBIM_ID | RBBIM_CHILD | RBBIM_CHILDSIZE;
    rebarBandInfo.fStyle =  RBBS_USECHEVRON; // RBBS_NOGRIPPER| RBBS_HIDETITLE | RBBS_TOPALIGN;

    rebarBandInfo.wID = BandID;
    rebarBandInfo.hwndChild = HwndChild;
    rebarBandInfo.cxMinChild = cxMinChild;
    rebarBandInfo.cyMinChild = cyMinChild;

    if (BandID == REBAR_BAND_ID_SEARCHBOX)
    {
        rebarBandInfo.fStyle |= RBBS_FIXEDSIZE;
    }

    if (ToolBarLocked)
    {
        rebarBandInfo.fStyle |= RBBS_NOGRIPPER;
    }

    SendMessage(RebarHandle, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rebarBandInfo);
}

VOID RebarBandRemove(
    _In_ UINT BandID
    )
{
    UINT index = (UINT)SendMessage(RebarHandle, RB_IDTOINDEX, (WPARAM)BandID, 0);

    if (index == -1)
        return;

    SendMessage(RebarHandle, RB_DELETEBAND, (WPARAM)index, 0);
}

BOOLEAN RebarBandExists(
    _In_ UINT BandID
    )
{
    UINT index = (UINT)SendMessage(RebarHandle, RB_IDTOINDEX, (WPARAM)BandID, 0);

    if (index != -1)
        return TRUE;

    return FALSE;
}

static VOID RebarLoadSettings(
    VOID
    )
{
    // Initialize the Toolbar Imagelist.
    if (EnableToolBar)
    {
        HBITMAP bitmapRefresh = NULL;
        HBITMAP bitmapSettings = NULL;
        HBITMAP bitmapFind = NULL;
        HBITMAP bitmapSysInfo = NULL;
        HBITMAP bitmapFindWindow = NULL;
        HBITMAP bitmapFindWindowThread = NULL;
        HBITMAP bitmapFindWindowKill = NULL;
        HBITMAP bitmapAlwaysOnTop = NULL;
        HBITMAP bitmapPowerMenu = NULL;

        // https://msdn.microsoft.com/en-us/library/ms701681.aspx
        INT cx = GetSystemMetrics(SM_CXSMICON); // SM_CXICON
        INT cy = GetSystemMetrics(SM_CYSMICON); // SM_CYICON

        if (!ToolBarImageList)
        {
            // Create the toolbar imagelist
            ToolBarImageList = ImageList_Create(cx, cy, ILC_COLOR32 | ILC_MASK, 0, 0);
            // Set the number of images
            ImageList_SetImageCount(ToolBarImageList, 9);
        }

        // Add the images to the imagelist
        if (PhGetIntegerSetting(SETTING_NAME_ENABLE_MODERNICONS))
        {
            bitmapRefresh = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_ARROW_REFRESH_MODERN));
            bitmapSettings = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_COG_EDIT_MODERN));
            bitmapFind = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_FIND_MODERN));
            bitmapSysInfo = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_CHART_LINE_MODERN));
            bitmapFindWindow = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_APPLICATION_MODERN));
            bitmapFindWindowThread = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_APPLICATION_GO_MODERN));
            bitmapFindWindowKill = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_CROSS_MODERN));
            bitmapAlwaysOnTop = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_APPLICATION_GET_MODERN));
            bitmapPowerMenu = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_POWER_MODERN));
        }
        else
        {
            bitmapRefresh = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_ARROW_REFRESH));
            bitmapSettings = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_COG_EDIT));
            bitmapFind = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_FIND));
            bitmapSysInfo = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_CHART_LINE));
            bitmapFindWindow = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_APPLICATION));
            bitmapFindWindowThread = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_APPLICATION_GO));
            bitmapFindWindowKill = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_CROSS));
            bitmapAlwaysOnTop = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_APPLICATION_GET));
            bitmapPowerMenu = LoadImageFromResources(cx, cy, MAKEINTRESOURCE(IDB_POWER));
        }

        if (bitmapRefresh)
        {
            ImageList_Replace(ToolBarImageList, 0, bitmapRefresh, NULL);
            DeleteObject(bitmapRefresh);
        }
        else
        {
            PhSetImageListBitmap(ToolBarImageList, 0, PluginInstance->DllBase, MAKEINTRESOURCE(IDB_ARROW_REFRESH_BMP));
        }

        if (bitmapSettings)
        {
            ImageList_Replace(ToolBarImageList, 1, bitmapSettings, NULL);
            DeleteObject(bitmapSettings);
        }
        else
        {
            PhSetImageListBitmap(ToolBarImageList, 1, PluginInstance->DllBase, MAKEINTRESOURCE(IDB_COG_EDIT_BMP));
        }

        if (bitmapFind)
        {
            ImageList_Replace(ToolBarImageList, 2, bitmapFind, NULL);
            DeleteObject(bitmapFind);
        }
        else
        {
            PhSetImageListBitmap(ToolBarImageList, 2, PluginInstance->DllBase, MAKEINTRESOURCE(IDB_FIND_BMP));
        }

        if (bitmapSysInfo)
        {
            ImageList_Replace(ToolBarImageList, 3, bitmapSysInfo, NULL);
            DeleteObject(bitmapSysInfo);
        }
        else
        {
            PhSetImageListBitmap(ToolBarImageList, 3, PluginInstance->DllBase, MAKEINTRESOURCE(IDB_CHART_LINE_BMP));
        }

        if (bitmapFindWindow)
        {
            ImageList_Replace(ToolBarImageList, 4, bitmapFindWindow, NULL);
            DeleteObject(bitmapFindWindow);
        }
        else
        {
            PhSetImageListBitmap(ToolBarImageList, 4, PluginInstance->DllBase, MAKEINTRESOURCE(IDB_APPLICATION_BMP));
        }

        if (bitmapFindWindowThread)
        {
            ImageList_Replace(ToolBarImageList, 5, bitmapFindWindowThread, NULL);
            DeleteObject(bitmapFindWindowThread);
        }
        else
        {
            PhSetImageListBitmap(ToolBarImageList, 5, PluginInstance->DllBase, MAKEINTRESOURCE(IDB_APPLICATION_GO_BMP));
        }

        if (bitmapFindWindowKill)
        {
            ImageList_Replace(ToolBarImageList, 6, bitmapFindWindowKill, NULL);
            DeleteObject(bitmapFindWindowKill);
        }
        else
        {
            PhSetImageListBitmap(ToolBarImageList, 6, PluginInstance->DllBase, MAKEINTRESOURCE(IDB_CROSS_BMP));
        }

        if (bitmapAlwaysOnTop)
        {
            ImageList_Replace(ToolBarImageList, 7, bitmapAlwaysOnTop, NULL);
            DeleteObject(bitmapAlwaysOnTop);
        }
        else
        {
            PhSetImageListBitmap(ToolBarImageList, 7, PluginInstance->DllBase, MAKEINTRESOURCE(IDB_APPLICATION_GET_BMP));
        }

        if (bitmapPowerMenu)
        {
            ImageList_Replace(ToolBarImageList, 8, bitmapPowerMenu, NULL);
            DeleteObject(bitmapPowerMenu);
        }
        else
        {
            PhSetImageListBitmap(ToolBarImageList, 8, PluginInstance->DllBase, MAKEINTRESOURCE(IDB_POWER_BMP));
        }
    }

    // Initialize the Rebar and Toolbar controls.
    if (EnableToolBar && !RebarHandle)
    {
        REBARINFO rebarInfo = { sizeof(REBARINFO) };
        ULONG toolbarButtonSize;

        // Create the ReBar window.
        RebarHandle = CreateWindowEx(
            WS_EX_TOOLWINDOW,
            REBARCLASSNAME,
            NULL,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CCS_NODIVIDER | CCS_TOP | RBS_VARHEIGHT | RBS_AUTOSIZE, // CCS_NOPARENTALIGN | RBS_FIXEDORDER
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            PhMainWndHandle,
            NULL,
            NULL,
            NULL
            );

        // Set the toolbar info with no imagelist.
        SendMessage(RebarHandle, RB_SETBARINFO, 0, (LPARAM)&rebarInfo);

        // Create the ToolBar window.
        ToolBarHandle = CreateWindowEx(
            0,
            TOOLBARCLASSNAME,
            NULL,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CCS_NORESIZE | CCS_NOPARENTALIGN | CCS_NODIVIDER | TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS | TBSTYLE_AUTOSIZE, // TBSTYLE_CUSTOMERASE  TBSTYLE_ALTDRAG
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            RebarHandle,
            NULL,
            NULL,
            NULL
            );

        // Set the toolbar struct size.
        SendMessage(ToolBarHandle, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
        // Set the toolbar extended toolbar styles.
        SendMessage(ToolBarHandle, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DOUBLEBUFFER | TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_HIDECLIPPEDBUTTONS);
        // Configure the toolbar imagelist.
        SendMessage(ToolBarHandle, TB_SETIMAGELIST, 0, (LPARAM)ToolBarImageList);
        // Add the buttons to the toolbar (also specifying the default number of items to display).
        SendMessage(ToolBarHandle, TB_ADDBUTTONS, MAX_DEFAULT_TOOLBAR_ITEMS, (LPARAM)ToolbarButtons);
        // Restore the toolbar settings.
        ToolbarLoadButtonSettings();
        // Query the toolbar width and height.
        //SendMessage(ToolBarHandle, TB_GETMAXSIZE, 0, (LPARAM)&toolbarSize);
        toolbarButtonSize = (ULONG)SendMessage(ToolBarHandle, TB_GETBUTTONSIZE, 0, 0);

        // Enable theming
        switch (ToolBarTheme)
        {
        case TOOLBAR_THEME_BLACK:
            {
                SendMessage(RebarHandle, RB_SETWINDOWTHEME, 0, (LPARAM)L"Media"); //Media/Communications/BrowserTabBar/Help
                SendMessage(ToolBarHandle, TB_SETWINDOWTHEME, 0, (LPARAM)L"Media"); //Media/Communications/BrowserTabBar/Help
            }
            break;
        case TOOLBAR_THEME_BLUE:
            {
                SendMessage(RebarHandle, RB_SETWINDOWTHEME, 0, (LPARAM)L"Communications");
                SendMessage(ToolBarHandle, TB_SETWINDOWTHEME, 0, (LPARAM)L"Communications");
            }
            break;
        }

        // Inset the toolbar into the rebar control.
        RebarBandInsert(REBAR_BAND_ID_TOOLBAR, ToolBarHandle, LOWORD(toolbarButtonSize), HIWORD(toolbarButtonSize));
    }

    // Initialize the Searchbox and TreeNewFilters.
    if (EnableSearchBox && !SearchboxHandle)
    {
        SearchboxText = PhReferenceEmptyString();

        ProcessTreeFilterEntry = PhAddTreeNewFilter(PhGetFilterSupportProcessTreeList(), (PPH_TN_FILTER_FUNCTION)ProcessTreeFilterCallback, NULL);
        ServiceTreeFilterEntry = PhAddTreeNewFilter(PhGetFilterSupportServiceTreeList(), (PPH_TN_FILTER_FUNCTION)ServiceTreeFilterCallback, NULL);
        NetworkTreeFilterEntry = PhAddTreeNewFilter(PhGetFilterSupportNetworkTreeList(), (PPH_TN_FILTER_FUNCTION)NetworkTreeFilterCallback, NULL);

        // Create the Searchbox control.
        SearchboxHandle = CreateSearchControl(ID_SEARCH_CLEAR);
    }

    // Initialize the Statusbar control.
    if (EnableStatusBar && !StatusBarHandle)
    {
        // Create the StatusBar window.
        StatusBarHandle = CreateWindowEx(
            0,
            STATUSCLASSNAME,
            NULL,
            WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            PhMainWndHandle,
            NULL,
            NULL,
            NULL
            );
    }

    // Hide or show controls (Note: don't unload or remove at runtime).
    if (EnableToolBar)
    {
        if (RebarHandle && !IsWindowVisible(RebarHandle))
            ShowWindow(RebarHandle, SW_SHOW);
    }
    else
    {
        if (RebarHandle && IsWindowVisible(RebarHandle))
            ShowWindow(RebarHandle, SW_HIDE);
    }

    if (EnableSearchBox && RebarHandle)
    {
        UINT height = (UINT)SendMessage(RebarHandle, RB_GETROWHEIGHT, 0, 0);

        // Add the Searchbox band into the rebar control.
        if (!RebarBandExists(REBAR_BAND_ID_SEARCHBOX))
            RebarBandInsert(REBAR_BAND_ID_SEARCHBOX, SearchboxHandle, 180, height - 2);

        if (SearchboxHandle && !IsWindowVisible(SearchboxHandle))
            ShowWindow(SearchboxHandle, SW_SHOW);
    }
    else
    {
        // Remove the Searchbox band from the rebar control.
        if (RebarBandExists(REBAR_BAND_ID_SEARCHBOX))
            RebarBandRemove(REBAR_BAND_ID_SEARCHBOX);

        if (SearchboxHandle)
        {
            // Clear search text and reset search filters.
            SetFocus(SearchboxHandle);
            Static_SetText(SearchboxHandle, L"");

            if (IsWindowVisible(SearchboxHandle))
                ShowWindow(SearchboxHandle, SW_HIDE);
        }
    }

    if (EnableSearchBox)
    {
        // TODO: Is there a better way of handling this in the above code?
        if (SearchBoxDisplayMode == SearchBoxDisplayHideInactive)
        {
            if (RebarBandExists(REBAR_BAND_ID_SEARCHBOX))
                RebarBandRemove(REBAR_BAND_ID_SEARCHBOX);
        }
        else
        {
            UINT height = (UINT)SendMessage(RebarHandle, RB_GETROWHEIGHT, 0, 0);

            if (!RebarBandExists(REBAR_BAND_ID_SEARCHBOX))
                RebarBandInsert(REBAR_BAND_ID_SEARCHBOX, SearchboxHandle, 180, height - 2);
        }
    }

    if (EnableStatusBar)
    {
        if (StatusBarHandle && !IsWindowVisible(StatusBarHandle))
            ShowWindow(StatusBarHandle, SW_SHOW);
    }
    else
    {
        if (StatusBarHandle && IsWindowVisible(StatusBarHandle))
            ShowWindow(StatusBarHandle, SW_HIDE);
    }

    ToolbarCreateGraphs();
}

VOID ToolbarLoadSettings(
    VOID
    )
{
    RebarLoadSettings();

    if (EnableToolBar && ToolBarHandle)
    {
        INT index = 0;
        INT buttonCount = 0;

        buttonCount = (INT)SendMessage(ToolBarHandle, TB_BUTTONCOUNT, 0, 0);

        for (index = 0; index < buttonCount; index++)
        {
            TBBUTTONINFO button = { sizeof(TBBUTTONINFO) };
            button.dwMask = TBIF_BYINDEX | TBIF_STYLE | TBIF_COMMAND | TBIF_STATE;

            // Get settings for first button
            if (SendMessage(ToolBarHandle, TB_GETBUTTONINFO, index, (LPARAM)&button) == -1)
                break;

            // Skip separator buttons
            if (button.fsStyle == BTNS_SEP)
                continue;

            // Invalidate the button rect when adding/removing the BTNS_SHOWTEXT style.
            button.dwMask |= TBIF_TEXT;
            button.pszText = ToolbarGetText(button.idCommand);

            switch (DisplayStyle)
            {
            case ToolbarDisplayImageOnly:
                button.fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
                break;
            case ToolbarDisplaySelectiveText:
                {
                    switch (button.idCommand)
                    {
                    case PHAPP_ID_VIEW_REFRESH:
                    case PHAPP_ID_HACKER_OPTIONS:
                    case PHAPP_ID_HACKER_FINDHANDLESORDLLS:
                    case PHAPP_ID_VIEW_SYSTEMINFORMATION:
                        button.fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT;
                        break;
                    default:
                        button.fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
                        break;
                    }
                }
                break;
            case ToolbarDisplayAllText:
                button.fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT;
                break;
            }

            switch (button.idCommand)
            {
            case PHAPP_ID_VIEW_ALWAYSONTOP:
                {
                    // Set the pressed state
                    if (PhGetIntegerSetting(L"MainWindowAlwaysOnTop"))
                    {
                        button.fsState |= TBSTATE_PRESSED;
                    }
                }
                break;
            case TIDC_POWERMENUDROPDOWN:
                {
                    button.fsStyle |= BTNS_WHOLEDROPDOWN;
                }
                break;
            }

            // Set updated button info
            SendMessage(ToolBarHandle, TB_SETBUTTONINFO, index, (LPARAM)&button);
        }

        // Resize the toolbar
        SendMessage(ToolBarHandle, TB_AUTOSIZE, 0, 0);
    }

    if (EnableToolBar && RebarHandle && ToolBarHandle)
    {
        UINT index;
        REBARBANDINFO rebarBandInfo = { REBARBANDINFO_V6_SIZE };
        rebarBandInfo.fMask = RBBIM_IDEALSIZE;

        index = (UINT)SendMessage(RebarHandle, RB_IDTOINDEX, (WPARAM)REBAR_BAND_ID_TOOLBAR, 0);

        // Get settings for Rebar band.
        if (SendMessage(RebarHandle, RB_GETBANDINFO, index, (LPARAM)&rebarBandInfo) != -1)
        {
            SIZE idealWidth;

            // Reset the cxIdeal for the Chevron
            SendMessage(ToolBarHandle, TB_GETIDEALSIZE, FALSE, (LPARAM)&idealWidth);

            rebarBandInfo.cxIdeal = idealWidth.cx;

            SendMessage(RebarHandle, RB_SETBANDINFO, index, (LPARAM)&rebarBandInfo);
        }
    }

    // Invoke the LayoutPaddingCallback.
    SendMessage(PhMainWndHandle, WM_SIZE, 0, 0);
}

VOID ToolbarResetSettings(
    VOID
    )
{
    // Remove all the user customizations.
    INT buttonCount = (INT)SendMessage(ToolBarHandle, TB_BUTTONCOUNT, 0, 0);
    while (buttonCount--)
        SendMessage(ToolBarHandle, TB_DELETEBUTTON, (WPARAM)buttonCount, 0);

    // Re-add the original buttons.
    SendMessage(ToolBarHandle, TB_ADDBUTTONS, MAX_DEFAULT_TOOLBAR_ITEMS, (LPARAM)ToolbarButtons);

}

PWSTR ToolbarGetText(
    _In_ INT CommandID
    )
{
    switch (CommandID)
    {
    case PHAPP_ID_VIEW_REFRESH:
        return L"Refresh";
    case PHAPP_ID_HACKER_OPTIONS:
        return L"Options";
    case PHAPP_ID_HACKER_FINDHANDLESORDLLS:
        return L"Find Handles or DLLs";
    case PHAPP_ID_VIEW_SYSTEMINFORMATION:
        return L"System Information";
    case TIDC_FINDWINDOW:
        return L"Find Window";
    case TIDC_FINDWINDOWTHREAD:
        return L"Find Window and Thread";
    case TIDC_FINDWINDOWKILL:
        return L"Find Window and Kill";
    case PHAPP_ID_VIEW_ALWAYSONTOP:
        return L"Always on Top";
    case TIDC_POWERMENUDROPDOWN:
        return L"Computer";
    }

    return L"ERROR";
}

VOID ToolbarLoadButtonSettings(
    VOID
    )
{
    INT buttonCount = 0;
    PPH_STRING settingsString;
    PTBBUTTON buttonArray;
    PH_STRINGREF remaining;
    PH_STRINGREF part;

    settingsString = PhGetStringSetting(SETTING_NAME_TOOLBAR_CONFIG);
    remaining = settingsString->sr;

    if (remaining.Length == 0)
        return;

    // Remove all current buttons.
    buttonCount = (INT)SendMessage(ToolBarHandle, TB_BUTTONCOUNT, 0, 0);
    while (buttonCount--)
        SendMessage(ToolBarHandle, TB_DELETEBUTTON, (WPARAM)buttonCount, 0);

    // Query the number of buttons to insert
    PhSplitStringRefAtChar(&remaining, '|', &part, &remaining);
    buttonCount = _wtoi(part.Buffer);

    // Allocate the button array
    buttonArray = PhAllocate(buttonCount * sizeof(TBBUTTON));
    memset(buttonArray, 0, buttonCount * sizeof(TBBUTTON));

    for (INT index = 0; index < buttonCount; index++)
    {
        PH_STRINGREF commandIdPart;
        PH_STRINGREF iBitmapPart;
        PH_STRINGREF buttonSepPart;
        PH_STRINGREF buttonAutoSizePart;
        PH_STRINGREF buttonShowTextPart;
        PH_STRINGREF buttonDropDownPart;

        if (remaining.Length == 0)
            break;

        PhSplitStringRefAtChar(&remaining, '|', &commandIdPart, &remaining);
        PhSplitStringRefAtChar(&remaining, '|', &iBitmapPart, &remaining);
        PhSplitStringRefAtChar(&remaining, '|', &buttonSepPart, &remaining);
        PhSplitStringRefAtChar(&remaining, '|', &buttonAutoSizePart, &remaining);
        PhSplitStringRefAtChar(&remaining, '|', &buttonShowTextPart, &remaining);
        PhSplitStringRefAtChar(&remaining, '|', &buttonDropDownPart, &remaining);

        buttonArray[index].idCommand = _wtoi(commandIdPart.Buffer);
        buttonArray[index].iBitmap = _wtoi(iBitmapPart.Buffer);

        if (_wtoi(buttonSepPart.Buffer))
            buttonArray[index].fsStyle |= BTNS_SEP;

        if (_wtoi(buttonAutoSizePart.Buffer))
            buttonArray[index].fsStyle |= BTNS_AUTOSIZE;
        
        if (_wtoi(buttonShowTextPart.Buffer))
            buttonArray[index].fsStyle |= BTNS_SHOWTEXT;
        
        if (_wtoi(buttonDropDownPart.Buffer))
            buttonArray[index].fsStyle |= BTNS_WHOLEDROPDOWN;

        buttonArray[index].fsState |= TBSTATE_ENABLED;
    }

    SendMessage(ToolBarHandle, TB_ADDBUTTONS, buttonCount, (LPARAM)buttonArray);

    PhFree(buttonArray);
    PhDereferenceObject(settingsString);
}

VOID ToolbarSaveButtonSettings(
    VOID
    )
{
    INT buttonIndex = 0;
    INT buttonCount = 0;
    PPH_STRING settingsString;
    PH_STRING_BUILDER stringBuilder;

    PhInitializeStringBuilder(&stringBuilder, 100);

    buttonCount = (INT)SendMessage(ToolBarHandle, TB_BUTTONCOUNT, 0, 0);

    PhAppendFormatStringBuilder(
        &stringBuilder,
        L"%d|",
        buttonCount
        );

    for (buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++)
    {
        TBBUTTONINFO button = { sizeof(TBBUTTONINFO) };
        button.dwMask = TBIF_BYINDEX | TBIF_IMAGE | TBIF_STATE | TBIF_STYLE | TBIF_COMMAND;

        // Get button information.
        if (SendMessage(ToolBarHandle, TB_GETBUTTONINFO, buttonIndex, (LPARAM)&button) == -1)
            break;

        PhAppendFormatStringBuilder(
            &stringBuilder,
            L"%d|%d|%d|%d|%d|%d|",
            button.idCommand,
            button.iImage,
            button.fsStyle & BTNS_SEP ? TRUE : FALSE,
            button.fsStyle & BTNS_AUTOSIZE ? TRUE : FALSE,
            button.fsStyle & BTNS_SHOWTEXT ? TRUE : FALSE,
            button.fsStyle & BTNS_WHOLEDROPDOWN ? TRUE : FALSE
            );
    }

    if (stringBuilder.String->Length != 0)
        PhRemoveEndStringBuilder(&stringBuilder, 1);

    settingsString = PhFinalStringBuilderString(&stringBuilder);
    PhSetStringSetting2(SETTING_NAME_TOOLBAR_CONFIG, &settingsString->sr);
    PhDereferenceObject(settingsString);
}

VOID ReBarLoadLayoutSettings(
    VOID
    )
{
    UINT bandIndex = 0;
    UINT bandCount = 0;
    PPH_STRING settingsString;
    PH_STRINGREF remaining;

    settingsString = PhGetStringSetting(SETTING_NAME_REBAR_CONFIG);
    remaining = settingsString->sr;

    if (remaining.Length == 0)
        return;

    bandCount = (UINT)SendMessage(RebarHandle, RB_GETBANDCOUNT, 0, 0);

    for (bandIndex = 0; bandIndex < bandCount; bandIndex++)
    {
        PH_STRINGREF idPart;
        PH_STRINGREF cxPart;
        PH_STRINGREF stylePart;
        ULONG64 idInteger;
        ULONG64 cxInteger;
        ULONG64 fStyleInteger;
        UINT bandOldIndex;

        if (remaining.Length == 0)
            break;

        PhSplitStringRefAtChar(&remaining, '|', &idPart, &remaining);
        PhSplitStringRefAtChar(&remaining, '|', &cxPart, &remaining);
        PhSplitStringRefAtChar(&remaining, '|', &stylePart, &remaining);

        PhStringToInteger64(&idPart, 10, &idInteger);
        PhStringToInteger64(&cxPart, 10, &cxInteger);
        PhStringToInteger64(&stylePart, 10, &fStyleInteger);

        if ((bandOldIndex = (UINT)SendMessage(RebarHandle, RB_IDTOINDEX, (UINT)idInteger, 0)) == -1)
            break;

        if (SendMessage(RebarHandle, RB_MOVEBAND, bandOldIndex, bandIndex))
        {
            REBARBANDINFO rebarBandInfo = { REBARBANDINFO_V6_SIZE };
            rebarBandInfo.fMask = RBBIM_STYLE | RBBIM_SIZE;

            if (SendMessage(RebarHandle, RB_GETBANDINFO, bandIndex, (LPARAM)&rebarBandInfo))
            {
                rebarBandInfo.cx = (UINT)cxInteger;
                rebarBandInfo.fStyle |= (UINT)fStyleInteger;

                SendMessage(RebarHandle, RB_SETBANDINFO, bandIndex, (LPARAM)&rebarBandInfo);
            }
        }
    }
}

VOID ReBarSaveLayoutSettings(
    VOID
    )
{
    UINT bandIndex = 0;
    UINT bandCount = 0;
    PPH_STRING settingsString;
    PH_STRING_BUILDER stringBuilder;

    PhInitializeStringBuilder(&stringBuilder, 100);

    bandCount = (UINT)SendMessage(RebarHandle, RB_GETBANDCOUNT, 0, 0);

    for (bandIndex = 0; bandIndex < bandCount; bandIndex++)
    {
        REBARBANDINFO rebarBandInfo = { REBARBANDINFO_V6_SIZE };
        rebarBandInfo.fMask = RBBIM_STYLE | RBBIM_SIZE | RBBIM_ID;

        SendMessage(RebarHandle, RB_GETBANDINFO, bandIndex, (LPARAM)&rebarBandInfo);

        if (rebarBandInfo.fStyle & RBBS_GRIPPERALWAYS)
        {
            rebarBandInfo.fStyle &= ~RBBS_GRIPPERALWAYS;
        }

        if (rebarBandInfo.fStyle & RBBS_NOGRIPPER)
        {
            rebarBandInfo.fStyle &= ~RBBS_NOGRIPPER;
        }

        if (rebarBandInfo.fStyle & RBBS_FIXEDSIZE)
        {
            rebarBandInfo.fStyle &= ~RBBS_FIXEDSIZE;
        }

        PhAppendFormatStringBuilder(
            &stringBuilder,
            L"%u|%u|%u|",
            rebarBandInfo.wID,
            rebarBandInfo.cx,
            rebarBandInfo.fStyle
            );
    }

    if (stringBuilder.String->Length != 0)
        PhRemoveEndStringBuilder(&stringBuilder, 1);

    settingsString = PhFinalStringBuilderString(&stringBuilder);
    PhSetStringSetting2(SETTING_NAME_REBAR_CONFIG, &settingsString->sr);
    PhDereferenceObject(settingsString);
}