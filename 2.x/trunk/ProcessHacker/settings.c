/*
 * Process Hacker -
 *   program settings
 *
 * Copyright (C) 2010-2015 wj32
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

/*
 * This file contains a program-specific settings system. All possible
 * settings are defined at program startup and added to a hashtable.
 * The values of these settings can then be read in from a XML file or
 * saved to a XML file at any time. Settings which are not recognized
 * are added to a list of "ignored settings"; this is necessary to
 * support plugin settings, as we don't want their settings to get
 * deleted whenever the plugins are disabled.
 *
 * The get/set functions are very strict. If the wrong function is used
 * (the get-integer-setting function is used on a string setting) or
 * the setting does not exist, an exception will be raised.
 */

#define PH_SETTINGS_PRIVATE
#include <phapp.h>
#include "mxml/mxml.h"
#include <settings.h>
#include <settingsp.h>

PPH_HASHTABLE PhSettingsHashtable;
PH_QUEUED_LOCK PhSettingsLock = PH_QUEUED_LOCK_INIT;

PPH_LIST PhIgnoredSettings;

// These macros make sure the C strings can be seamlessly converted into
// PH_STRINGREFs at compile time, for a small speed boost.

#define ADD_SETTING_WRAPPER(Type, Name, DefaultValue) \
    { \
        static PH_STRINGREF name = PH_STRINGREF_INIT(Name); \
        static PH_STRINGREF defaultValue = PH_STRINGREF_INIT(DefaultValue); \
        PhpAddSetting(Type, &name, &defaultValue); \
    }

#define PhpAddStringSetting(A, B) ADD_SETTING_WRAPPER(StringSettingType, A, B)
#define PhpAddIntegerSetting(A, B) ADD_SETTING_WRAPPER(IntegerSettingType, A, B)
#define PhpAddIntegerPairSetting(A, B) ADD_SETTING_WRAPPER(IntegerPairSettingType, A, B)

VOID PhSettingsInitialization(
    VOID
    )
{
    PhSettingsHashtable = PhCreateHashtable(
        sizeof(PH_SETTING),
        PhpSettingsHashtableCompareFunction,
        PhpSettingsHashtableHashFunction,
        256
        );
    PhIgnoredSettings = PhCreateList(4);

    PhpAddIntegerSetting(L"AllowOnlyOneInstance", L"1");
    PhpAddIntegerSetting(L"CloseOnEscape", L"0");
    PhpAddIntegerSetting(L"CollapseServicesOnStart", L"0");
    PhpAddStringSetting(L"DbgHelpPath", L"dbghelp.dll");
    PhpAddStringSetting(L"DbgHelpSearchPath", L"");
    PhpAddIntegerSetting(L"DbgHelpUndecorate", L"1");
    PhpAddStringSetting(L"DisabledPlugins", L"");
    PhpAddIntegerSetting(L"ElevationLevel", L"1"); // PromptElevateAction
    PhpAddIntegerSetting(L"EnableCycleCpuUsage", L"1");
    PhpAddIntegerSetting(L"EnableInstantTooltips", L"0");
    PhpAddIntegerSetting(L"EnableKph", L"1");
    PhpAddIntegerSetting(L"EnableNetworkResolve", L"1");
    PhpAddIntegerSetting(L"EnablePlugins", L"1");
    PhpAddIntegerSetting(L"EnableServiceNonPoll", L"0");
    PhpAddIntegerSetting(L"EnableStage2", L"1");
    PhpAddIntegerSetting(L"EnableWarnings", L"1");
    PhpAddStringSetting(L"EnvironmentListViewColumns", L"");
    PhpAddStringSetting(L"FindObjListViewColumns", L"");
    PhpAddIntegerPairSetting(L"FindObjWindowPosition", L"350,350");
    PhpAddIntegerPairSetting(L"FindObjWindowSize", L"550,420");
    PhpAddIntegerSetting(L"FirstRun", L"1");
    PhpAddStringSetting(L"Font", L""); // null
    PhpAddIntegerSetting(L"ForceNoParent", L"0");
    PhpAddStringSetting(L"HandleTreeListColumns", L"");
    PhpAddStringSetting(L"HandleTreeListSort", L"0,1"); // 0, AscendingSortOrder
    PhpAddStringSetting(L"HiddenProcessesListViewColumns", L"");
    PhpAddIntegerPairSetting(L"HiddenProcessesWindowPosition", L"400,400");
    PhpAddIntegerPairSetting(L"HiddenProcessesWindowSize", L"520,400");
    PhpAddIntegerSetting(L"HideDriverServices", L"0");
    PhpAddIntegerSetting(L"HideOnClose", L"0");
    PhpAddIntegerSetting(L"HideOnMinimize", L"0");
    PhpAddIntegerSetting(L"HideOtherUserProcesses", L"0");
    PhpAddIntegerSetting(L"HideSignedProcesses", L"0");
    PhpAddIntegerSetting(L"HideUnnamedHandles", L"1");
    PhpAddIntegerSetting(L"HighlightingDuration", L"3e8"); // 1000ms
    PhpAddIntegerSetting(L"IconMask", L"1"); // PH_ICON_CPU_HISTORY
    PhpAddStringSetting(L"IconMaskList", L"");
    PhpAddIntegerSetting(L"IconNotifyMask", L"c"); // PH_NOTIFY_SERVICE_CREATE | PH_NOTIFY_SERVICE_DELETE
    PhpAddIntegerSetting(L"IconProcesses", L"f"); // 15
    PhpAddIntegerSetting(L"IconSingleClick", L"0");
    PhpAddIntegerSetting(L"IconTogglesVisibility", L"1");
    PhpAddIntegerSetting(L"LogEntries", L"200"); // 512
    PhpAddStringSetting(L"LogListViewColumns", L"");
    PhpAddIntegerPairSetting(L"LogWindowPosition", L"300,300");
    PhpAddIntegerPairSetting(L"LogWindowSize", L"450,500");
    PhpAddIntegerSetting(L"MainWindowAlwaysOnTop", L"0");
    PhpAddIntegerSetting(L"MainWindowOpacity", L"0"); // means 100%
    PhpAddIntegerPairSetting(L"MainWindowPosition", L"100,100");
    PhpAddIntegerPairSetting(L"MainWindowSize", L"800,600");
    PhpAddIntegerSetting(L"MainWindowState", L"1");
    PhpAddIntegerSetting(L"MaxSizeUnit", L"6");
    PhpAddIntegerSetting(L"MemEditBytesPerRow", L"10"); // 16
    PhpAddStringSetting(L"MemEditGotoChoices", L"");
    PhpAddIntegerPairSetting(L"MemEditPosition", L"450,450");
    PhpAddIntegerPairSetting(L"MemEditSize", L"600,500");
    PhpAddStringSetting(L"MemFilterChoices", L"");
    PhpAddStringSetting(L"MemResultsListViewColumns", L"");
    PhpAddIntegerPairSetting(L"MemResultsPosition", L"300,300");
    PhpAddIntegerPairSetting(L"MemResultsSize", L"500,520");
    PhpAddStringSetting(L"MemoryTreeListColumns", L"");
    PhpAddStringSetting(L"MemoryTreeListSort", L"0,0"); // 0, NoSortOrder
    PhpAddIntegerPairSetting(L"MemoryListsWindowPosition", L"400,400");
    PhpAddStringSetting(L"MemoryReadWriteAddressChoices", L"");
    PhpAddIntegerSetting(L"MiniInfoWindowEnabled", L"1");
    PhpAddIntegerSetting(L"MiniInfoWindowOpacity", L"0"); // means 100%
    PhpAddIntegerSetting(L"MiniInfoWindowPinned", L"0");
    PhpAddIntegerPairSetting(L"MiniInfoWindowPosition", L"200,200");
    PhpAddIntegerSetting(L"MiniInfoWindowRefreshAutomatically", L"1");
    PhpAddIntegerPairSetting(L"MiniInfoWindowSize", L"10,10");
    PhpAddStringSetting(L"ModuleTreeListColumns", L"");
    PhpAddStringSetting(L"ModuleTreeListSort", L"0,0"); // 0, NoSortOrder
    PhpAddStringSetting(L"NetworkTreeListColumns", L"");
    PhpAddStringSetting(L"NetworkTreeListSort", L"0,1"); // 0, AscendingSortOrder
    PhpAddIntegerSetting(L"NoPurgeProcessRecords", L"0");
    PhpAddStringSetting(L"PluginsDirectory", L"plugins");
    PhpAddStringSetting(L"ProcessServiceListViewColumns", L"");
    PhpAddStringSetting(L"ProcessTreeListColumns", L"");
    PhpAddStringSetting(L"ProcessTreeListSort", L"0,0"); // 0, NoSortOrder
    PhpAddStringSetting(L"ProcPropPage", L"General");
    PhpAddIntegerPairSetting(L"ProcPropPosition", L"200,200");
    PhpAddIntegerPairSetting(L"ProcPropSize", L"460,580");
    PhpAddStringSetting(L"ProgramInspectExecutables", L"peview.exe \"%s\"");
    PhpAddIntegerSetting(L"PropagateCpuUsage", L"0");
    PhpAddStringSetting(L"RunAsProgram", L"");
    PhpAddStringSetting(L"RunAsUserName", L"");
    PhpAddIntegerSetting(L"SampleCount", L"200"); // 512
    PhpAddIntegerSetting(L"SampleCountAutomatic", L"1");
    PhpAddIntegerSetting(L"ScrollToNewProcesses", L"0");
    PhpAddStringSetting(L"SearchEngine", L"http://www.google.com/search?q=\"%s\"");
    PhpAddStringSetting(L"ServiceListViewColumns", L"");
    PhpAddStringSetting(L"ServiceTreeListColumns", L"");
    PhpAddStringSetting(L"ServiceTreeListSort", L"0,1"); // 0, AscendingSortOrder
    PhpAddIntegerPairSetting(L"SessionShadowHotkey", L"106,2"); // VK_MULTIPLY,KBDCTRL
    PhpAddIntegerSetting(L"ShowCommitInSummary", L"1");
    PhpAddIntegerSetting(L"ShowCpuBelow001", L"0");
    PhpAddIntegerSetting(L"StartHidden", L"0");
    PhpAddIntegerSetting(L"SysInfoWindowAlwaysOnTop", L"0");
    PhpAddIntegerSetting(L"SysInfoWindowOneGraphPerCpu", L"0");
    PhpAddIntegerPairSetting(L"SysInfoWindowPosition", L"200,200");
    PhpAddStringSetting(L"SysInfoWindowSection", L"");
    PhpAddIntegerPairSetting(L"SysInfoWindowSize", L"620,590");
    PhpAddIntegerSetting(L"ThinRows", L"0");
    PhpAddStringSetting(L"ThreadTreeListColumns", L"");
    PhpAddStringSetting(L"ThreadTreeListSort", L"1,2"); // 1, DescendingSortOrder
    PhpAddStringSetting(L"ThreadStackListViewColumns", L"");
    PhpAddIntegerPairSetting(L"ThreadStackWindowSize", L"420,380");
    PhpAddIntegerSetting(L"UpdateInterval", L"3e8"); // 1000ms

    // Colors are specified with R in the lowest byte, then G, then B.
    // So: bbggrr.
    PhpAddIntegerSetting(L"ColorNew", L"00ff7f"); // Chartreuse
    PhpAddIntegerSetting(L"ColorRemoved", L"283cff");
    PhpAddIntegerSetting(L"UseColorOwnProcesses", L"1");
    PhpAddIntegerSetting(L"ColorOwnProcesses", L"aaffff");
    PhpAddIntegerSetting(L"UseColorSystemProcesses", L"1");
    PhpAddIntegerSetting(L"ColorSystemProcesses", L"ffccaa");
    PhpAddIntegerSetting(L"UseColorServiceProcesses", L"1");
    PhpAddIntegerSetting(L"ColorServiceProcesses", L"ffffcc");
    PhpAddIntegerSetting(L"UseColorJobProcesses", L"1");
    PhpAddIntegerSetting(L"ColorJobProcesses", L"3f85cd"); // Peru
    PhpAddIntegerSetting(L"UseColorWow64Processes", L"1");
    PhpAddIntegerSetting(L"ColorWow64Processes", L"8f8fbc"); // Rosy Brown
    PhpAddIntegerSetting(L"UseColorPosixProcesses", L"1");
    PhpAddIntegerSetting(L"ColorPosixProcesses", L"8b3d48"); // Dark Slate Blue
    PhpAddIntegerSetting(L"UseColorDebuggedProcesses", L"1");
    PhpAddIntegerSetting(L"ColorDebuggedProcesses", L"ffbbcc");
    PhpAddIntegerSetting(L"UseColorElevatedProcesses", L"1");
    PhpAddIntegerSetting(L"ColorElevatedProcesses", L"00aaff");
    PhpAddIntegerSetting(L"UseColorImmersiveProcesses", L"1");
    PhpAddIntegerSetting(L"ColorImmersiveProcesses", L"cbc0ff"); // Pink
    PhpAddIntegerSetting(L"UseColorSuspended", L"1");
    PhpAddIntegerSetting(L"ColorSuspended", L"777777");
    PhpAddIntegerSetting(L"UseColorDotNet", L"1");
    PhpAddIntegerSetting(L"ColorDotNet", L"00ffde");
    PhpAddIntegerSetting(L"UseColorPacked", L"1");
    PhpAddIntegerSetting(L"ColorPacked", L"9314ff"); // Deep Pink
    PhpAddIntegerSetting(L"UseColorGuiThreads", L"1");
    PhpAddIntegerSetting(L"ColorGuiThreads", L"77ffff");
    PhpAddIntegerSetting(L"UseColorRelocatedModules", L"1");
    PhpAddIntegerSetting(L"ColorRelocatedModules", L"80c0ff");
    PhpAddIntegerSetting(L"UseColorProtectedHandles", L"1");
    PhpAddIntegerSetting(L"ColorProtectedHandles", L"777777");
    PhpAddIntegerSetting(L"UseColorInheritHandles", L"1");
    PhpAddIntegerSetting(L"ColorInheritHandles", L"ffff77");

    PhpAddIntegerSetting(L"GraphShowText", L"1");
    PhpAddIntegerSetting(L"GraphColorMode", L"0");
    PhpAddIntegerSetting(L"ColorCpuKernel", L"00ff00");
    PhpAddIntegerSetting(L"ColorCpuUser", L"0000ff");
    PhpAddIntegerSetting(L"ColorIoReadOther", L"00ffff");
    PhpAddIntegerSetting(L"ColorIoWrite", L"ff0077");
    PhpAddIntegerSetting(L"ColorPrivate", L"0077ff");
    PhpAddIntegerSetting(L"ColorPhysical", L"ffff00");

    PhUpdateCachedSettings();
}

VOID PhUpdateCachedSettings(
    VOID
    )
{
#define UPDATE_INTEGER_CS(Name) (PhCs##Name = PhGetIntegerSetting(L#Name))

    UPDATE_INTEGER_CS(CollapseServicesOnStart);
    UPDATE_INTEGER_CS(ForceNoParent);
    UPDATE_INTEGER_CS(HighlightingDuration);
    UPDATE_INTEGER_CS(PropagateCpuUsage);
    UPDATE_INTEGER_CS(ScrollToNewProcesses);
    UPDATE_INTEGER_CS(ShowCpuBelow001);
    UPDATE_INTEGER_CS(UpdateInterval);

    UPDATE_INTEGER_CS(ColorNew);
    UPDATE_INTEGER_CS(ColorRemoved);
    UPDATE_INTEGER_CS(UseColorOwnProcesses);
    UPDATE_INTEGER_CS(ColorOwnProcesses);
    UPDATE_INTEGER_CS(UseColorSystemProcesses);
    UPDATE_INTEGER_CS(ColorSystemProcesses);
    UPDATE_INTEGER_CS(UseColorServiceProcesses);
    UPDATE_INTEGER_CS(ColorServiceProcesses);
    UPDATE_INTEGER_CS(UseColorJobProcesses);
    UPDATE_INTEGER_CS(ColorJobProcesses);
    UPDATE_INTEGER_CS(UseColorWow64Processes);
    UPDATE_INTEGER_CS(ColorWow64Processes);
    UPDATE_INTEGER_CS(UseColorPosixProcesses);
    UPDATE_INTEGER_CS(ColorPosixProcesses);
    UPDATE_INTEGER_CS(UseColorDebuggedProcesses);
    UPDATE_INTEGER_CS(ColorDebuggedProcesses);
    UPDATE_INTEGER_CS(UseColorElevatedProcesses);
    UPDATE_INTEGER_CS(ColorElevatedProcesses);
    UPDATE_INTEGER_CS(UseColorImmersiveProcesses);
    UPDATE_INTEGER_CS(ColorImmersiveProcesses);
    UPDATE_INTEGER_CS(UseColorSuspended);
    UPDATE_INTEGER_CS(ColorSuspended);
    UPDATE_INTEGER_CS(UseColorDotNet);
    UPDATE_INTEGER_CS(ColorDotNet);
    UPDATE_INTEGER_CS(UseColorPacked);
    UPDATE_INTEGER_CS(ColorPacked);
    UPDATE_INTEGER_CS(UseColorGuiThreads);
    UPDATE_INTEGER_CS(ColorGuiThreads);
    UPDATE_INTEGER_CS(UseColorRelocatedModules);
    UPDATE_INTEGER_CS(ColorRelocatedModules);
    UPDATE_INTEGER_CS(UseColorProtectedHandles);
    UPDATE_INTEGER_CS(ColorProtectedHandles);
    UPDATE_INTEGER_CS(UseColorInheritHandles);
    UPDATE_INTEGER_CS(ColorInheritHandles);
    UPDATE_INTEGER_CS(GraphShowText);
    UPDATE_INTEGER_CS(GraphColorMode);
    UPDATE_INTEGER_CS(ColorCpuKernel);
    UPDATE_INTEGER_CS(ColorCpuUser);
    UPDATE_INTEGER_CS(ColorIoReadOther);
    UPDATE_INTEGER_CS(ColorIoWrite);
    UPDATE_INTEGER_CS(ColorPrivate);
    UPDATE_INTEGER_CS(ColorPhysical);
}

BOOLEAN NTAPI PhpSettingsHashtableCompareFunction(
    _In_ PVOID Entry1,
    _In_ PVOID Entry2
    )
{
    PPH_SETTING setting1 = (PPH_SETTING)Entry1;
    PPH_SETTING setting2 = (PPH_SETTING)Entry2;

    return PhEqualStringRef(&setting1->Name, &setting2->Name, FALSE);
}

ULONG NTAPI PhpSettingsHashtableHashFunction(
    _In_ PVOID Entry
    )
{
    PPH_SETTING setting = (PPH_SETTING)Entry;

    return PhHashBytes((PUCHAR)setting->Name.Buffer, setting->Name.Length);
}

static VOID PhpAddSetting(
    _In_ PH_SETTING_TYPE Type,
    _In_ PPH_STRINGREF Name,
    _In_ PPH_STRINGREF DefaultValue
    )
{
    PH_SETTING setting;

    setting.Type = Type;
    setting.Name = *Name;
    setting.DefaultValue = *DefaultValue;
    memset(&setting.u, 0, sizeof(setting.u));

    PhpSettingFromString(Type, &setting.DefaultValue, NULL, &setting);

    PhAddEntryHashtable(PhSettingsHashtable, &setting);
}

static PPH_STRING PhpSettingToString(
    _In_ PH_SETTING_TYPE Type,
    _In_ PPH_SETTING Setting
    )
{
    switch (Type)
    {
    case StringSettingType:
        {
            if (!Setting->u.Pointer)
                return PhReferenceEmptyString();

            PhReferenceObject(Setting->u.Pointer);

            return (PPH_STRING)Setting->u.Pointer;
        }
    case IntegerSettingType:
        {
            return PhFormatString(L"%x", Setting->u.Integer);
        }
    case IntegerPairSettingType:
        {
            PPH_INTEGER_PAIR integerPair = &Setting->u.IntegerPair;

            return PhFormatString(L"%u,%u", integerPair->X, integerPair->Y);
        }
    }

    return PhReferenceEmptyString();
}

static BOOLEAN PhpSettingFromString(
    _In_ PH_SETTING_TYPE Type,
    _In_ PPH_STRINGREF StringRef,
    _In_opt_ PPH_STRING String,
    _Inout_ PPH_SETTING Setting
    )
{
    switch (Type)
    {
    case StringSettingType:
        {
            if (String)
            {
                PhSetReference(&Setting->u.Pointer, String);
            }
            else
            {
                Setting->u.Pointer = PhCreateString2(StringRef);
            }

            return TRUE;
        }
    case IntegerSettingType:
        {
            ULONG64 integer;

            if (PhStringToInteger64(StringRef, 16, &integer))
            {
                Setting->u.Integer = (ULONG)integer;
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    case IntegerPairSettingType:
        {
            LONG64 x;
            LONG64 y;
            PH_STRINGREF xString;
            PH_STRINGREF yString;

            if (!PhSplitStringRefAtChar(StringRef, ',', &xString, &yString))
                return FALSE;

            if (PhStringToInteger64(&xString, 10, &x) && PhStringToInteger64(&yString, 10, &y))
            {
                Setting->u.IntegerPair.X = (LONG)x;
                Setting->u.IntegerPair.Y = (LONG)y;
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }

    return FALSE;
}

static VOID PhpFreeSettingValue(
    _In_ PH_SETTING_TYPE Type,
    _In_ PPH_SETTING Setting
    )
{
    switch (Type)
    {
    case StringSettingType:
        if (Setting->u.Pointer)
            PhDereferenceObject(Setting->u.Pointer);
        break;
    }
}

static PVOID PhpLookupSetting(
    _In_ PPH_STRINGREF Name
    )
{
    PH_SETTING lookupSetting;
    PPH_SETTING setting;

    lookupSetting.Name = *Name;
    setting = (PPH_SETTING)PhFindEntryHashtable(
        PhSettingsHashtable,
        &lookupSetting
        );

    return setting;
}

_May_raise_ ULONG PhGetIntegerSetting(
    _In_ PWSTR Name
    )
{
    PPH_SETTING setting;
    PH_STRINGREF name;
    ULONG value;

    PhInitializeStringRef(&name, Name);

    PhAcquireQueuedLockShared(&PhSettingsLock);

    setting = PhpLookupSetting(&name);

    if (setting && setting->Type == IntegerSettingType)
    {
        value = setting->u.Integer;
    }
    else
    {
        setting = NULL;
    }

    PhReleaseQueuedLockShared(&PhSettingsLock);

    if (!setting)
        PhRaiseStatus(STATUS_NOT_FOUND);

    return value;
}

_May_raise_ PH_INTEGER_PAIR PhGetIntegerPairSetting(
    _In_ PWSTR Name
    )
{
    PPH_SETTING setting;
    PH_STRINGREF name;
    PH_INTEGER_PAIR value;

    PhInitializeStringRef(&name, Name);

    PhAcquireQueuedLockShared(&PhSettingsLock);

    setting = PhpLookupSetting(&name);

    if (setting && setting->Type == IntegerPairSettingType)
    {
        value = setting->u.IntegerPair;
    }
    else
    {
        setting = NULL;
    }

    PhReleaseQueuedLockShared(&PhSettingsLock);

    if (!setting)
        PhRaiseStatus(STATUS_NOT_FOUND);

    return value;
}

_May_raise_ PPH_STRING PhGetStringSetting(
    _In_ PWSTR Name
    )
{
    PPH_SETTING setting;
    PH_STRINGREF name;
    PPH_STRING value;

    PhInitializeStringRef(&name, Name);

    PhAcquireQueuedLockShared(&PhSettingsLock);

    setting = PhpLookupSetting(&name);

    if (setting && setting->Type == StringSettingType)
    {
        if (setting->u.Pointer)
        {
            PhSetReference(&value, setting->u.Pointer);
        }
        else
        {
            // Set to NULL, create an empty string
            // outside of the lock.
            value = NULL;
        }
    }
    else
    {
        setting = NULL;
    }

    PhReleaseQueuedLockShared(&PhSettingsLock);

    if (!setting)
        PhRaiseStatus(STATUS_NOT_FOUND);

    if (!value)
        value = PhReferenceEmptyString();

    return value;
}

_May_raise_ VOID PhSetIntegerSetting(
    _In_ PWSTR Name,
    _In_ ULONG Value
    )
{
    PPH_SETTING setting;
    PH_STRINGREF name;

    PhInitializeStringRef(&name, Name);

    PhAcquireQueuedLockExclusive(&PhSettingsLock);

    setting = PhpLookupSetting(&name);

    if (setting && setting->Type == IntegerSettingType)
    {
        setting->u.Integer = Value;
    }

    PhReleaseQueuedLockExclusive(&PhSettingsLock);

    if (!setting)
        PhRaiseStatus(STATUS_NOT_FOUND);
}

_May_raise_ VOID PhSetIntegerPairSetting(
    _In_ PWSTR Name,
    _In_ PH_INTEGER_PAIR Value
    )
{
    PPH_SETTING setting;
    PH_STRINGREF name;

    PhInitializeStringRef(&name, Name);

    PhAcquireQueuedLockExclusive(&PhSettingsLock);

    setting = PhpLookupSetting(&name);

    if (setting && setting->Type == IntegerPairSettingType)
    {
        setting->u.IntegerPair = Value;
    }

    PhReleaseQueuedLockExclusive(&PhSettingsLock);

    if (!setting)
        PhRaiseStatus(STATUS_NOT_FOUND);
}

_May_raise_ VOID PhSetStringSetting(
    _In_ PWSTR Name,
    _In_ PWSTR Value
    )
{
    PPH_SETTING setting;
    PH_STRINGREF name;

    PhInitializeStringRef(&name, Name);

    PhAcquireQueuedLockExclusive(&PhSettingsLock);

    setting = PhpLookupSetting(&name);

    if (setting && setting->Type == StringSettingType)
    {
        PhpFreeSettingValue(StringSettingType, setting);
        setting->u.Pointer = PhCreateString(Value);
    }

    PhReleaseQueuedLockExclusive(&PhSettingsLock);

    if (!setting)
        PhRaiseStatus(STATUS_NOT_FOUND);
}

_May_raise_ VOID PhSetStringSetting2(
    _In_ PWSTR Name,
    _In_ PPH_STRINGREF Value
    )
{
    PPH_SETTING setting;
    PH_STRINGREF name;

    PhInitializeStringRef(&name, Name);

    PhAcquireQueuedLockExclusive(&PhSettingsLock);

    setting = PhpLookupSetting(&name);

    if (setting && setting->Type == StringSettingType)
    {
        PhpFreeSettingValue(StringSettingType, setting);
        setting->u.Pointer = PhCreateString2(Value);
    }

    PhReleaseQueuedLockExclusive(&PhSettingsLock);

    if (!setting)
        PhRaiseStatus(STATUS_NOT_FOUND);
}

VOID PhpFreeIgnoredSetting(
    _In_ PPH_SETTING Setting
    )
{
    PhFree(Setting->Name.Buffer);
    PhDereferenceObject(Setting->u.Pointer);

    PhFree(Setting);
}

VOID PhpClearIgnoredSettings(
    VOID
    )
{
    ULONG i;

    PhAcquireQueuedLockExclusive(&PhSettingsLock);

    for (i = 0; i < PhIgnoredSettings->Count; i++)
    {
        PhpFreeIgnoredSetting(PhIgnoredSettings->Items[i]);
    }

    PhClearList(PhIgnoredSettings);

    PhReleaseQueuedLockExclusive(&PhSettingsLock);
}

VOID PhClearIgnoredSettings(
    VOID
    )
{
    PhpClearIgnoredSettings();
}

VOID PhConvertIgnoredSettings(
    VOID
    )
{
    ULONG i;

    PhAcquireQueuedLockExclusive(&PhSettingsLock);

    for (i = 0; i < PhIgnoredSettings->Count; i++)
    {
        PPH_SETTING ignoredSetting = PhIgnoredSettings->Items[i];
        PPH_SETTING setting;

        setting = PhpLookupSetting(&ignoredSetting->Name);

        if (setting)
        {
            PhpFreeSettingValue(setting->Type, setting);

            if (!PhpSettingFromString(
                setting->Type,
                &((PPH_STRING)ignoredSetting->u.Pointer)->sr,
                ignoredSetting->u.Pointer,
                setting
                ))
            {
                PhpSettingFromString(
                    setting->Type,
                    &setting->DefaultValue,
                    NULL,
                    setting
                    );
            }

            PhpFreeIgnoredSetting(ignoredSetting);

            PhRemoveItemList(PhIgnoredSettings, i);
            i--;
        }
    }

    PhReleaseQueuedLockExclusive(&PhSettingsLock);
}

mxml_type_t PhpSettingsLoadCallback(
    _In_ mxml_node_t *node
    )
{
    return MXML_OPAQUE;
}

NTSTATUS PhLoadSettings(
    _In_ PWSTR FileName
    )
{
    NTSTATUS status;
    HANDLE fileHandle;
    LARGE_INTEGER fileSize;
    mxml_node_t *topNode;
    mxml_node_t *currentNode;

    PhpClearIgnoredSettings();

    status = PhCreateFileWin32(
        &fileHandle,
        FileName,
        FILE_GENERIC_READ,
        0,
        FILE_SHARE_READ | FILE_SHARE_DELETE,
        FILE_OPEN,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
        );

    if (!NT_SUCCESS(status))
        return status;

    if (NT_SUCCESS(PhGetFileSize(fileHandle, &fileSize)) && fileSize.QuadPart == 0)
    {
        // A blank file is OK. There are no settings to load.
        NtClose(fileHandle);
        return status;
    }

    topNode = mxmlLoadFd(NULL, fileHandle, PhpSettingsLoadCallback);
    NtClose(fileHandle);

    if (!topNode)
        return STATUS_FILE_CORRUPT_ERROR;

    if (topNode->type != MXML_ELEMENT)
    {
        mxmlDelete(topNode);
        return STATUS_FILE_CORRUPT_ERROR;
    }

    currentNode = topNode->child;

    while (currentNode)
    {
        PPH_STRING settingName = NULL;

        if (
            currentNode->type == MXML_ELEMENT &&
            currentNode->value.element.num_attrs >= 1 &&
            _stricmp(currentNode->value.element.attrs[0].name, "name") == 0
            )
        {
            settingName = PhConvertUtf8ToUtf16(currentNode->value.element.attrs[0].value);
        }

        if (settingName)
        {
            PPH_STRING settingValue;

            settingValue = PhGetOpaqueXmlNodeText(currentNode);

            PhAcquireQueuedLockExclusive(&PhSettingsLock);

            {
                PPH_SETTING setting;

                setting = PhpLookupSetting(&settingName->sr);

                if (setting)
                {
                    PhpFreeSettingValue(setting->Type, setting);

                    if (!PhpSettingFromString(
                        setting->Type,
                        &settingValue->sr,
                        settingValue,
                        setting
                        ))
                    {
                        PhpSettingFromString(
                            setting->Type,
                            &setting->DefaultValue,
                            NULL,
                            setting
                            );
                    }
                }
                else
                {
                    setting = PhAllocate(sizeof(PH_SETTING));
                    setting->Name.Buffer = PhAllocateCopy(settingName->Buffer, settingName->Length + sizeof(WCHAR));
                    setting->Name.Length = settingName->Length;
                    PhReferenceObject(settingValue);
                    setting->u.Pointer = settingValue;

                    PhAddItemList(PhIgnoredSettings, setting);
                }
            }

            PhReleaseQueuedLockExclusive(&PhSettingsLock);

            PhDereferenceObject(settingValue);
            PhDereferenceObject(settingName);
        }

        currentNode = currentNode->next;
    }

    mxmlDelete(topNode);

    PhUpdateCachedSettings();

    return STATUS_SUCCESS;
}

char *PhpSettingsSaveCallback(
    _In_ mxml_node_t *node,
    _In_ int position
    )
{
    if (PhEqualBytesZ(node->value.element.name, "setting", TRUE))
    {
        if (position == MXML_WS_BEFORE_OPEN)
            return "  ";
        else if (position == MXML_WS_AFTER_CLOSE)
            return "\r\n";
    }
    else if (PhEqualBytesZ(node->value.element.name, "settings", TRUE))
    {
        if (position == MXML_WS_AFTER_OPEN)
            return "\r\n";
    }

    return NULL;
}

mxml_node_t *PhpCreateSettingElement(
    _Inout_ mxml_node_t *ParentNode,
    _In_ PPH_STRINGREF SettingName,
    _In_ PPH_STRINGREF SettingValue
    )
{
    mxml_node_t *settingNode;
    mxml_node_t *textNode;
    PPH_BYTES settingNameUtf8;
    PPH_BYTES settingValueUtf8;

    // Create the setting element.

    settingNode = mxmlNewElement(ParentNode, "setting");

    settingNameUtf8 = PhConvertUtf16ToUtf8Ex(SettingName->Buffer, SettingName->Length);
    mxmlElementSetAttr(settingNode, "name", settingNameUtf8->Buffer);
    PhDereferenceObject(settingNameUtf8);

    // Set the value.

    settingValueUtf8 = PhConvertUtf16ToUtf8Ex(SettingValue->Buffer, SettingValue->Length);
    textNode = mxmlNewOpaque(settingNode, settingValueUtf8->Buffer);
    PhDereferenceObject(settingValueUtf8);

    return settingNode;
}

NTSTATUS PhSaveSettings(
    _In_ PWSTR FileName
    )
{
    NTSTATUS status;
    HANDLE fileHandle;
    mxml_node_t *topNode;
    PH_HASHTABLE_ENUM_CONTEXT enumContext;
    PPH_SETTING setting;

    topNode = mxmlNewElement(MXML_NO_PARENT, "settings");

    PhAcquireQueuedLockShared(&PhSettingsLock);

    PhBeginEnumHashtable(PhSettingsHashtable, &enumContext);

    while (setting = PhNextEnumHashtable(&enumContext))
    {
        PPH_STRING settingValue;

        settingValue = PhpSettingToString(setting->Type, setting);
        PhpCreateSettingElement(topNode, &setting->Name, &settingValue->sr);
        PhDereferenceObject(settingValue);
    }

    // Write the ignored settings.
    {
        ULONG i;

        for (i = 0; i < PhIgnoredSettings->Count; i++)
        {
            PPH_STRING settingValue;

            setting = PhIgnoredSettings->Items[i];
            settingValue = setting->u.Pointer;
            PhpCreateSettingElement(topNode, &setting->Name, &settingValue->sr);
        }
    }

    PhReleaseQueuedLockShared(&PhSettingsLock);

    // Create the directory if it does not exist.
    {
        PPH_STRING fullPath;
        ULONG indexOfFileName;
        PPH_STRING directoryName;

        fullPath = PhGetFullPath(FileName, &indexOfFileName);

        if (fullPath)
        {
            if (indexOfFileName != -1)
            {
                directoryName = PhSubstring(fullPath, 0, indexOfFileName);
                SHCreateDirectoryEx(NULL, directoryName->Buffer, NULL);
                PhDereferenceObject(directoryName);
            }

            PhDereferenceObject(fullPath);
        }
    }

    status = PhCreateFileWin32(
        &fileHandle,
        FileName,
        FILE_GENERIC_WRITE,
        0,
        FILE_SHARE_READ,
        FILE_OVERWRITE_IF,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
        );

    if (!NT_SUCCESS(status))
    {
        mxmlDelete(topNode);
        return status;
    }

    mxmlSaveFd(topNode, fileHandle, PhpSettingsSaveCallback);
    mxmlDelete(topNode);
    NtClose(fileHandle);

    return STATUS_SUCCESS;
}

VOID PhResetSettings(
    VOID
    )
{
    PH_HASHTABLE_ENUM_CONTEXT enumContext;
    PPH_SETTING setting;

    PhAcquireQueuedLockExclusive(&PhSettingsLock);

    PhBeginEnumHashtable(PhSettingsHashtable, &enumContext);

    while (setting = PhNextEnumHashtable(&enumContext))
    {
        PhpFreeSettingValue(setting->Type, setting);
        PhpSettingFromString(setting->Type, &setting->DefaultValue, NULL, setting);
    }

    PhReleaseQueuedLockExclusive(&PhSettingsLock);
}

VOID PhAddSettings(
    _In_ PPH_SETTING_CREATE Settings,
    _In_ ULONG NumberOfSettings
    )
{
    ULONG i;

    PhAcquireQueuedLockExclusive(&PhSettingsLock);

    for (i = 0; i < NumberOfSettings; i++)
    {
        PH_STRINGREF name;
        PH_STRINGREF defaultValue;

        PhInitializeStringRefLongHint(&name, Settings[i].Name);
        PhInitializeStringRefLongHint(&defaultValue, Settings[i].DefaultValue);
        PhpAddSetting(Settings[i].Type, &name, &defaultValue);
    }

    PhReleaseQueuedLockExclusive(&PhSettingsLock);
}
