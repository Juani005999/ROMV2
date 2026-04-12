#include <ROMV2_DISPLAY_IRTEMP.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_IRTEMP::ROMV2_DISPLAY_IRTEMP()
{
}

/// <summary>
/// Initialisation
/// </summary>
/// <param name="tft"></param>
/// <param name="dataSkyState"></param>
void ROMV2_DISPLAY_IRTEMP::Init(TFT_eSPI* tft, DataSensorSkyState* dataSkyState)
{
	// Valorisation des membres internes
	_tft = tft;
	_dataSkyState = dataSkyState;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
/// <param name="forceRedraw"></param>
void ROMV2_DISPLAY_IRTEMP::ForceRedraw()
{
	// Actualisation des membres internes
	_forceRedraw = true;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_IRTEMP::UpdateDisplay()
{
	// On clear l'affichage
	ClearDisplay();

	// Affichage des données
	DisplayIRTemp();

	// Reset de la mise à jour complète de l'affichage
	_forceRedraw = false;
}

/// <summary>
/// Clear de l'affichage
/// </summary>
void ROMV2_DISPLAY_IRTEMP::ClearDisplay()
{
	if (_forceRedraw)
	{
		// Panneau
		_tft->fillRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_LIGHTGREY);
		_tft->drawRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_DARKGREY);
	}
}

/// <summary>
/// Affichage du panneau Etat du ciel par température IR
/// </summary>
void ROMV2_DISPLAY_IRTEMP::DisplayIRTemp()
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
		_tft->print("IR TEMPERATURE");

		// Libellés
		_tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
		_tft->drawRightString(F("Ambiant:"), 80, 90, 1);
		_tft->drawRightString(F("Objet:"), 80, 100, 1);
	}

	// Icone de l'état du ciel : Affichage sur MAJ complète de l'écran ou sur changement d'état
	if (_forceRedraw || _dataSkyState->skyState != _skyStateLast)
	{
		_tft->setSwapBytes(true);
		switch (_dataSkyState->skyState)
		{
		case SKY_STATE_CLEAR:
			_tft->pushImage(15, 47, 36, 36, icon_big_sky_clear_lightgrey);
			break;

		case SKY_STATE_PARTLY_CLOUDY:
			_tft->pushImage(15, 47, 36, 36, icon_big_sky_partly_cloudy_lightgrey);
			break;

		case SKY_STATE_CLOUDY:
			_tft->pushImage(15, 47, 36, 36, icon_big_sky_cloudy_lightgrey);
			break;

		case SKY_STATE_UNKNOWN:
		default:
			_tft->drawRect(30, 47, 36, 36, TFT_LIGHTGREY);
			break;
		}
	}

	// Mise à jour des flags
	_skyStateLast = _dataSkyState->skyState;

	// Affichage des données
	_tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
	_tft->setTextSize(1);
	char stringBuffer[20];

	// Etat du ciel
	_tft->setCursor(60, 62);
	switch (_dataSkyState->skyState)
	{
		case SKY_STATE_CLEAR:
			strcpy(stringBuffer, "Ciel clair");
			break;

		case SKY_STATE_PARTLY_CLOUDY:
			strcpy(stringBuffer, "Peu nuageux");
			break;

		case SKY_STATE_CLOUDY:
			strcpy(stringBuffer, "Nuageux");
			break;

		case SKY_STATE_UNKNOWN:
		default:
			strcpy(stringBuffer, "Inconnu");
			break;
	}
	while (strlen(stringBuffer) < 12)
	{
		strcat(stringBuffer, " ");
	}
	_tft->print(stringBuffer);

	// Temperature ambiante
	_tft->setCursor(84, 90);
	if (!isnan(_dataSkyState->tempAmbient))
	{
		sprintf(stringBuffer, "%.2f (c)", _dataSkyState->tempAmbient);
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

	// Temperature objet
	_tft->setCursor(84, 100);
	if (!isnan(_dataSkyState->tempObject))
	{
		sprintf(stringBuffer, "%.2f (c)", _dataSkyState->tempObject);
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
