/*
Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

/**
 * @file ReloadScreenController.cpp
 *
 *  Created on: Feb 27, 2012
 *      Author: Iraklis Rossis
 */

#include "ReloadClient.h"
#include "ReloadScreenController.h"
#include "View/MainStackScreen.h"
#include "View/ReloadTabScreen.h"
#include "View/ConnectionScreen.h"
#include "View/WorkspaceScreen.h"

using namespace MAUtil; // Class Moblet
using namespace NativeUI; // WebView widget.

/**
 * Constructor.
 * @param client The ReloadClient that will handle all the reload business logic.
 */
ReloadScreenController::ReloadScreenController(ReloadClient *client) :
		mLoginScreen(NULL),
		mReloadTabScreen(NULL),
		mConnectionScreen(NULL),
		mWorkspaceScreen(NULL)
{
	mReloadClient = client;
}

ReloadScreenController::~ReloadScreenController()
{
	mLoginScreen->removeReloadUIListener(this);
}

/**
 * Creates the screen, the layouts, the widgets and positions everything.
 * @param os A string containing the current os.
 * @param orientation One of the values:
 * 		MA_SCREEN_ORIENTATION_LANDSCAPE_LEFT
 * 		MA_SCREEN_ORIENTATION_LANDSCAPE_RIGHT
 * 		MA_SCREEN_ORIENTATION_PORTRAIT
 * 		MA_SCREEN_ORIENTATION_PORTRAIT_UPSIDE_DOWN
 */
void ReloadScreenController::initializeScreen(MAUtil::String &os, int orientation)
{
	mOS = os;

	mLoginScreen = new LoginScreen(os, orientation);
	mLoginScreen->addReloadUIListener(this);

//	MainStackScreen::getInstance()->push(mLoginScreen);
//	MainStackScreen::getInstance()->show();
	mLoginScreen->setVisible(true);
	mLoginScreen->show();
}

/**
 * Called when the client has connected to the server.
 * @param serverAddress The server IP address.
 */
void ReloadScreenController::connectedTo(const char *serverAddress)
{
	//Success, show the disconnect controls
	String conTo = "Connected to: ";
	conTo += serverAddress;

	showTabScreen(true);
	mConnectionScreen->fillConnectionData(conTo.c_str());
}

/**
 * Called when the client has disconnected from the server.
 */
void ReloadScreenController::disconnected()
{
	showTabScreen(false);
}

/**
 * Show the login screen in the connected state
 * with the "connected" controls visible.
 */
void ReloadScreenController::showConnectedScreen()
{
	showTabScreen(true);
}

/**
 * Show the login screen in the not connected state.
 */
void ReloadScreenController::showNotConnectedScreen()
{
	showTabScreen(false);
}

/**
 * Sets the default address (will appear inside the connect EditBox).
 * @param serverAddress The default server address.
 */
void ReloadScreenController::defaultAddress(const char *serverAddress)
{
	mLoginScreen->setDefaultIPAddress(serverAddress);
}

/**
 * Checks if the login screen is visible.
 * @return true if the login screen is visible, false otherwise.
 */
bool ReloadScreenController::loginScreenVisible()
{
	return mLoginScreen->isVisible();
}

/**
 * Shows or hides the reload tab screen (containing the connection screen and the
 * workspace screen).
 * @param show If true, the reload tab screen is pushed into the main stack screen and poped
 * 	otherwise.
 */
void ReloadScreenController::showTabScreen(bool show)
{
	if (show)
	{
		if (mReloadTabScreen == NULL)
		{
			mReloadTabScreen = new ReloadTabScreen();

			int orientation = maScreenGetCurrentOrientation();
			mConnectionScreen = new ConnectionScreen(mOS, orientation);
			mConnectionScreen->setTitle("Reload");
			mConnectionScreen->addReloadUIListener(this);
			mReloadTabScreen->addTab(mConnectionScreen);

			mWorkspaceScreen = new WorkspaceScreen();
			mWorkspaceScreen->setTitle("Workspace");
			mReloadTabScreen->addTab(mWorkspaceScreen);

			mReloadTabScreen->setActiveTab(0);
		}

		mReloadTabScreen->show();
		mLoginScreen->setVisible(false);
		lprintfln("TAB SCREEN VISIBLE!!!!!!!!!!!!!!!!!!!");
	//	MainStackScreen::getInstance()->push(mReloadTabScreen);
	}
	else
	{
		mLoginScreen->setVisible(true);
		mLoginScreen->show();
	//	MainStackScreen::getInstance()->pop();
	}
}

/**
 * Called when the connect button is clicked.
 * @param address The address contained by the connect EditBox.
 */
void ReloadScreenController::connectButtonClicked(String address)
{
	mReloadClient->connectToServer(address.c_str());
}

/**
 * Called when the disconnect button is clicked.
 */
void ReloadScreenController::disconnectButtonClicked()
{
	mReloadClient->disconnectFromServer();
}

/**
 * Called when the reload last app button is clicked.
 */
void ReloadScreenController::reloadLastAppButtonClicked()
{
	//Just load whatever app we have already extracted
	mReloadClient->launchSavedApp();
}

/**
 * Called when the info button is clicked.
 */
void ReloadScreenController::infoButtonClicked()
{
	//Show the info screen
	maMessageBox("Reload Client Info",mReloadClient->getInfo().c_str());
}