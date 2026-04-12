#include <ROMV2_APP.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_APP::ROMV2_APP()
{
}

/// <summary>
/// Initialisation de l'App
/// </summary>
void ROMV2_APP::Init()
{
	// Positionnement des PIN
	pinMode(JOYSTICK_PIN_SWITCH, INPUT_PULLUP);
	digitalWrite(JOYSTICK_PIN_SWITCH, HIGH);
	
	// Initialisation Wire avec définition des PIN SDA/SCL
	Wire.setPins(ESP32_GPIO_SDA, ESP32_GPIO_SCL);

	// Initialisation des objets représentant les capteurs et périphériques
	_tft.Init(&_dataEnvironment, &_dataLuminosity, &_dataSkyState, &_dataAcceleration, &_dataGPS, &_bluetoothConnected);
	_bme280.Init(&_dataEnvironment);
	_tsl2591.Init(&_tft, &_dataLuminosity);
	_mlx90614.Init(&_dataSkyState);
	_adxl345.Init(&_dataAcceleration);
	_gps.Init(&_dataGPS);
	_bluetoothLE.Init(&_bluetoothConnected, &_dataEnvironment, &_dataLuminosity, &_dataSkyState);

	// Initialisation Chronos
	_chronoReadJoystick = 0;
	_chronoJoystickButtonPressed = 0;
	_chronoDisplayTimeout = millis();

	// Initialisation des performances pour les traces
	InitAppPerformances();
}

/// <summary>
/// Processus complet a exécuter lors d'un Loop
/// </summary>
void ROMV2_APP::Loop()
{
	// Lecture de l'état du Joystick
	ReadJoystickState();

	// Lecture température, humidité, pression atm. et point de rosée
	_bme280.ReadEnvironment();

	// Lecture Luminosité si l'affichage courant est différent de DISPLAY_ACCELERATION
	if (_currentDisplayScreenType != DISPLAY_ACCELERATION)
	{
		_tsl2591.ReadLuminosity();
	}

	// Lecture Température IR
	_mlx90614.ReadIRTemp();

	// Lecture Accélération uniquement si l'affichage courant est DISPLAY_ACCELERATION
	if (_currentDisplayScreenType == DISPLAY_ACCELERATION)
	{
		_adxl345.ReadAcceleration();
	}
  
	// Lecture des données GPS et vérification de l'état du GPS
	_gps.ReadGPS();
	_gps.CheckGPSState();

	// Envoi des notifications des caractéristiques du BLE (Bluetooth Low Energy)
	_bluetoothLE.Notify();

	// Actualisation du display
	UpdateCurrentDisplay();
	_tft.SetCurrentDisplay(_currentDisplayScreenType, _displayHomeType);
	_tft.UpdateDisplay();

	// Trace des performances
	TraceAppPerformances();
}

/// <summary>
/// Lecture de l'état du Joystick
/// </summary>
void ROMV2_APP::ReadJoystickState()
{
	// Lecture de l'état du Joystick sur intervalle
	if (millis() > _chronoReadJoystick + JOYSTICK_READ_INTERVAL)
	{
		// Bouton du Joystick
		if (digitalRead(JOYSTICK_PIN_SWITCH) == LOW)
		{
			// Actualisation Chrono et Flag
			if (!_joystickPressed)
			{
				_chronoJoystickButtonPressed = millis();
			}
			_joystickPressed = true;

			// Clic long
			if (millis() > _chronoJoystickButtonPressed + JOYSTICK_BUTTON_LONG_INTERVAL)
			{
				_currentAction = ACTION_CLICK_LONG;
				// Actualisation des Flags
				_joystickPressed = false;
				// Trace
				debugln(F(""));
				debugln(F("[ACTION] Long Click"));
			}
		}
		// Pour le bouton Joystick, on traite le clic court sur relachement du bouton
		else
		{
			if (_joystickPressed)
			{
				_currentAction = ACTION_CLICK;
				debugln(F(""));
				debugln(F("[ACTION] Click"));
			}
			// Actualisation des Flags
			_joystickPressed = false;
		}

		// Position du Joystick :
		// - Pas de lecture si le bouton du Joystick est pressé, afin de réduire les miss clicks
		if (!_joystickPressed)
		{
			int x = analogRead(JOYSTICK_PIN_X);
			int y = analogRead(JOYSTICK_PIN_Y);

			if (y > JOYSTICK_POSITION_MIN_VALUE_HIGH && !_joystickMoved)
			{
				_joystickMoved = true;
				_currentAction = ACTION_RIGHT;
				debugln(F(""));
				debugln(F("[ACTION] Right"));
			}
			else if (y < JOYSTICK_POSITION_MIN_VALUE_LOW && !_joystickMoved)
			{
				_joystickMoved = true;
				_currentAction = ACTION_LEFT;
				debugln(F(""));
				debugln(F("[ACTION] Left"));
			}
			else if (x > JOYSTICK_POSITION_MIN_VALUE_HIGH && !_joystickMoved)
			{
				_joystickMoved = true;
				_currentAction = ACTION_UP;
				debugln(F(""));
				debugln(F("[ACTION] Up"));
			}
			else if (x < JOYSTICK_POSITION_MIN_VALUE_LOW && !_joystickMoved)
			{
				_joystickMoved = true;
				_currentAction = ACTION_DOWN;
				debugln(F(""));
				debugln(F("[ACTION] Down"));
			}
			else
			{
				if (x < JOYSTICK_POSITION_MIN_VALUE_HIGH
					&& x > JOYSTICK_POSITION_MIN_VALUE_LOW
					&& y < JOYSTICK_POSITION_MIN_VALUE_HIGH
					&& y > JOYSTICK_POSITION_MIN_VALUE_LOW)
					_joystickMoved = false;
				// String trace = "X : " + String(x) + " / Y : " + String(y);
				// debugln(trace);
			}
		}

		// Si une action est en cours, on force la mise à jour complète de l'affichage
		if (_currentAction != ACTION_NONE)
		{
			_tft.ForceRedraw();
		}

		// Actualisation Chrono
		_chronoReadJoystick = millis();
	}
}

/// <summary>
/// Actualise l'affichage en cours sur action du Joystick
/// </summary>
void ROMV2_APP::UpdateCurrentDisplay()
{
	// Gestion de la mise en veille de l'affichage
	if (_currentAction == ACTION_NONE && millis() > _chronoDisplayTimeout + SCREEN_TIMEOUT)
	{
		_currentDisplayScreenType = DISPLAY_NONE;
	}

	// On ne fait rien si pas d'actions en cours
	if (_currentAction != ACTION_NONE)
	{
		// Si l'affichage est en mode veille, on le sort de la veille
		if (_currentDisplayScreenType == DISPLAY_NONE)
		{
			_currentDisplayScreenType = DISPLAY_HOME;
		}
		// Sinon, on positionne l'affichage en fonction de l'action en cours
		else
		{
			// En fonction de l'action en cours
			switch (_currentAction)
			{
				case ACTION_RIGHT:
					if (_currentDisplayScreenType == DISPLAY_GPS)
					{
						_currentDisplayScreenType = DISPLAY_HOME;
					}
					else
					{
						_currentDisplayScreenType = static_cast<DISPLAY_SCREEN_TYPE>(static_cast<int>(_currentDisplayScreenType) + 1);
					}
					break;

				case ACTION_LEFT:
					if (_currentDisplayScreenType == DISPLAY_HOME)
					{
						_currentDisplayScreenType = DISPLAY_GPS;
					}
					else
					{
						_currentDisplayScreenType = static_cast<DISPLAY_SCREEN_TYPE>(static_cast<int>(_currentDisplayScreenType) - 1);
					}
					break;

				case ACTION_CLICK:
					if (_currentDisplayScreenType == DISPLAY_HOME)
					{
						if (_displayHomeType == DISPLAY_HOME_SQM)
						{
							_displayHomeType = DISPLAY_HOME_LUX;
						}
						else
						{
							_displayHomeType = DISPLAY_HOME_SQM;
						}
					}
					else if (_currentDisplayScreenType == DISPLAY_LUX)
					{
						_tsl2591.ClearLuxAverage();
					}
					break;

				case ACTION_CLICK_LONG:
					_currentDisplayScreenType = DISPLAY_NONE;
					break;

				case ACTION_UP:
				case ACTION_DOWN:
					_currentDisplayScreenType = DISPLAY_HOME;
					break;

				case ACTION_NONE:
				default:
					break;
			}
		}

		// Trace
		debugln(F(""));
		debug(F("[DISPLAY] ScreenType: "));
		debug(_currentDisplayScreenType);
		debug(F(" / HomeType: "));
		debugln(_displayHomeType);

		// RAZ de l'état de l'action en cours
		_currentAction = ACTION_NONE;
		// Sur action, RAZ du compteur de veille en cours
		_chronoDisplayTimeout = millis();
	}
}
