#include <ROMV2_DISPLAY_HEADER.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_HEADER::ROMV2_DISPLAY_HEADER()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="dataEnvironment"></param>
/// <param name="dataGPS"></param>
/// <param name="bluetoothConnected"></param>
void ROMV2_DISPLAY_HEADER::Init(TFT_eSPI* tft, DataSensorEnvironment* dataEnvironment, DataSensorGPS* dataGPS, bool* bluetoothConnected)
{
	// Valorisation des objets internes
	_tft = tft;
	_dataEnvironment = dataEnvironment;
	_dataGPS = dataGPS;
	_bluetoothConnected = bluetoothConnected;
}

/// <summary>
/// Met à jour les variables de l'objet
/// </summary>
/// <param name="currentDisplayScreenType"></param>
void ROMV2_DISPLAY_HEADER::SetCurrentDisplay(DISPLAY_SCREEN_TYPE currentDisplayScreenType)
{
	// Actualisation des membres internes
	_currentDisplayScreenType = currentDisplayScreenType;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
/// <param name="forceRedraw"></param>
void ROMV2_DISPLAY_HEADER::ForceRedraw()
{
	// Actualisation des membres internes
	_forceRedraw = true;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_HEADER::UpdateDisplay()
{
	// On clear l'affichage
	ClearDisplay();

	// Affichage des données
	DisplayMainIcon();
	DisplayDate();
	DisplayGPSIcon();
	DisplayBluetoothIcon();

	// Reset de la mise à jour complète de l'affichage
	_forceRedraw = false;
}

/// <summary>
/// Clear de l'affichage du Header
/// </summary>
void ROMV2_DISPLAY_HEADER::ClearDisplay()
{
	if (_forceRedraw)
	{
		// Background et cadre
		_tft->fillRect(0, 0, SCREEN_WIDTH, SCREEN_HDR_HEIGHT, TFT_WHITE);
		_tft->drawRect(0, 0, SCREEN_WIDTH, SCREEN_HDR_HEIGHT, TFT_SILVER);

		// Contour de l'icone de luminosité
		_tft->drawCircle(102, 16, 8, TFT_MAROON);
	}
}

/// <summary>
/// Affichage de la date et heure
/// </summary>
void ROMV2_DISPLAY_HEADER::DisplayDate()
{
	// Mise en forme de l'affichage
	_tft->setTextColor(TFT_BLACK, TFT_WHITE);
	_tft->setTextSize(1);

	// Mise en forme de la date
	char destDate[20];
	destDate[0] = '\0';
	if (strlen(_dataGPS->gpsDate) > 0 && strcmp(_dataGPS->gpsDate, "00/00/1900") != 0)
	{
		strncpy(destDate, _dataGPS->gpsDate, 10);
		destDate[10] = '\0';
	}
	else
	{
		destDate[0] = '\0';
	}
	while (strlen(destDate) < 10)
	{
		strcat(destDate, " ");
	}

	// Mise en forme de l'heure
	char destTime[20];
	destTime[0] = '\0';
	char destUTC[7] = "(UTC)";
	if (strlen(_dataGPS->gpsTime) > 0 && strcmp(_dataGPS->gpsTime, "00:00:00") != 0)
	{
		strncpy(destTime, _dataGPS->gpsTime, 5);
		destTime[5] = '\0';
		strcat(destTime, destUTC);
	}
	else
	{
		destTime[0] = '\0';
	}
	while (strlen(destTime) < 10)
	{
		strcat(destTime, " ");
	}

	// Affichage des données
	_tft->setCursor(SCREEN_WIDTH / 2 - 50, 6);
	_tft->print(destDate);
	_tft->setCursor(SCREEN_WIDTH / 2 - 50, 16);
	_tft->print(destTime);
}

/// <summary>
/// Affichage de l'icone principal
/// </summary>
void ROMV2_DISPLAY_HEADER::DisplayMainIcon()
{
	if (_forceRedraw)
	{
		_tft->setSwapBytes(true);
		switch (_currentDisplayScreenType)
		{
		case DISPLAY_LUX:
			_tft->pushImage(5, 5, 20, 20, icon_ampoule);
			break;

		case DISPLAY_ENVIRONMENT:
			_tft->pushImage(5, 5, 20, 20, icon_thermometre);
			break;

		case DISPLAY_IRTEMP:
			_tft->pushImage(5, 5, 20, 20, icon_ir);
			break;

		case DISPLAY_ACCELERATION:
			_tft->pushImage(5, 5, 20, 20, icon_bubble_level);
			break;

		case DISPLAY_GPS:
			_tft->pushImage(5, 5, 20, 20, icon_gps);
			break;

		case DISPLAY_HOME:
		default:
			_tft->pushImage(5, 5, 20, 20, icon_home);
			break;
		}
	}
}

/// <summary>
/// Affichage de l'icone GPS
/// </summary>
void ROMV2_DISPLAY_HEADER::DisplayGPSIcon()
{
	// On vérifie l'état de l'icone a afficher
	bool iconState = _dataGPS->gpsFix;

	// Affichage icone sur mise à jour complète de l'affichage
	if (_forceRedraw
		|| iconState != _iconGPSLastState)
	{
		_tft->setSwapBytes(true);
		if (_dataGPS->gpsFix)
		{
			_tft->pushImage(129, 7, 16, 16, icon_gps_green);
		}
		else
		{
			_tft->pushImage(129, 7, 16, 16, icon_gps_red);
		}
	}
	_iconGPSLastState = iconState;

	// Mise en forme de l'affichage
	_tft->setTextColor(TFT_BLACK, TFT_WHITE);
	_tft->setTextSize(1);

	// Mise en forme du texte
	if (strlen(_dataGPS->gpsSatellites) < 2)
	{
		strcat(_dataGPS->gpsSatellites, " ");
	}

	// Nombre de satellites
	_tft->setCursor(144, 16);
	_tft->print(_dataGPS->gpsSatellites);
}

/// <summary>
/// Affichage de l'icone Bluetooth
/// </summary>
void ROMV2_DISPLAY_HEADER::DisplayBluetoothIcon()
{
	// On vérifie l'état de l'icone a afficher
	bool iconState = *_bluetoothConnected;

	if (_forceRedraw
		|| iconState != _iconBluetoothLastState)
	{
		_tft->setSwapBytes(true);
		_tft->pushImage(112, 8, 16, 16, *_bluetoothConnected ? icon_bluetooth_green : icon_bluetooth_red);
	}
	_iconBluetoothLastState = iconState;
}

/// <summary>
/// Affiche l'icone d'état de lecture de la luminosité
/// </summary>
/// <param name="on"></param>
void ROMV2_DISPLAY_HEADER::SetLuminosityIcon(bool on)
{
	if (!firstLuminosityIcon && _currentDisplayScreenType != DISPLAY_NONE)
	{
		_tft->drawCircle(102, 16, 8, TFT_MAROON);
		_tft->fillCircle(102, 16, 6, on ? TFT_ORANGE : TFT_WHITE);
	}
	firstLuminosityIcon = false;
}
