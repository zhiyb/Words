//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace Words
{
	using namespace Platform;
	using namespace Windows::UI::Xaml::Data;

	[BindableAttribute]
	public ref class MainPageViewModel sealed: public INotifyPropertyChanged
	{
	public:
		event virtual PropertyChangedEventHandler^ PropertyChanged;

		property bool FileOpened
		{
			bool get() { return fileOpened; }
			void set(bool e) { fileOpened = e; propertyChanged("FileOpened"); }
		}
		property Platform::String^ String
		{
			Platform::String^ get() { return string; }
			void set(Platform::String^ str) { string = str; propertyChanged("String"); }
		}

	private:
		void propertyChanged(Platform::String^ name = nullptr)
		{
			PropertyChanged(this, ref new PropertyChangedEventArgs(name));
		}

		bool fileOpened;
		Platform::String^ string;
	};

	public ref class MainPage sealed
	{
	public:
		MainPage();

		property MainPageViewModel^ ViewModel
		{
			MainPageViewModel^ get() { return viewModel; }
		}

	private:
		void openFile(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void editor(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void toggle(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		MainPageViewModel ^viewModel;
	};
}
