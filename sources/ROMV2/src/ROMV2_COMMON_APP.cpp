#include <ROMV2_COMMON_APP.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_COMMON_APP::ROMV2_COMMON_APP()
{
}

/// <summary>
/// Initialisation de l'App
/// </summary>
void ROMV2_COMMON_APP::Init()
{
	// Positionnement des PIN
	pinMode(JOYSTICK_PIN_SWITCH, INPUT_PULLUP);
	digitalWrite(JOYSTICK_PIN_SWITCH, HIGH);
	
	// Lecture du mode réseau depuis les préférences
	LoadCredentials();

	// Initialisation Wire avec définition des PIN SDA/SCL
	Wire.setPins(ESP32_GPIO_SDA, ESP32_GPIO_SCL);

	// Création du verrou partagé du bus I2C (avant tout accès et tout démarrage de tâche)
	ROMV2_I2C::Init();

	// Initialisation des objets représentant les capteurs et périphériques
	InitModelSensors();

	// Chargement de la valeur de tsl2591Calibration depuis la ROM
	_tsl2591.LoadTSL2591Calibration();
	_tft->SetNewCalibrationValue(_dataLuminosity.tsl2591Calibration);

	// Démarrage des tâches FreeRTOS
	_tsl2591.StartTask();
	if (_networkMode == NETWORK_WIFI)
	{
		_wifi.StartTask();
	}

	// Initialisation Chronos
	_chronoReadJoystick = 0;
	_chronoJoystickButtonPressed = 0;
	_chronoJoystickButtonMoved = 0;
	_chronoDisplayTimeout = millis();

	// Initialisation des performances pour les traces
	InitAppPerformances();
}

/// <summary>
/// Processus complet a exécuter lors d'un Loop
/// </summary>
void ROMV2_COMMON_APP::Loop()
{
	// Lecture de l'état du Joystick
	ReadJoystickState();

	// Lecture des capteurs
	LoopModelSensors();

	// Envoi des notifications des caractéristiques du BLE (Bluetooth Low Energy)
	if (_networkMode == NETWORK_BLE)
	{
		_bluetoothLE.Notify();
	}

	// Gestion du Wifi et du MQTT
	if (_networkMode == NETWORK_WIFI)
	{
		// Traitement cadencé du WiFi (serveur web AP + surveillance connexion STA)
		_wifi.Loop();

		// Traitement cadencé du MQTT (reconnexion + publication)
		_mqtt.Loop();
	}

	// Actualisation du display
	UpdateCurrentDisplay();
	_tft->SetCurrentDisplay(_currentDisplayScreenType, _displayHomeType);
	_tft->UpdateDisplay();

	// Trace des performances
	TraceAppPerformances();
}

/// <summary>
/// Actualise l'affichage en cours sur action du Joystick
/// </summary>
void ROMV2_COMMON_APP::UpdateCurrentDisplay()
{
	// Gestion de la mise en veille de l'affichage
	// Exception : pas de mise en veille si l'écran WiFi est en mode AP (on attend la config)
	if (_currentAction == ACTION_NONE
		&& millis() > _chronoDisplayTimeout + SCREEN_TIMEOUT
		&& !(_currentDisplayScreenType == DISPLAY_WIFI && (_dataWifi.wifiState == WIFI_STATE_AP || _dataWifi.wifiState == WIFI_STATE_CONNECTING)))
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
					// Ecran suivant selon l'ordre propre au modèle
					GoToAdjacentScreen(+1);
					break;

				case ACTION_LEFT:
					// Ecran précédent selon l'ordre propre au modèle
					GoToAdjacentScreen(-1);
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
					else if (_currentDisplayScreenType == DISPLAY_TSL2591_CALIBRATION)
					{
						_tsl2591.SaveTSL2591Calibration(_tft->GetNewCalibrationValue());
						_tft->SetNewCalibrationValue(_dataLuminosity.tsl2591Calibration);
					}
					else if (_currentDisplayScreenType == DISPLAY_NETWORK)
					{
						_tft->SaveNetworkCredentials();
					}
					else if (_currentDisplayScreenType == DISPLAY_WIFI)
					{
						// Clic sur l'écran WiFi : action sur le bouton actif
						if (_dataWifi.wifiState == WIFI_STATE_AP)
						{
							// Mode AP actif -> bouton "Annuler" -> retour STA
							debugln(F(""));
							debugln(F("[WIFI] Annulation mode AP"));
							_wifi.CancelAP();
						}
						else
						{
							// Mode STA -> bouton "Réinitialiser" -> passage en mode AP
							debugln(F(""));
							debugln(F("[WIFI] Demarrage mode AP (reinitialisation)"));
							_wifi.StartAP();
						}
					}
					break;

				case ACTION_CLICK_LONG:
					// Si on est en mode Wifi STA, on reset les Credentials Wifi et MQTT
					if (_currentDisplayScreenType == DISPLAY_WIFI && _dataWifi.wifiState == WIFI_STATE_STA)
					{
						_wifi.ResetCredentials();
						_mqtt.ResetBrokerConfig();
					}
					// Sinon on passe en mode veille
					else
					{
						// Si on est en mode Wifi AP, on annule d'abord avant de passer en veille
						if (_currentDisplayScreenType == DISPLAY_WIFI && _dataWifi.wifiState == WIFI_STATE_AP)
						{
							_wifi.CancelAP();
						}
						_currentDisplayScreenType = DISPLAY_NONE;
					}
					break;

				case ACTION_UP:
					if (_currentDisplayScreenType == DISPLAY_TSL2591_CALIBRATION)
					{
						_tft->IncreaseCalibrationValue();
					}
					else if (_currentDisplayScreenType == DISPLAY_NETWORK)
					{
						_tft->SetNewNetworkMode(true);
					}
					else
					{
						_currentDisplayScreenType = DISPLAY_HOME;
					}
					break;

				case ACTION_DOWN:
					if (_currentDisplayScreenType == DISPLAY_TSL2591_CALIBRATION)
					{
						_tft->DecreaseCalibrationValue();
					}
					else if (_currentDisplayScreenType == DISPLAY_NETWORK)
					{
						_tft->SetNewNetworkMode(false);
					}
					else
					{
						_currentDisplayScreenType = DISPLAY_HOME;
					}
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

	// Vérification : si une configuration WiFi a été reçue via la page web,
	// l'écran WiFi repasse automatiquement en mode STA
	if (_currentDisplayScreenType == DISPLAY_WIFI && _dataWifi.configReceived)
	{
		_dataWifi.configReceived = false;
		_wifi.FlushConfigReceived();

		debugln(F(""));
		debugln(F("[WIFI] Config reçue -> retour affichage STA"));
	}
}

/// <summary>
/// Charge les credentials depuis les Preferences
/// </summary>
void ROMV2_COMMON_APP::LoadCredentials()
{
	Preferences prefs;

	// Network
	prefs.begin(PREF_NETWORK_NAMESPACE, true);
	int newNetworkMode = prefs.getInt(PREF_NETWORK_KEY_NETWORK, 0);
	prefs.end();

	switch (newNetworkMode)
	{
		case 1:
			_networkMode = NETWORK_BLE;
			break;

		case 2:
			_networkMode = NETWORK_WIFI;
			break;

		case 0:
		default:
			_networkMode = NETWORK_NONE;
			break;
	}

	// Trace
	debugln(F(""));
	debugln(F("[APP] LoadCredentials"));
	debug(F("[APP] NetworkMode : "));
	debugln(_networkMode);
}

/// <summary>
/// Indique si un écran est visible dans le carrousel à l'instant T.
/// L'écran de configuration WiFi n'apparaît que lorsque le mode réseau est WiFi.
/// </summary>
bool ROMV2_COMMON_APP::IsScreenVisible(DISPLAY_SCREEN_TYPE screen) const
{
	if (screen == DISPLAY_WIFI && _networkMode != NETWORK_WIFI)
	{
		return false;
	}
	return true;
}

/// <summary>
/// Déplace l'écran courant vers l'écran adjacent en suivant l'ordre propre au
/// modèle (GetScreenOrder), en sautant les écrans non visibles à l'instant T.
/// direction = +1 : droite ; direction = -1 : gauche. La navigation est circulaire.
/// </summary>
void ROMV2_COMMON_APP::GoToAdjacentScreen(int direction)
{
	uint8_t count = 0;
	const DISPLAY_SCREEN_TYPE* order = GetScreenOrder(count);
	if (order == nullptr || count == 0)
	{
		return;
	}

	// Recherche de l'index de l'écran courant dans l'ordre du modèle
	int idx = 0;
	for (uint8_t i = 0; i < count; i++)
	{
		if (order[i] == _currentDisplayScreenType)
		{
			idx = i;
			break;
		}
	}

	// Avance dans la direction demandée en sautant les écrans non visibles
	for (uint8_t step = 0; step < count; step++)
	{
		idx = (idx + direction + count) % count;
		if (IsScreenVisible(order[idx]))
		{
			_currentDisplayScreenType = order[idx];
			return;
		}
	}
}
