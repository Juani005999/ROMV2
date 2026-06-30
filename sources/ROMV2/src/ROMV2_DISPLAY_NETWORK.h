// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_NETWORK : Objet applicatif permettant l'affichage du panneau de sélection du réseau
/// </summary>
class ROMV2_DISPLAY_NETWORK
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_NETWORK();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="networkMode"></param>
    void Init(TFT_eSPI* tft, NETWORK_MODE* networkMode);

    /// <summary>
    /// Actualisation de l'affichage
    /// </summary>
    void UpdateDisplay();

    /// <summary>
    /// Force la mise à jour complète de l'affichage
    /// </summary>
    void ForceRedraw();

    /// <summary>
    /// Modification du réseau en fonction de la direction (Up / Down)
    /// </summary>
    void SetNewNetworkMode(bool directionUp);

    /// <summary>
    /// Sauvegarde les credentials réseau en Preferences
    /// </summary>
    void SaveNetworkCredentials();

private:
    // Fonctions
    void ClearDisplay();
    void DisplayNetworkSelection();
    String GetNetworkName(NETWORK_MODE networkMode);

    // Instanciation des objets internes
    TFT_eSPI* _tft;

    // Membres internes
    bool            _forceRedraw        = true;
    NETWORK_MODE*   _networkMode        = nullptr;
    NETWORK_MODE    _newNetworkMode     = NETWORK_NONE;
    NETWORK_MODE    _lastNetworkMode    = NETWORK_NONE;
};