// FileBookmark
// Copyright 2023, SGrottel
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "CmdLineOptions.h"
#include "Registation.h"
#include "CallElevated.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#if defined _WIN64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#include <Commctrl.h>

#include <stdexcept>
#include <sstream>

namespace
{
	static const wchar_t* appName = L"FileBookmark";
}

void AskForAction(HINSTANCE hInstance);
void RegisterFileType();
void UnregisterFileType();
void OpenBookmarkFile(std::wstring const& filepath);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	using filebookmark::CmdLineOptions;

	SetProcessDPIAware();
	CmdLineOptions cmdLine;
	try {
		cmdLine.Parse(pCmdLine);

		switch (cmdLine.GetMode())
		{
		case CmdLineOptions::Mode::None:
			AskForAction(hInstance);
			break;

		case CmdLineOptions::Mode::RegisterFileType:
			RegisterFileType();
			break;

		case CmdLineOptions::Mode::UnregisterFileType:
			UnregisterFileType();
			break;

		case CmdLineOptions::Mode::OpenBookmark:
			if (cmdLine.GetBookmarkFile().empty())
			{
				throw std::runtime_error{"Bookmark file not specified"};
			}
			OpenBookmarkFile(cmdLine.GetBookmarkFile());
			break;

		default:
			std::string msg{ "Unsupported `Mode` encountered after parsing the application's command line: " };
			msg += std::to_string(static_cast<int>(cmdLine.GetMode()));
			throw std::logic_error(msg);
		}

	}
	catch (std::exception const& ex)
	{
		std::wstringstream msg;
		msg << L"An unrecoverable error was encountered:\n" << ex.what();

		MessageBoxW(nullptr, msg.str().c_str(), appName, MB_ICONERROR | MB_OK);
	}

	return 0;
}

void AskForAction(HINSTANCE hInstance)
{
	int buttonPressed = 0;

	TASKDIALOGCONFIG config = { 0 };

	std::wstring regText{L"Register \".bookmark\" File Type"};
	std::wstring unregText{L"Unregister \".bookmark\" File Type"};

	if (!filebookmark::Registation::HasAccess())
	{
		std::wstring warning;
		if (filebookmark::CallElevated::IsElevated())
		{
			warning = L"\nAccess rights seem to be restricted. Operation might fail.";
		}
		else
		{
			warning = L"\nElevated access rights will be requested.";
		}
		regText += warning;
		unregText += warning;
	}

	const TASKDIALOG_BUTTON buttons[] = {
		{ 100, regText.c_str() },
		{ 101, unregText.c_str() },
		{ 102, L"Open \".bookmark\" File..." },
		{ 103, L"Create a \".bookmark\" on a File..." }
	};

	config.cbSize = sizeof(config);
	config.hInstance = hInstance;
	config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
	config.dwFlags = TDF_USE_COMMAND_LINKS | TDF_USE_HICON_MAIN;
	config.pszWindowTitle = appName;
	config.hMainIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(100));
	config.pszMainInstruction = L"Bookmark File Type Registration";
	config.pszContent = L"You did not open a bookmark file. In this mode, you can register the file type.";
	config.nDefaultButton = IDCANCEL;
	config.pButtons = buttons;
	config.cButtons = ARRAYSIZE(buttons);

	HRESULT res = TaskDialogIndirect(&config, &buttonPressed, nullptr, nullptr);
	if (res != S_OK)
	{
		throw std::runtime_error{"Failed to open UI Dialog: " + std::to_string(static_cast<int>(res))};
	}

	switch (buttonPressed)
	{
	case 100:
		RegisterFileType();
		break;
	case 101:
		UnregisterFileType();
		break;
	//case 102:
	//	break;
	//case 103:
	//	break;
	case IDCANCEL:
		break;
	default:
		throw std::logic_error{"Handling of UI Dialog result not implemented: " + std::to_string(buttonPressed)};
	}
}

void RegisterFileType()
{
	using filebookmark::Registation;
	using filebookmark::CallElevated;
	if (!Registation::HasAccess() && !CallElevated::IsElevated())
	{
		CallElevated c;
		c.ReCallAs(L"--reg");
		return;
	}

	Registation reg;
	reg.Register();

	MessageBoxW(nullptr, L"Successfully registered the file type `.bookmark`.", appName, MB_OK);
}

void UnregisterFileType()
{
	using filebookmark::Registation;
	using filebookmark::CallElevated;
	if (!Registation::HasAccess() && !CallElevated::IsElevated())
	{
		CallElevated c;
		c.ReCallAs(L"--unreg");
		return;
	}

	Registation reg;
	reg.Unregister();

	MessageBoxW(nullptr, L"Successfully unregistered the file type `.bookmark`.", appName, MB_OK);
}

void OpenBookmarkFile(std::wstring const& filepath)
{
	throw std::logic_error{"Not Implemented"};
}
