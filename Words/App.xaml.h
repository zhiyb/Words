//
// App.xaml.h
// Declaration of the App class.
//

#pragma once

#include "App.g.h"

namespace Words
{
	using namespace Windows::UI::Xaml::Navigation;

	/// <summary>
	/// Provides application-specific behavior to supplement the default Application class.
	/// </summary>
	ref class App sealed
	{
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		App();

	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object^ sender, NavigationFailedEventArgs^ e);
		void OnNavigated(Platform::Object^ sender, NavigationEventArgs^ e);
		void OnBackRequested(Platform::Object ^sender, Windows::UI::Core::BackRequestedEventArgs ^args);
	};
}
