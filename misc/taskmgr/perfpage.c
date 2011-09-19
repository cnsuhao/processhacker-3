/*
*  ReactOS Task Manager
*
*  perfpage.c
*
*  Copyright (C) 1999 - 2001  Brian Palmer  <brianp@reactos.org>
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License along
*  with this program; if not, write to the Free Software Foundation, Inc.,
*  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "taskmgr.h"

TGraphCtrl PerformancePageCpuUsageHistoryGraph;
TGraphCtrl PerformancePageMemUsageHistoryGraph;

HWND  hPerformancePage;                               /*  Performance Property Page */
HWND  hPerformancePageCpuUsageGraph;                  /*  CPU Usage Graph */
HWND  hPerformancePageMemUsageGraph;                  /*  MEM Usage Graph */
HWND  hPerformancePageCpuUsageHistoryGraph;           /*  CPU Usage History Graph */
HWND  hPerformancePageMemUsageHistoryGraph;           /*  Memory Usage History Graph */
HWND  hPerformancePageTotalsFrame;                    /*  Totals Frame */
HWND  hPerformancePageCommitChargeFrame;              /*  Commit Charge Frame */
HWND  hPerformancePageKernelMemoryFrame;              /*  Kernel Memory Frame */
HWND  hPerformancePagePhysicalMemoryFrame;            /*  Physical Memory Frame */
HWND  hPerformancePageCpuUsageFrame;
HWND  hPerformancePageMemUsageFrame;
HWND  hPerformancePageCpuUsageHistoryFrame;
HWND  hPerformancePageMemUsageHistoryFrame;
HWND  hPerformancePageCommitChargeTotalEdit;          /*  Commit Charge Total Edit Control */
HWND  hPerformancePageCommitChargeLimitEdit;          /*  Commit Charge Limit Edit Control */
HWND  hPerformancePageCommitChargePeakEdit;           /*  Commit Charge Peak Edit Control */
HWND  hPerformancePageKernelMemoryTotalEdit;          /*  Kernel Memory Total Edit Control */
HWND  hPerformancePageKernelMemoryPagedEdit;          /*  Kernel Memory Paged Edit Control */
HWND  hPerformancePageKernelMemoryNonPagedEdit;       /*  Kernel Memory NonPaged Edit Control */
HWND  hPerformancePagePhysicalMemoryTotalEdit;        /*  Physical Memory Total Edit Control */
HWND  hPerformancePagePhysicalMemoryAvailableEdit;    /*  Physical Memory Available Edit Control */
HWND  hPerformancePagePhysicalMemorySystemCacheEdit;  /*  Physical Memory System Cache Edit Control */
HWND  hPerformancePageTotalsHandleCountEdit;          /*  Total Handles Edit Control */
HWND  hPerformancePageTotalsProcessCountEdit;         /*  Total Processes Edit Control */
HWND  hPerformancePageTotalsThreadCountEdit;          /*  Total Threads Edit Control */

static uintptr_t hPerformanceThread = 0;
static UINT dwPerformanceThread = 0;

WNDPROC OldGraphWndProc;

static int nPerformancePageWidth = 0;
static int nPerformancePageHeight = 0;
static int lastX = 0, lastY = 0;
UINT WINAPI PerformancePageRefreshThread(void *lpParameter);

void AdjustFrameSize(HWND hCntrl, HWND hDlg, int nXDifference, int nYDifference, int pos)
{
    RECT  rc;
    int   cx, cy, sx, sy;

    GetClientRect(hCntrl, &rc);
    MapWindowPoints(hCntrl, hDlg, (LPPOINT)(PRECT)(&rc), sizeof(RECT) / sizeof(POINT));

    if (pos) 
    {
        cx = rc.left;
        cy = rc.top;
        sx = rc.right - rc.left;

        switch (pos) 
        {
        case 1:
            break;
        case 2:
            cy += nYDifference / 2;
            break;
        case 3:
            sx += nXDifference;
            break;
        case 4:
            cy += nYDifference / 2;
            sx += nXDifference;
            break;
        }

        sy = rc.bottom - rc.top + nYDifference / 2;
        SetWindowPos(hCntrl, NULL, cx, cy, sx, sy, SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOZORDER);
    } 
    else 
    {
        cx = rc.left + nXDifference;
        cy = rc.top + nYDifference;
        sx = sy = 0;
        SetWindowPos(hCntrl, NULL, cx, cy, 0, 0, SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOSIZE|SWP_NOZORDER);
    }

    InvalidateRect(hCntrl, NULL, TRUE);
}

static void AdjustControlPostion(HWND hCntrl, HWND hDlg, int nXDifference, int nYDifference)
{
    AdjustFrameSize(hCntrl, hDlg, nXDifference, nYDifference, 0);
}

static void AdjustCntrlPos(int ctrl_id, HWND hDlg, int nXDifference, int nYDifference)
{
    AdjustFrameSize(GetDlgItem(hDlg, ctrl_id), hDlg, nXDifference, nYDifference, 0);
}

INT_PTR CALLBACK PerformancePageWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rc = { 0 };
    int nXDifference = 0, nYDifference = 0;
    
    switch (message) 
    {
    case WM_DESTROY:
        GraphCtrl_Dispose(&PerformancePageCpuUsageHistoryGraph);
        GraphCtrl_Dispose(&PerformancePageMemUsageHistoryGraph);
        break;

    case WM_INITDIALOG:
        // Save the width and height.
        GetClientRect(hDlg, &rc);
        nPerformancePageWidth = rc.right;
        nPerformancePageHeight = rc.bottom;

        // Get handles to all the controls.
        hPerformancePageTotalsFrame = GetDlgItem(hDlg, IDC_TOTALS_FRAME);
        hPerformancePageCommitChargeFrame = GetDlgItem(hDlg, IDC_COMMIT_CHARGE_FRAME);
        hPerformancePageKernelMemoryFrame = GetDlgItem(hDlg, IDC_KERNEL_MEMORY_FRAME);
        hPerformancePagePhysicalMemoryFrame = GetDlgItem(hDlg, IDC_PHYSICAL_MEMORY_FRAME);

        hPerformancePageCpuUsageFrame = GetDlgItem(hDlg, IDC_CPU_USAGE_FRAME);
        hPerformancePageMemUsageFrame = GetDlgItem(hDlg, IDC_MEM_USAGE_FRAME);
        hPerformancePageCpuUsageHistoryFrame = GetDlgItem(hDlg, IDC_CPU_USAGE_HISTORY_FRAME);
        hPerformancePageMemUsageHistoryFrame = GetDlgItem(hDlg, IDC_MEMORY_USAGE_HISTORY_FRAME);

        hPerformancePageCommitChargeTotalEdit = GetDlgItem(hDlg, IDC_COMMIT_CHARGE_TOTAL);
        hPerformancePageCommitChargeLimitEdit = GetDlgItem(hDlg, IDC_COMMIT_CHARGE_LIMIT);
        hPerformancePageCommitChargePeakEdit = GetDlgItem(hDlg, IDC_COMMIT_CHARGE_PEAK);
        hPerformancePageKernelMemoryTotalEdit = GetDlgItem(hDlg, IDC_KERNEL_MEMORY_TOTAL);
        hPerformancePageKernelMemoryPagedEdit = GetDlgItem(hDlg, IDC_KERNEL_MEMORY_PAGED);
        hPerformancePageKernelMemoryNonPagedEdit = GetDlgItem(hDlg, IDC_KERNEL_MEMORY_NONPAGED);
        hPerformancePagePhysicalMemoryTotalEdit = GetDlgItem(hDlg, IDC_PHYSICAL_MEMORY_TOTAL);
        hPerformancePagePhysicalMemoryAvailableEdit = GetDlgItem(hDlg, IDC_PHYSICAL_MEMORY_AVAILABLE);
        hPerformancePagePhysicalMemorySystemCacheEdit = GetDlgItem(hDlg, IDC_PHYSICAL_MEMORY_SYSTEM_CACHE);
        hPerformancePageTotalsHandleCountEdit = GetDlgItem(hDlg, IDC_TOTALS_HANDLE_COUNT);
        hPerformancePageTotalsProcessCountEdit = GetDlgItem(hDlg, IDC_TOTALS_PROCESS_COUNT);
        hPerformancePageTotalsThreadCountEdit = GetDlgItem(hDlg, IDC_TOTALS_THREAD_COUNT);

        hPerformancePageCpuUsageGraph = GetDlgItem(hDlg, IDC_CPU_USAGE_GRAPH);
        hPerformancePageMemUsageGraph = GetDlgItem(hDlg, IDC_MEM_USAGE_GRAPH);
        hPerformancePageMemUsageHistoryGraph = GetDlgItem(hDlg, IDC_MEM_USAGE_HISTORY_GRAPH);
        hPerformancePageCpuUsageHistoryGraph = GetDlgItem(hDlg, IDC_CPU_USAGE_HISTORY_GRAPH);

        GetClientRect(hPerformancePageCpuUsageHistoryGraph, &rc);
        /*  create the control */
        GraphCtrl_Create(&PerformancePageCpuUsageHistoryGraph, hPerformancePageCpuUsageHistoryGraph, hDlg, IDC_CPU_USAGE_HISTORY_GRAPH);
        /*  customize the control */
        GraphCtrl_SetRange(&PerformancePageCpuUsageHistoryGraph, 0.0, 100.0, 10);
        /*PerformancePageCpuUsageHistoryGraph.SetYUnits("Current") ; */
        /*PerformancePageCpuUsageHistoryGraph.SetXUnits("Samples (Windows Timer: 100 msec)") ; */
        /*PerformancePageCpuUsageHistoryGraph.SetBackgroundColor(RGB(0, 0, 64)) ; */
        /*PerformancePageCpuUsageHistoryGraph.SetGridColor(RGB(192, 192, 255)) ; */
        /*PerformancePageCpuUsageHistoryGraph.SetPlotColor(RGB(255, 255, 255)) ; */
        GraphCtrl_SetBackgroundColor(&PerformancePageCpuUsageHistoryGraph, RGB(0, 0, 0)) ;
        GraphCtrl_SetGridColor(&PerformancePageCpuUsageHistoryGraph, RGB(0, 128, 64));

        GraphCtrl_SetPlotColor(&PerformancePageCpuUsageHistoryGraph, 0, RGB(0, 255, 0)) ;
        GraphCtrl_SetPlotColor(&PerformancePageCpuUsageHistoryGraph, 1, RGB(255, 0, 0)) ;

        GetClientRect(hPerformancePageMemUsageHistoryGraph, &rc);
        GraphCtrl_Create(&PerformancePageMemUsageHistoryGraph, hPerformancePageMemUsageHistoryGraph, hDlg, IDC_MEM_USAGE_HISTORY_GRAPH);
        GraphCtrl_SetRange(&PerformancePageMemUsageHistoryGraph, 0.0, 100.0, 10) ;
        GraphCtrl_SetBackgroundColor(&PerformancePageMemUsageHistoryGraph, RGB(0, 0, 0)) ;
        GraphCtrl_SetGridColor(&PerformancePageMemUsageHistoryGraph, RGB(0, 128, 64)) ;
        GraphCtrl_SetPlotColor(&PerformancePageMemUsageHistoryGraph, 0, RGB(255, 255, 0)) ;

        /*  Start our refresh thread */
        hPerformanceThread = _beginthreadex(NULL, 0, PerformancePageRefreshThread, NULL, 0, &dwPerformanceThread);

        // Subclass graph buttons.
        OldGraphWndProc = (WNDPROC)(LONG_PTR)SetWindowLongPtr(hPerformancePageCpuUsageGraph, GWLP_WNDPROC, (LONG_PTR)Graph_WndProc);
        OldGraphCtrlWndProc = (WNDPROC)(LONG_PTR)SetWindowLongPtr(hPerformancePageMemUsageHistoryGraph, GWLP_WNDPROC, (LONG_PTR)GraphCtrl_WndProc);

        SetWindowLongPtr(hPerformancePageMemUsageGraph, GWLP_WNDPROC, (LONG_PTR)Graph_WndProc);
        SetWindowLongPtr(hPerformancePageCpuUsageHistoryGraph, GWLP_WNDPROC, (LONG_PTR)GraphCtrl_WndProc);
        return TRUE;

    case WM_SIZE:
        {
            INT cx = 0, cy = 0; 

            if (wParam == SIZE_MINIMIZED)
                return 0;

            cx = LOWORD(lParam);
            cy = HIWORD(lParam);
            nXDifference = cx - nPerformancePageWidth;
            nYDifference = cy - nPerformancePageHeight;
            nPerformancePageWidth = cx;
            nPerformancePageHeight = cy;


            /*  Reposition the performance page's controls */
            AdjustFrameSize(hPerformancePageTotalsFrame, hDlg, 0, nYDifference, 0);
            AdjustFrameSize(hPerformancePageCommitChargeFrame, hDlg, 0, nYDifference, 0);
            AdjustFrameSize(hPerformancePageKernelMemoryFrame, hDlg, 0, nYDifference, 0);
            AdjustFrameSize(hPerformancePagePhysicalMemoryFrame, hDlg, 0, nYDifference, 0);
            AdjustCntrlPos(IDS_COMMIT_CHARGE_TOTAL, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_COMMIT_CHARGE_LIMIT, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_COMMIT_CHARGE_PEAK, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_KERNEL_MEMORY_TOTAL, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_KERNEL_MEMORY_PAGED, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_KERNEL_MEMORY_NONPAGED, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_PHYSICAL_MEMORY_TOTAL, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_PHYSICAL_MEMORY_AVAILABLE, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_PHYSICAL_MEMORY_SYSTEM_CACHE, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_TOTALS_HANDLE_COUNT, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_TOTALS_PROCESS_COUNT, hDlg, 0, nYDifference);
            AdjustCntrlPos(IDS_TOTALS_THREAD_COUNT, hDlg, 0, nYDifference);

            AdjustControlPostion(hPerformancePageCommitChargeTotalEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePageCommitChargeLimitEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePageCommitChargePeakEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePageKernelMemoryTotalEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePageKernelMemoryPagedEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePageKernelMemoryNonPagedEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePagePhysicalMemoryTotalEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePagePhysicalMemoryAvailableEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePagePhysicalMemorySystemCacheEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePageTotalsHandleCountEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePageTotalsProcessCountEdit, hDlg, 0, nYDifference);
            AdjustControlPostion(hPerformancePageTotalsThreadCountEdit, hDlg, 0, nYDifference);

            nXDifference += lastX;
            nYDifference += lastY;
            lastX = lastY = 0;

            if (nXDifference % 2) 
            {
                if (nXDifference > 0) 
                {
                    nXDifference--;
                    lastX++;
                } 
                else 
                {
                    nXDifference++;
                    lastX--;
                }
            }

            if (nYDifference % 2) 
            {
                if (nYDifference > 0) 
                {
                    nYDifference--;
                    lastY++;
                } 
                else 
                {
                    nYDifference++;
                    lastY--;
                }
            }

            AdjustFrameSize(hPerformancePageCpuUsageFrame, hDlg, nXDifference, nYDifference, 1);
            AdjustFrameSize(hPerformancePageMemUsageFrame, hDlg, nXDifference, nYDifference, 2);
            AdjustFrameSize(hPerformancePageCpuUsageHistoryFrame, hDlg, nXDifference, nYDifference, 3);
            AdjustFrameSize(hPerformancePageMemUsageHistoryFrame, hDlg, nXDifference, nYDifference, 4);
            AdjustFrameSize(hPerformancePageCpuUsageGraph, hDlg, nXDifference, nYDifference, 1);
            AdjustFrameSize(hPerformancePageMemUsageGraph, hDlg, nXDifference, nYDifference, 2);
            AdjustFrameSize(hPerformancePageCpuUsageHistoryGraph, hDlg, nXDifference, nYDifference, 3);
            AdjustFrameSize(hPerformancePageMemUsageHistoryGraph, hDlg, nXDifference, nYDifference, 4);
        }
        break;
    }

    return 0;
}

UINT WINAPI PerformancePageRefreshThread(void *lpParameter)
{
    ULONG CommitChargeTotal = 0, 
          CommitChargeLimit = 0,  
          CommitChargePeak = 0,  
          CpuUsage = 0, 
          CpuKernelUsage = 0,  
          KernelMemoryTotal = 0,  
          KernelMemoryPaged = 0, 
          KernelMemoryNonPaged = 0,  
          PhysicalMemoryTotal = 0, 
          PhysicalMemoryAvailable = 0, 
          TotalHandles = 0, 
          TotalThreads = 0, 
          TotalProcesses = 0;
	SIZE_T PhysicalMemorySystemCache = 0;
    WCHAR Text[MAX_PATH], szMemUsage[256];
    BOOL refresh = TRUE;

	UNREFERENCED_PARAMETER(lpParameter);

    LoadString(hInst, IDS_STATUS_MEMUSAGE, szMemUsage, 256);

    while (refresh)
    {
        int nBarsUsed1;
        int nBarsUsed2;

        /*
        *  Update the commit charge info
        */
        CommitChargeTotal = PerfDataGetCommitChargeTotalK();
        CommitChargeLimit = PerfDataGetCommitChargeLimitK();
        CommitChargePeak = PerfDataGetCommitChargePeakK();
        _ultow_s(CommitChargeTotal, Text, _countof(Text), 10);
        SetWindowText(hPerformancePageCommitChargeTotalEdit, Text);
        _ultow_s(CommitChargeLimit, Text, _countof(Text), 10);
        SetWindowText(hPerformancePageCommitChargeLimitEdit, Text);
        _ultow_s(CommitChargePeak, Text, _countof(Text), 10);
        SetWindowText(hPerformancePageCommitChargePeakEdit, Text);
        wsprintf(Text, szMemUsage, CommitChargeTotal, CommitChargeLimit);
        SendMessage(hStatusWnd, SB_SETTEXT, 2, (LPARAM)Text);

        /*
        *  Update the kernel memory info
        */
        KernelMemoryTotal = PerfDataGetKernelMemoryTotalK();
        KernelMemoryPaged = PerfDataGetKernelMemoryPagedK();
        KernelMemoryNonPaged = PerfDataGetKernelMemoryNonPagedK();
        _ultow_s(KernelMemoryTotal, Text, _countof(Text), 10);
        SetWindowText(hPerformancePageKernelMemoryTotalEdit, Text);
        _ultow_s(KernelMemoryPaged, Text, _countof(Text), 10);
        SetWindowText(hPerformancePageKernelMemoryPagedEdit, Text);
        _ultow_s(KernelMemoryNonPaged, Text, _countof(Text), 10);
        SetWindowText(hPerformancePageKernelMemoryNonPagedEdit, Text);

        /*
        *  Update the physical memory info
        */
        PhysicalMemoryTotal = PerfDataGetPhysicalMemoryTotalK();
        PhysicalMemoryAvailable = PerfDataGetPhysicalMemoryAvailableK();
        PhysicalMemorySystemCache = PerfDataGetPhysicalMemorySystemCacheK();
        _ultow_s(PhysicalMemoryTotal, Text, _countof(Text), 10);
        SetWindowText(hPerformancePagePhysicalMemoryTotalEdit, Text);
        _ultow_s(PhysicalMemoryAvailable, Text, _countof(Text), 10);
        SetWindowText(hPerformancePagePhysicalMemoryAvailableEdit, Text);
        _ultow_s(PhysicalMemorySystemCache, Text, _countof(Text), 10);
        SetWindowText(hPerformancePagePhysicalMemorySystemCacheEdit, Text);

        /*
        *  Update the totals info
        */
        TotalHandles = PerfDataGetSystemHandleCount();
        TotalThreads = PerfDataGetTotalThreadCount();
        TotalProcesses = PerfDataGetProcessCount();
        _ultow_s(TotalHandles, Text, _countof(Text), 10);
        SetWindowText(hPerformancePageTotalsHandleCountEdit, Text);
        _ultow_s(TotalThreads, Text, _countof(Text), 10);
        SetWindowText(hPerformancePageTotalsThreadCountEdit, Text);
        _ultow_s(TotalProcesses, Text, _countof(Text), 10);
        SetWindowText(hPerformancePageTotalsProcessCountEdit, Text);

        /*
        *  Redraw the graphs
        */
        InvalidateRect(hPerformancePageCpuUsageGraph, NULL, FALSE);
        InvalidateRect(hPerformancePageMemUsageGraph, NULL, FALSE);

        /*
        *  Get the CPU usage
        */
        CpuUsage = PerfDataGetProcessorUsage();

        if (CpuUsage <= 0 )       
            CpuUsage = 0;
        if (CpuUsage > 100)       
            CpuUsage = 100;

        if (TaskManagerSettings.ShowKernelTimes)
        {
            CpuKernelUsage = PerfDataGetProcessorSystemUsage();
            if (CpuKernelUsage <= 0)   CpuKernelUsage = 0;
            if (CpuKernelUsage > 100) CpuKernelUsage = 100;
        }
        else
        {
            CpuKernelUsage = 0;
        }
        /*
        *  Get the memory usage
        */
        CommitChargeTotal = PerfDataGetCommitChargeTotalK();
        CommitChargeLimit = PerfDataGetCommitChargeLimitK();
        nBarsUsed1 = CommitChargeLimit ? ((CommitChargeTotal * 100) / CommitChargeLimit) : 0;

        PhysicalMemoryTotal = PerfDataGetPhysicalMemoryTotalK();
        PhysicalMemoryAvailable = PerfDataGetPhysicalMemoryAvailableK();
        nBarsUsed2 = PhysicalMemoryTotal ? ((PhysicalMemoryAvailable * 100) / PhysicalMemoryTotal) : 0;

        GraphCtrl_AppendPoint(&PerformancePageCpuUsageHistoryGraph, CpuUsage, CpuKernelUsage, 0.0, 0.0);
        GraphCtrl_AppendPoint(&PerformancePageMemUsageHistoryGraph, nBarsUsed1, nBarsUsed2, 0.0, 0.0);
        /* PerformancePageMemUsageHistoryGraph.SetRange(0.0, 100.0, 10) ; */
        InvalidateRect(hPerformancePageMemUsageHistoryGraph, NULL, FALSE);
        InvalidateRect(hPerformancePageCpuUsageHistoryGraph, NULL, FALSE);

        Sleep(1000);
    }

    return 0;
}

void PerformancePage_OnViewShowKernelTimes(void)
{
    HMENU hMenu = GetMenu(hMainWnd);
    HMENU hViewMenu = GetSubMenu(hMenu, 2);

    /*  Check or uncheck the show 16-bit tasks menu item */
    if (GetMenuState(hViewMenu, ID_VIEW_SHOWKERNELTIMES, MF_BYCOMMAND) & MF_CHECKED)
    {
        CheckMenuItem(hViewMenu, ID_VIEW_SHOWKERNELTIMES, MF_BYCOMMAND | MF_UNCHECKED);
        TaskManagerSettings.ShowKernelTimes = FALSE;
    }
    else
    {
        CheckMenuItem(hViewMenu, ID_VIEW_SHOWKERNELTIMES, MF_BYCOMMAND | MF_CHECKED);
        TaskManagerSettings.ShowKernelTimes = TRUE;
    }
}

void PerformancePage_OnViewCPUHistoryOneGraphAll(void)
{
    HMENU hMenu = GetMenu(hMainWnd);
    HMENU hViewMenu = GetSubMenu(hMenu, 2);
    HMENU hCPUHistoryMenu = GetSubMenu(hViewMenu, 3);

    TaskManagerSettings.CPUHistory_OneGraphPerCPU = FALSE;
    CheckMenuRadioItem(hCPUHistoryMenu, ID_VIEW_CPUHISTORY_ONEGRAPHALL, ID_VIEW_CPUHISTORY_ONEGRAPHPERCPU, ID_VIEW_CPUHISTORY_ONEGRAPHALL, MF_BYCOMMAND);
}

void PerformancePage_OnViewCPUHistoryOneGraphPerCPU(void)
{
    HMENU hMenu = GetMenu(hMainWnd);
    HMENU hViewMenu = GetSubMenu(hMenu, 2);
    HMENU hCPUHistoryMenu = GetSubMenu(hViewMenu, 3);

    TaskManagerSettings.CPUHistory_OneGraphPerCPU = TRUE;
    CheckMenuRadioItem(hCPUHistoryMenu, ID_VIEW_CPUHISTORY_ONEGRAPHALL, ID_VIEW_CPUHISTORY_ONEGRAPHPERCPU, ID_VIEW_CPUHISTORY_ONEGRAPHPERCPU, MF_BYCOMMAND);
}

