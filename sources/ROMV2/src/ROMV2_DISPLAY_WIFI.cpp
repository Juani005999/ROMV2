#include <ROMV2_DISPLAY_WIFI.h>

/// <summary>
/// Constructeur
/// </summary>
ROMV2_DISPLAY_WIFI::ROMV2_DISPLAY_WIFI()
{
}

/// <summary>
/// Initialisation
/// </summary>
void ROMV2_DISPLAY_WIFI::Init(TFT_eSPI* tft, DataWifi* dataWifi)
{
    _tft = tft;
    _dataWifi = dataWifi;
}

/// <summary>
/// Actualisation de l'affichage
/// </summary>
void ROMV2_DISPLAY_WIFI::UpdateDisplay()
{
    // Détection de changement d'état nécessitant un redraw
    bool stateChanged = (_dataWifi->connected != _lastConnected)
        || (strcmp(_dataWifi->ip, _lastIp) != 0)
        || (_dataWifi->wifiState != _lastWifiState);

    if (!_forceRedraw && !stateChanged)
    {
        return;
    }

    // On force le redraw si l'état du Wifi a changé
    if (stateChanged)
    {
        _forceRedraw = true;
    }

    // On met à jour la variable _displayState en fonction de l'état en cours du Wifi
    switch (_dataWifi->wifiState)
    {
        case WIFI_STATE_AP:
            _displayState = DISPLAY_WIFI_STATE_AP_WAITING;
            break;

        case WIFI_STATE_CONNECTING:
            _displayState = DISPLAY_WIFI_STATE_STA_CONNECTING;
            break;

        case WIFI_STATE_STA:
        default:
            _displayState = DISPLAY_WIFI_STATE_STA;
            break;
    }

    // Mise à jour du cache
    _lastConnected = _dataWifi->connected;
    _lastWifiState = _dataWifi->wifiState;
    strncpy(_lastIp, _dataWifi->ip, sizeof(_lastIp) - 1);

    // Clear de l'affichage
    ClearDisplay();

    // Actualise l'affichage en fonction du mode
    switch (_displayState)
    {
        case DISPLAY_WIFI_STATE_AP_WAITING:
            DrawAPWaiting();
            break;

        case DISPLAY_WIFI_STATE_STA_CONNECTING:
            DrawSTAConnecting();
            break;

        case DISPLAY_WIFI_STATE_STA:
        default:
            DrawSTA();
            break;
    }

    // Update des flags
    _forceRedraw = false;
}

/// <summary>
/// Force la mise à jour complète de l'affichage
/// </summary>
void ROMV2_DISPLAY_WIFI::ForceRedraw()
{
    _forceRedraw = true;
}

/// <summary>
/// Retourne l'état courant
/// </summary>
DISPLAY_WIFI_STATE ROMV2_DISPLAY_WIFI::GetDisplayWifiState() const
{
    return _displayState;
}

/// <summary>
/// Efface la zone de contenu (sous le header)
/// </summary>
void ROMV2_DISPLAY_WIFI::ClearDisplay()
{
    if (_forceRedraw)
    {
        // Panneau
        _tft->fillRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_LIGHTGREY);
        _tft->drawRect(0, SCREEN_HDR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - SCREEN_HDR_HEIGHT, TFT_DARKGREY);
    }
}

/// <summary>
/// Affichage mode STA : SSID / Password masqué / IP / état + bouton Réinitialiser
/// </summary>
void ROMV2_DISPLAY_WIFI::DrawSTA()
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
        _tft->print(F("WIFI (Station)"));

        // Libellés
        _tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
        _tft->drawRightString(F("SSID:"), 70, 50, 1);
        _tft->drawRightString(F("Pass.:"), 70, 60, 1);
        _tft->drawRightString(F("IP:"), 70, 70, 1);
        _tft->drawRightString(F("Etat:"), 70, 80, 1);

        // Bouton Définir
        DrawButton("Definir le WiFi", TFT_DARKGREEN);
    }

    // Affichage des données
    _tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
    _tft->setTextSize(1);

    // SSID
    char stringBuffer[20];
    stringBuffer[0] = 0;
    _tft->setCursor(74, 50);
    if (strlen(_dataWifi->ssid) > 0)
    {
        strncpy(stringBuffer, _dataWifi->ssid, 11);
        stringBuffer[11] = '\0';
        // Tronquer si trop long pour l'écran (max ~11 caractères en taille 1)
        if (strlen(_dataWifi->ssid) > 11)
        {
            strcat(stringBuffer, "..");
        }
    }
    else
    {
        strcpy(stringBuffer, "(non defini)");
    }
    _tft->print(stringBuffer);

    // Password
    _tft->setCursor(74, 60);
    stringBuffer[0] = 0;
    if (strlen(_dataWifi->password) > 0)
    {
        // Afficher des points selon la longueur réelle, max 10
        int dots = min((int)strlen(_dataWifi->password), 10);
        for (int i = 0; i < dots; i++)
        {
            strcat(stringBuffer, "*");
        }
    }
    else
    {
        strcpy(stringBuffer, "(vide)");
    }
    _tft->print(stringBuffer);

    // Ip
    _tft->setCursor(74, 70);
    stringBuffer[0] = 0;
    if (_dataWifi->connected && strlen(_dataWifi->ip) > 0)
    {
        strcpy(stringBuffer, _dataWifi->ip);
    }
    else
    {
        strcpy(stringBuffer, "---");
    }
    _tft->print(stringBuffer);

    // Statut
    _tft->setCursor(74, 80);
    stringBuffer[0] = 0;
    if (_dataWifi->connected)
    {
        strcpy(stringBuffer, "Connecte");
    }
    else if (strlen(_dataWifi->ssid) == 0)
    {
        strcpy(stringBuffer, "Non config.");
    }
    else
    {
        strcpy(stringBuffer, "Deconnecte");
    }
    _tft->print(stringBuffer);
}

/// <summary>
/// Affichage mode STA : Connexion en cours
/// </summary>
void ROMV2_DISPLAY_WIFI::DrawSTAConnecting()
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
        _tft->print(F("WIFI (Station)"));

        // Libellés
        _tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
        _tft->drawCentreString(F("Connexion en cours ..."), 80, 50, 1);
        _tft->drawRightString(F("SSID:"), 80, 70, 1);
    }

    // Affichage des données
    _tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
    _tft->setTextSize(1);

    // SSID
    char stringBuffer[20];
    stringBuffer[0] = 0;
    _tft->setCursor(84, 70);
    if (strlen(_dataWifi->ssid) > 0)
    {
        strncpy(stringBuffer, _dataWifi->ssid, 11);
        stringBuffer[11] = '\0';
        // Tronquer si trop long pour l'écran (max ~11 caractères en taille 1)
        if (strlen(_dataWifi->ssid) > 11)
        {
            strcat(stringBuffer, "..");
        }
    }
    else
    {
        strcpy(stringBuffer, "(non defini)");
    }
    _tft->print(stringBuffer);
}

/// <summary>
/// Affichage mode AP : SSID AP / IP AP / message d'attente + bouton Annuler
/// </summary>
void ROMV2_DISPLAY_WIFI::DrawAPWaiting()
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
        _tft->print(F("WIFI (Config AP)"));

        // Libellés
        _tft->setTextColor(TFT_BLACK, TFT_LIGHTGREY);
        _tft->drawRightString(F("SSID AP:"), 70, 47, 1);
        _tft->drawRightString(F("IP:"), 70, 57, 1);

        // Message d'attente
        _tft->setTextColor(TFT_DARKGREEN, TFT_LIGHTGREY);
        _tft->drawString(F("Connectez-vous au point"), 10, 68, 1);
        _tft->drawString(F("d'acces puis ouvrez le"), 10, 78, 1);
        _tft->drawString(F("navigateur sur l'Ip"), 10, 88, 1);

        // Bouton Annuler
        DrawButton("Annuler", TFT_RED);
    }

    // Affichage des données
    _tft->setTextColor(TFT_DARKGREY, TFT_LIGHTGREY);
    _tft->setTextSize(1);

    // SSID AP
    _tft->setCursor(74, 47);
    _tft->print(_dataWifi->apSsid);

    // IP AP
    _tft->setCursor(74, 57);
    _tft->print(_dataWifi->apIp);
}

/// <summary>
/// Dessine le bouton d'action centré en bas de l'écran
/// </summary>
void ROMV2_DISPLAY_WIFI::DrawButton(const char* label, uint16_t color)
{
    // Fond du bouton
    _tft->fillRoundRect(BTN_X, BTN_Y, BTN_W, BTN_H, 4, color);
    _tft->drawRoundRect(BTN_X, BTN_Y, BTN_W, BTN_H, 4, TFT_SILVER);

    // Texte du bouton centré
    _tft->setTextColor(TFT_WHITE, color);
    _tft->setTextSize(1);
    _tft->drawCentreString(label, 80, BTN_Y + 6, 1);
}
