#include <ROMV2_DISPLAY_NETWORK.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_NETWORK::ROMV2_DISPLAY_NETWORK()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="networkMode"></param>
void ROMV2_DISPLAY_NETWORK::Init(TFT_eSPI* tft, NETWORK_MODE* networkMode)
{
	// Valorisation des membres internes
	_tft = tft;
	_networkMode = networkMode;
	_newNetworkMode = *_networkMode;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
/// <param name="forceRedraw"></param>
void ROMV2_DISPLAY_NETWORK::ForceRedraw()
{
	// Actualisation des membres internes
	_forceRedraw = true;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_NETWORK::UpdateDisplay()
{
	// On clear l'affichage
	ClearDisplay();

	// Affichage l'écran de sélection du réseau
	DisplayNetworkSelection();

	// Reset de la mise à jour complète de l'affichage
	_forceRedraw = false;
}

/// <summary>
/// Clear de l'affichage
/// </summary>
void ROMV2_DISPLAY_NETWORK::ClearDisplay()
{
	if (_forceRedraw)
	{
		// Panneau
		_tft->fillRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_LIGHTGREY);
		_tft->drawRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_DARKGREY);
	}
}

/// <summary>
/// Affichage du Lux
/// </summary>
void ROMV2_DISPLAY_NETWORK::DisplayNetworkSelection()
{
	// Mise en forme de l'affichage
	_tft->setTextWrap(false);

	// Affichage des libellés et icones
	if (_forceRedraw)
	{
		// Titre
		_tft->setTextColor(TFT_BROWN, TFT_LIGHTGREY);
		_tft->setTextSize(1);
		_tft->setCursor(10, 35);
		_tft->print(F("SELECTION DU RESEAU"));

		// Libellés
		_tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
		_tft->drawRightString(F("Nouveau:"), 80, 60, 1);
		_tft->drawRightString(F("Actuel:"), 80, 82, 1);

		// Zone Edit
		_tft->fillRect(84, 53, 70, 20, TFT_WHITE);
		_tft->drawRect(84, 53, 70, 20, TFT_BLACK);

		// Icone du réseau
		_tft->setSwapBytes(true);
		switch (*_networkMode)
		{
			case NETWORK_BLE:
				_tft->pushImage(5, 75, 20, 20, icon_bluetooth_lightgrey);
				break;

			case NETWORK_WIFI:
				_tft->pushImage(5, 75, 20, 20, icon_wifi_lightgrey);
				break;

			case NETWORK_NONE:
			default:
				_tft->pushImage(5, 75, 20, 20, icon_network_offline_black_lightgrey);
				break;
		}
	}
	_tft->setTextColor(TFT_BROWN, TFT_LIGHTGREY);
	if (_newNetworkMode != *_networkMode)
	{
		_tft->drawCentreString(F("Clic pour sauvegarder"), 80, 100, 1);
	}
	else
	{
		_tft->drawCentreString(F("                       "), 80, 100, 1);
	}

	// Affichage des données
	_tft->setTextColor(TFT_DARKGREY, TFT_WHITE);
	if (_newNetworkMode == *_networkMode)
	{
		_tft->setTextColor(TFT_DARKGREEN, TFT_WHITE);
	}
	else
	{
		_tft->setTextColor(TFT_RED, TFT_WHITE);
	}
	_tft->setTextSize(1);

	// Initialisation buffer
	char stringBuffer[20];
	stringBuffer[0] = 0;

	// Nouvelle valeur de réseau
	_tft->drawRightString(GetNetworkName(_newNetworkMode), 145, 60, 1);

	// Valeur actuelle de réseau
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->drawRightString(GetNetworkName(*_networkMode), 145, 82, 1);

	// Icone du nouveau réseau
	_tft->setSwapBytes(true);
	if (_forceRedraw || _newNetworkMode != _lastNetworkMode)
	{
		switch (_newNetworkMode)
		{
			case NETWORK_BLE:
				_tft->pushImage(5, 53, 20, 20, icon_bluetooth_lightgrey);
				break;

			case NETWORK_WIFI:
				_tft->pushImage(5, 53, 20, 20, icon_wifi_lightgrey);
				break;

			case NETWORK_NONE:
			default:
				_tft->pushImage(5, 53, 20, 20, icon_network_offline_black_lightgrey);
				break;
		}

		// Actualisation du flag
		_lastNetworkMode = _newNetworkMode;
	}
}

/// <summary>
/// Renvoi le nom du réseau passé en paramètre
/// </summary>
String ROMV2_DISPLAY_NETWORK::GetNetworkName(NETWORK_MODE networkMode)
{
	switch (networkMode)
	{
	case NETWORK_BLE:
		return "Bluetooth";

	case NETWORK_WIFI:
		return "     Wifi";

	case NETWORK_NONE:
	default:
		return "    Aucun";
	}
}

/// <summary>
/// Modification du réseau en fonction de la direction (Up / Down)
/// </summary>
void ROMV2_DISPLAY_NETWORK::SetNewNetworkMode(bool directionUp)
{
	if (directionUp)
	{
		if (_newNetworkMode == NETWORK_WIFI)
		{
			_newNetworkMode = NETWORK_NONE;
		}
		else
		{
			_newNetworkMode = static_cast<NETWORK_MODE>(static_cast<int>(_newNetworkMode) + 1);
		}
	}
	else
	{
		if (_newNetworkMode == NETWORK_NONE)
		{
			_newNetworkMode = NETWORK_WIFI;
		}
		else
		{
			_newNetworkMode = static_cast<NETWORK_MODE>(static_cast<int>(_newNetworkMode) - 1);
		}
	}
}

/// <summary>
/// Sauvegarde les credentials réseau en Preferences
/// </summary>
void ROMV2_DISPLAY_NETWORK::SaveNetworkCredentials()
{
	Preferences prefs;

	int mode = 0;
	switch (_newNetworkMode)
	{
	case NETWORK_BLE:
		mode = 1;
		break;

	case NETWORK_WIFI:
		mode = 2;
		break;

	case NETWORK_NONE:
	default:
		mode = 0;
		break;
	}

	// Enregistrement dans les préférences
	prefs.begin(PREF_NETWORK_NAMESPACE, false);
	prefs.putInt(PREF_NETWORK_KEY_NETWORK, mode);
	prefs.end();

	// Trace
	debugln(F(""));
	debugln(F("[APP] SaveCredentials"));
	debug(F("[APP] NetworkMode : "));
	debugln(mode);

	// On affiche le message d'attente
	_tft->setTextColor(TFT_BROWN, TFT_LIGHTGREY);
	_tft->drawCentreString(F("Redemarrage en cours ..."), 80, 100, 1);

	// On delay et on redémarre l'ESP
	delay(2000);
	ESP.restart();
}
