#include <ROMV2_DISPLAY_ENVIRONMENT.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_ENVIRONMENT::ROMV2_DISPLAY_ENVIRONMENT()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="dataEnvironment"></param>
void ROMV2_DISPLAY_ENVIRONMENT::Init(TFT_eSPI* tft, DataSensorEnvironment* dataEnvironment)
{
	// Valorisation des membres internes
	_tft = tft;
	_dataEnvironment = dataEnvironment;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
/// <param name="forceRedraw"></param>
void ROMV2_DISPLAY_ENVIRONMENT::ForceRedraw()
{
	// Actualisation des membres internes
	_forceRedraw = true;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_ENVIRONMENT::UpdateDisplay()
{
	// On clear l'affichage
	ClearDisplay();

	// Affichage des données
	DisplayEnvironment();

	// Reset de la mise à jour complète de l'affichage
	_forceRedraw = false;
}

/// <summary>
/// Clear de l'affichage
/// </summary>
void ROMV2_DISPLAY_ENVIRONMENT::ClearDisplay()
{
	if (_forceRedraw)
	{
		// Panneau
		_tft->fillRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_LIGHTGREY);
		_tft->drawRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_DARKGREY);
	}
}

/// <summary>
/// Affichage du panneau Environnement
/// </summary>
void ROMV2_DISPLAY_ENVIRONMENT::DisplayEnvironment()
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
		_tft->print("ENVIRONNEMENT");

		// Libellés
		_tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
		_tft->drawRightString(F("Temperature:"), 80, 50, 1);
		_tft->drawRightString(F("Humidite:"), 80, 60, 1);
		_tft->drawRightString(F("Pression:"), 80, 70, 1);
		_tft->drawRightString(F("Rosee:"), 80, 95, 1);
	}

	// Icone de l'état du Point de rosée : Affichage sur MAJ complète de l'écran ou sur changement d'état
	if (_forceRedraw || _dataEnvironment->dewPointState != _dewPointStateLast)
	{
		// Icone du Point de Rosée
		_tft->setSwapBytes(true);
		switch (_dataEnvironment->dewPointState)
		{
		case DEWPOINT_STATE_DRY:
			_tft->pushImage(15, 90, 20, 20, icon_goutte_blue_lightgrey);
			break;

		case DEWPOINT_STATE_HUMID:
			_tft->pushImage(15, 90, 20, 20, icon_goutte_orange_lightgrey);
			break;

		case DEWPOINT_STATE_WET:
			_tft->pushImage(15, 90, 20, 20, icon_goutte_red_lightgrey);
			break;

		case DEWPOINT_STATE_UNKNOWN:
		default:
			_tft->drawRect(15, 90, 20, 20, TFT_LIGHTGREY);
			break;
		}
	}

	// Mise à jour des flags
	_dewPointStateLast = _dataEnvironment->dewPointState;

	// Affichage des données
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->setTextSize(1);
	char stringBuffer[20];

	// Temperature
	_tft->setCursor(84, 50);
	if (!isnan(_dataEnvironment->temperature))
	{
		sprintf(stringBuffer, "%.2f (c)", _dataEnvironment->temperature);
		while (strlen(stringBuffer) < 10)
		{
			strcat(stringBuffer, " ");
		}
	}
	else
	{
		strcpy(stringBuffer, "            \0");
	}
	_tft->print(stringBuffer);

	// Humidité
	_tft->setCursor(84, 60);
	if (!isnan(_dataEnvironment->humidite))
	{
		sprintf(stringBuffer, "%.2f (%c)", _dataEnvironment->humidite, '%');
		while (strlen(stringBuffer) < 10)
		{
			strcat(stringBuffer, " ");
		}
	}
	else
	{
		strcpy(stringBuffer, "            \0");
	}
	_tft->print(stringBuffer);

	// Pression
	_tft->setCursor(84, 70);
	if (!isnan(_dataEnvironment->pression))
	{
		sprintf(stringBuffer, "%.2f (Pa)", _dataEnvironment->pression);
		while (strlen(stringBuffer) < 10)
		{
			strcat(stringBuffer, " ");
		}
	}
	else
	{
		strcpy(stringBuffer, "            \0");
	}
	_tft->print(stringBuffer);

	// Point de rosée
	_tft->setCursor(84, 95);
	if (!isnan(_dataEnvironment->dewPoint))
	{
		sprintf(stringBuffer, "%.2f (c)", _dataEnvironment->dewPoint);
		while (strlen(stringBuffer) < 10)
		{
			strcat(stringBuffer, " ");
		}
	}
	else
	{
		strcpy(stringBuffer, "            \0");
	}
	_tft->print(stringBuffer);
}
