// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_WIFI : Affichage de l'écran WiFi sur le TFT
/// </summary>
class ROMV2_DISPLAY_WIFI
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_WIFI();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="dataWifi"></param>
    void Init(TFT_eSPI* tft, DataWifi* dataWifi);

    /// <summary>
    /// Actualisation de l'affichage
    /// </summary>
    void UpdateDisplay();

    /// <summary>
    /// Force la mise à jour complète de l'affichage
    /// </summary>
    void ForceRedraw();

    /// <summary>
    /// Retourne l'état courant de l'écran WiFi
    /// </summary>
    DISPLAY_WIFI_STATE GetDisplayWifiState() const;

private:
    // Fonctions internes
    void ClearDisplay();
    void DrawSTA();
    void DrawAPWaiting();
    void DrawSTAConnecting();
    void DrawButton(const char* label, uint16_t color);

    // Membres internes
    TFT_eSPI*           _tft            = nullptr;
    DataWifi*           _dataWifi       = nullptr;
    DISPLAY_WIFI_STATE  _displayState   = DISPLAY_WIFI_STATE_STA;
    bool                _forceRedraw    = true;

    // Cache pour détecter les changements sans redraw complet
    bool        _lastConnected  = false;
    char        _lastIp[20]     = "";
    WIFI_STATE  _lastWifiState  = WIFI_STATE_STA;

    // Constantes de mise en page
    static const int CONTENT_Y      = 35;   // Y de début du contenu (sous le header)
    static const int LINE_HEIGHT    = 14;   // Hauteur d'une ligne de texte
    static const int LABEL_X        = 5;    // X des labels
    static const int VALUE_X        = 70;   // X des valeurs
    static const int BTN_X          = 20;   // X du bouton
    static const int BTN_Y          = 100;  // Y du bouton
    static const int BTN_W          = 120;  // Largeur du bouton
    static const int BTN_H          = 20;   // Hauteur du bouton
};