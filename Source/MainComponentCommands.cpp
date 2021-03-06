/*
 ==============================================================================

 MainComponentMenuBar.cpp
 Created: 25 Mar 2016 6:02:02pm
 Author:  Martin Hermant

 ==============================================================================
 */


#include "MainComponent.h"
#include "UI/AboutWindow.h"

namespace ChataigneCommandIDs
{
	static const int showAbout = 0x60000;
	static const int gotoWebsite = 0x60001;
	static const int gotoForum = 0x60002;
	static const int gotoDocs = 0x60003;
	static const int postGithubIssue = 0x60004;
	static const int donate = 0x60005;

}


void MainContentComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) {
	const String category("General");


	switch (commandID)
	{
	case ChataigneCommandIDs::showAbout:
		result.setInfo("About...", "", category, 0);
		break;

	case ChataigneCommandIDs::donate:
		result.setInfo("Be cool and donate", "", category, 0);
		break;

	case ChataigneCommandIDs::gotoWebsite:
		result.setInfo("Go to website", "", category, 0);
		break;
	case ChataigneCommandIDs::gotoForum:
		result.setInfo("Go to forum", "", category, 0);
		break;

	case ChataigneCommandIDs::gotoDocs:
		result.setInfo("Go to the Amazing Documentation", "", category, 0);
		break;

	case ChataigneCommandIDs::postGithubIssue:
		result.setInfo("Post an issue on github", "", category, 0);
		break;

	default:
		OrganicMainContentComponent::getCommandInfo(commandID, result);
		break;
	}
}


void MainContentComponent::getAllCommands(Array<CommandID>& commands) {

	OrganicMainContentComponent::getAllCommands(commands);

	const CommandID ids[] = {
	
		ChataigneCommandIDs::showAbout,
		ChataigneCommandIDs::donate,
	  ChataigneCommandIDs::gotoWebsite,
	  ChataigneCommandIDs::gotoForum,
	  ChataigneCommandIDs::gotoDocs,
	  ChataigneCommandIDs::postGithubIssue
	};

	commands.addArray(ids, numElementsInArray(ids));
}


PopupMenu MainContentComponent::getMenuForIndex(int topLevelMenuIndex, const String& menuName) 
{
	PopupMenu menu = OrganicMainContentComponent::getMenuForIndex(topLevelMenuIndex, menuName);


	if (menuName == "Help")
	{
		menu.addCommandItem(&getCommandManager(), ChataigneCommandIDs::showAbout);
		menu.addCommandItem(&getCommandManager(), ChataigneCommandIDs::donate);
		menu.addSeparator();
		menu.addCommandItem(&getCommandManager(), ChataigneCommandIDs::gotoWebsite);
		menu.addCommandItem(&getCommandManager(), ChataigneCommandIDs::gotoForum);
		menu.addCommandItem(&getCommandManager(), ChataigneCommandIDs::gotoDocs);
		menu.addCommandItem(&getCommandManager(), ChataigneCommandIDs::postGithubIssue);
	}

	return menu;
}

bool MainContentComponent::perform(const InvocationInfo& info) {

	switch (info.commandID)
	{


	case ChataigneCommandIDs::showAbout:
	{
		AboutWindow w;
		DialogWindow::showModalDialog("About", &w, getTopLevelComponent(), Colours::transparentBlack, true);
	}
	break;

	case ChataigneCommandIDs::donate:
		URL("https://www.paypal.me/benkuper").launchInDefaultBrowser();
		break;

	case ChataigneCommandIDs::gotoWebsite:
		URL("http://benjamin.kuperberg.fr/chataigne").launchInDefaultBrowser();
		break;

	case ChataigneCommandIDs::gotoForum:
		URL("http://benjamin.kuperberg.fr/chataigne/forum").launchInDefaultBrowser();
		break;

	case ChataigneCommandIDs::gotoDocs:
		URL("http://benjamin.kuperberg.fr/chataigne/docs").launchInDefaultBrowser();
		break;

	case ChataigneCommandIDs::postGithubIssue:
		URL("http://github.com/benkuper/Chataigne/issues").launchInDefaultBrowser();
		break;

	default:
		return OrganicMainContentComponent::perform(info);
	}

	return true;
}

StringArray MainContentComponent::getMenuBarNames()
{
	StringArray names = OrganicMainContentComponent::getMenuBarNames();
	names.add("Help");
	return names;
}
