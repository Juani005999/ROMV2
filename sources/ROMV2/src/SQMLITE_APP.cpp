#include <SQMLITE_APP.h>

/// <summary>
/// Constructeur
/// </summary>
SQMLITE_APP::SQMLITE_APP()
{
}

/// <summary>
/// Initialisation des capteurs propres au modèle complet.
/// </summary>
void SQMLITE_APP::InitModelSensors()
{
	// Initialisation des membres
	_tft = new SQMLITE_TFT();

	// Initiaisation du type d'application en cours
	_appType = APP_SQMLITE;

	// Initialisation des objets représentant les capteurs et périphériques
	_tft->Init(_appType, &_dataEnvironment, &_dataLuminosity, &_dataSkyState, &_dataAcceleration, &_dataGPS, &_dataWifi, &_dataMqtt, &_bluetoothConnected, &_networkMode);
	_bme280.Init(&_dataEnvironment);
	_tsl2591.Init(_tft, &_dataLuminosity, &_dataEnvironment);
	if (_networkMode == NETWORK_BLE)
	{
		_bluetoothLE.Init(_appType, &_bluetoothConnected, &_dataEnvironment, &_dataLuminosity, &_dataSkyState);
	}
	if (_networkMode == NETWORK_WIFI)
	{
		_wifi.Init(_appType, &_dataWifi, &_dataMqtt);
		_mqtt.Init(_appType, &_dataWifi, &_dataMqtt, &_dataEnvironment, &_dataLuminosity, &_dataSkyState, &_dataAcceleration, &_dataGPS);
	}
}

/// <summary>
/// Lectures cadencées des capteurs propres au modèle complet.
/// </summary>
void SQMLITE_APP::LoopModelSensors()
{
	// Lecture température, humidité, pression atm. et point de rosée
	_bme280.ReadEnvironment();

	// Lecture Luminosité
	_tsl2591.ReadLuminosity();
}

/// <summary>
/// Lecture de l'état du Joystick
/// </summary>
void SQMLITE_APP::ReadJoystickState()
{
	// Lecture de l'état du Joystick sur intervalle
	if (millis() > _chronoReadJoystick + JOYSTICK_READ_INTERVAL)
	{
		bool addChronoInterval = true;

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
			if (_joystickPressed && _currentDisplayScreenType != DISPLAY_NONE)
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

			if (y > SQMLITE_JOYSTICK_POSITION_MIN_VALUE_HIGH && !_joystickMoved)
			{
				_joystickMoved = true;
				_currentAction = ACTION_LEFT;
				debugln(F(""));
				debugln(F("[ACTION] Left"));
			}
			else if (y < SQMLITE_JOYSTICK_POSITION_MIN_VALUE_LOW && !_joystickMoved)
			{
				_joystickMoved = true;
				_currentAction = ACTION_RIGHT;
				debugln(F(""));
				debugln(F("[ACTION] Right"));
			}
			else if (x > SQMLITE_JOYSTICK_POSITION_MIN_VALUE_HIGH)
			{
				// Update des commandes uniquement si pas de mouvement en cours
				if (!_joystickMoved)
				{
					_currentAction = ACTION_DOWN;
					_joystickMoved = true;
					_chronoJoystickButtonMoved = millis();

					// Trace
					debugln(F(""));
					debugln(F("[ACTION] Down"));
				}
				// Sur l'écran Calibration et sur mouvement long, on update l'Action en cours afin d'accélérer la décrémentation
				if (_currentDisplayScreenType == DISPLAY_TSL2591_CALIBRATION && millis() > _chronoJoystickButtonMoved + JOYSTICK_MOVE_LONG_INTERVAL)
				{
					_currentAction = ACTION_DOWN;
					_chronoJoystickButtonMoved += 50;
					addChronoInterval = false;

					// Trace
					debugln(F(""));
					debugln(F("[ACTION] Down"));
				}
			}
			else if (x < SQMLITE_JOYSTICK_POSITION_MIN_VALUE_LOW)
			{
				// Update des commandes uniquement si pas de mouvement en cours
				if (!_joystickMoved)
				{
					_currentAction = ACTION_UP;
					_joystickMoved = true;
					_chronoJoystickButtonMoved = millis();

					// Trace
					debugln(F(""));
					debugln(F("[ACTION] Up"));
				}
				// Sur l'écran Calibration et sur mouvement long, on update l'Action en cours afin d'accélérer l'incrémentation
				if (_currentDisplayScreenType == DISPLAY_TSL2591_CALIBRATION && millis() > _chronoJoystickButtonMoved + JOYSTICK_MOVE_LONG_INTERVAL)
				{
					_currentAction = ACTION_UP;
					_chronoJoystickButtonMoved += 50;
					addChronoInterval = false;

					// Trace
					debugln(F(""));
					debugln(F("[ACTION] Up"));
				}
			}
			else
			{
				if (x < SQMLITE_JOYSTICK_POSITION_MIN_VALUE_HIGH
					&& x > SQMLITE_JOYSTICK_POSITION_MIN_VALUE_LOW
					&& y < SQMLITE_JOYSTICK_POSITION_MIN_VALUE_HIGH
					&& y > SQMLITE_JOYSTICK_POSITION_MIN_VALUE_LOW)
				{
					_joystickMoved = false;
					_chronoJoystickButtonMoved = millis();
				}
			}
		}

		// Si une action est en cours, on force la mise à jour complète de l'affichage
		if (_currentAction != ACTION_NONE)
		{
			if (_currentDisplayScreenType != DISPLAY_TSL2591_CALIBRATION
				|| (_currentAction != ACTION_UP && _currentAction != ACTION_DOWN))
			{
				_tft->ForceRedraw();
			}
		}

		// Actualisation Chrono et Flag
		if (addChronoInterval)
		{
			_chronoReadJoystick = millis();
		}
	}
}

/// <summary>
/// Ordre complet des écrans navigables au joystick pour le modèle SQMLITE_APP.
/// L'écran WiFi reste filtré à l'exécution par IsScreenVisible (présent seulement en mode WiFi).
/// </summary>
const DISPLAY_SCREEN_TYPE* SQMLITE_APP::GetScreenOrder(uint8_t& count) const
{
	static const DISPLAY_SCREEN_TYPE order[] =
	{
		DISPLAY_HOME,
		DISPLAY_LUX,
		DISPLAY_ENVIRONMENT,
		DISPLAY_NETWORK,
		DISPLAY_WIFI,
		DISPLAY_TSL2591_CALIBRATION
	};

	count = sizeof(order) / sizeof(order[0]);
	return order;
}
