#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


#include <windows.h>
#include "windowOGL.h"
#include "GameConstants.h"
#include "cWNDManager.h"
#include "cInputMgr.h"
#include "cSoundMgr.h"
#include "cFontMgr.h"
#include "cSprite.h"
#include "asteroidsGame.h"
#include "cButton.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{

    //Set our window settings
    const int windowWidth = 960;
    const int windowHeight = 640;
    const int windowBPP = 16;



    //This is our window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	// This is the input manager
	static cInputMgr* theInputMgr = cInputMgr::getInstance();

	// This is the sound manager
	static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();

	// This is the Font manager
	static cFontMgr* theFontMgr = cFontMgr::getInstance();

	//The example OpenGL code
    windowOGL theOGLWnd;
	
    //Attach our the OpenGL window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	// Attach the keyboard manager
	pgmWNDMgr->attachInputMgr(theInputMgr);

    //Attempt to create the window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
    {
        //If it fails

        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	if (!theOGLWnd.initOGL(windowWidth, windowHeight)) //Initialize our example
    {
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	//Clear key buffers
	theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);

	/* initialize random seed: */
	srand((unsigned int)time(NULL));

	// Create vector array of textures
	LPCSTR texturesToUse[] = { "Images\\fox.png", "Images\\fox.png", "Images\\fox.png", "Images\\fox.png", "Images\\egg.png" };
	for (int tCount = 0; tCount < 5; tCount++)
	{
		theGameTextures.push_back(new cTexture());
		theGameTextures[tCount]->createTexture(texturesToUse[tCount]);
	}

	// load game sounds
	// Load Sound
	LPCSTR gameSounds[2] = { "Audio/bgmusic.wav", "Audio/chickensound.wav" };

	theSoundMgr->add("Theme", gameSounds[0]);
	theSoundMgr->add("Chicken", gameSounds[1]);
	

	// load game fonts
	// Load Fonts
	LPCSTR gameFonts[] = {"Fonts/orange juice 2.0.ttf" };
	theFontMgr->addFont("orange juice 2.0.ttf", gameFonts[0], 36);
	
	// Create vector array of textures for asteroids(foxes)

	for (int astro = 0; astro < 5; astro++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos(glm::vec2(windowWidth / (rand() % 5 + 1), windowHeight / (rand() % 5 + 1)));
		theAsteroids[astro]->setSpriteTranslation(glm::vec2((rand() % 4 + 1), (rand() % 4 + 1)));
		int randAsteroid = rand() % 4;
		theAsteroids[astro]->setTexture(theGameTextures[randAsteroid]->getTexture());
		theAsteroids[astro]->setTextureDimensions(theGameTextures[randAsteroid]->getTWidth()/4, theGameTextures[randAsteroid]->getTHeight()/4);
		theAsteroids[astro]->setSpriteCentre();
		theAsteroids[astro]->setAsteroidVelocity(glm::vec2(glm::vec2(0.0f, 0.0f)));
		theAsteroids[astro]->setActive(true);
		theAsteroids[astro]->setMdlRadius();
	}



	//vector array for background textures
	vector<cTexture*> textureBkgList;
	LPCSTR bkgTexturesToUse[] = { "Images\\farmbg.png", "Images\\farmStartbg.png", "Images\\farmEndbg.png" };
	for (int tCount = 0; tCount < 4; tCount++)
	{
		textureBkgList.push_back(new cTexture());
		textureBkgList[tCount]->createTexture(bkgTexturesToUse[tCount]);
	}
	//create individual instances of each background
	cBkGround spriteBkgd;
	spriteBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteBkgd.setTexture(textureBkgList[0]->getTexture());
	spriteBkgd.setTextureDimensions(textureBkgList[0]->getTWidth(), textureBkgList[0]->getTHeight());

	cBkGround spriteStartBkgd;
	spriteStartBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteStartBkgd.setTexture(textureBkgList[1]->getTexture());
	spriteStartBkgd.setTextureDimensions(textureBkgList[1]->getTWidth(), textureBkgList[1]->getTHeight());

	cBkGround spriteEndBkgd;
	spriteEndBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteEndBkgd.setTexture(textureBkgList[2]->getTexture());
	spriteEndBkgd.setTextureDimensions(textureBkgList[2]->getTWidth(), textureBkgList[2]->getTHeight());

	//Vector array for button textures
	vector<cTexture*> btnTextureList;
	LPCSTR btnTexturesToUse[] = { "Images\\exitBtn.png", "Images\\playBtn.png" };
	for (int tCount = 0; tCount < 3; tCount++)
	{
		btnTextureList.push_back(new cTexture());
		btnTextureList[tCount]->createTexture(btnTexturesToUse[tCount]);
	}
	//create individual instances of each button
	cButton exitButton;
	exitButton.attachInputMgr(theInputMgr);
	exitButton.setTexture(btnTextureList[0]->getTexture());
	exitButton.setTextureDimensions(btnTextureList[0]->getTWidth(), btnTextureList[0]->getTHeight());

	cButton playButton;
	playButton.attachInputMgr(theInputMgr);
	playButton.setTexture(btnTextureList[1]->getTexture());
	playButton.setTextureDimensions(btnTextureList[1]->getTWidth(), btnTextureList[1]->getTHeight());

	//attach texture to rocket(chicken) object
	cTexture rocketTxt;
	rocketTxt.createTexture("Images\\chicken.png");
	cRocket rocketSprite;
	rocketSprite.attachInputMgr(theInputMgr); 
	// Attach the input manager to the sprite
	rocketSprite.setSpritePos(glm::vec2(512.0f, 380.0f));
	rocketSprite.setTexture(rocketTxt.getTexture());
	rocketSprite.setTextureDimensions(rocketTxt.getTWidth() / 4, rocketTxt.getTHeight() / 4);
	rocketSprite.setSpriteCentre();
	rocketSprite.setRocketVelocity(glm::vec2(0.0f, 0.0f));

	// Attach sound manager to rocket sprite
	rocketSprite.attachSoundMgr(theSoundMgr);

	//create string output array
	string outputMsg;
	string strMsg[] = { "Chicken Run", "Score:", "Use the Arrow Keys to move", "Spacebar to Fire", "Get the foxes!" };

	gameState theGameState = MENU;
	btnTypes theBtnType = EXIT;


    //This is the mainloop, we render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
    {
		pgmWNDMgr->processWNDEvents(); //Process any window events

        //We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//method for switching between menus(game states); MENU, PLAYING, END
		switch (theGameState)
		{
		case MENU: 
		{
			//render the menu background
			spriteStartBkgd.render();
			//set button position then render
			playButton.setSpritePos(glm::vec2(400.0f, 500.0f));
			exitButton.setSpritePos(glm::vec2(500.0f, 500.0f));
			playButton.render();
			exitButton.render();
			//when the play button is clicked, game state while switch to playing state
			theGameState = playButton.update(theGameState, PLAYING);
			exitButton.update();
			//print text stored in array 
			outputMsg = strMsg[2];
			theFontMgr->getFont("orange juice 2.0.ttf")->printText(outputMsg.c_str(), FTPoint(300.0f, 250.0f, 0.0f));
			outputMsg = strMsg[3];
			theFontMgr->getFont("orange juice 2.0.ttf")->printText(outputMsg.c_str(), FTPoint(300.0f, 350.0f, 0.0f));
			outputMsg = strMsg[4];
			theFontMgr->getFont("orange juice 2.0.ttf")->printText(outputMsg.c_str(), FTPoint(300.0f, 450.0f, 0.0f));

			//Play theme music
			theSoundMgr->getSnd("Theme")->playAudio(AL_TRUE);
			//if exit button is clicked, the application will close
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}
		}
		
			break;

		case PLAYING:
			//render the playing background
		{spriteBkgd.render();

			rocketSprite.update(elapsedTime);
			//iterates the asteroids(foxes)
			{vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
			while (asteroidIterator != theAsteroids.end())
			{
				if ((*asteroidIterator)->isActive() == false)
				{
					{asteroidIterator = theAsteroids.erase(asteroidIterator); }
				}
				else
				{
					{(*asteroidIterator)->update(elapsedTime); (*asteroidIterator)->render(); ++asteroidIterator; }
				}
			}
			}
			//renders the rocket(chicken) and prints text stored in array
		rocketSprite.render();
		outputMsg = strMsg[0];
		theFontMgr->getFont("orange juice 2.0.ttf")->printText(outputMsg.c_str(), FTPoint(0.0f, 50.0f, 0.0f));
		outputMsg = strMsg[1];
		theFontMgr->getFont("orange juice 2.0.ttf")->printText(outputMsg.c_str(), FTPoint(450.0f, 50.0f, 0.0f));

		//Play theme music
		theSoundMgr->getSnd("Theme")->playAudio(AL_TRUE);
		//sets button position and renders. When activated, switches game to end state
		exitButton.setSpritePos(glm::vec2(740.0f, 600.0f));
		exitButton.render();
		theGameState = exitButton.update(theGameState, END);

		}
			break;

		case END:
			//renders end game background
		{	spriteEndBkgd.render();
		//postion and renders buttons
			playButton.setClicked(false);
			exitButton.setClicked(false);

			playButton.setSpritePos(glm::vec2(400.0f, 500.0f));
			exitButton.setSpritePos(glm::vec2(500.0f, 500.0f));
			playButton.render();
			exitButton.render();

			//Play theme music
			theSoundMgr->getSnd("Theme")->playAudio(AL_TRUE);
			//if the play button is activated, game state will switch to playing
			theGameState = playButton.update(theGameState, PLAYING);
			exitButton.update();
			//if exit button is activated, the application will close
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}
		}
			break;

		}

		//clears all key buffers
		pgmWNDMgr->swapBuffers();
		theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);
    }

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

    return 0; //Return success
}
