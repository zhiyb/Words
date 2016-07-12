//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Editor.xaml.h"
#include <Audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>

using namespace Words;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace concurrency;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Media::Devices;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	viewModel = ref new MainPageViewModel;
	ViewModel->FileOpened = false;
	ViewModel->String = "DUMMY";
	InitializeComponent();
}


void Words::MainPage::openFile(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto picker = ref new FileOpenPicker();
	picker->ViewMode = PickerViewMode::List;
	picker->FileTypeFilter->Append(".json");

	auto op = picker->PickSingleFileAsync();
	create_task(op).then([this] (StorageFile^ file)
	{
		bool valid = file != nullptr;
		if (valid)
			debug->Items->Append(file->Path);
		else
			debug->Items->Append("File open failed");
		ViewModel->FileOpened = valid;
		ViewModel->String = file->Path;
	});
}


void Words::MainPage::editor(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Frame->Navigate(TypeName(Editor::typeid));
}


void Words::MainPage::toggle(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->FileOpened = !ViewModel->FileOpened;
	ViewModel->String += "!";
}
