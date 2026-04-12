// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_LUX : Objet applicatif permettant l'affichage du panneau Lux
/// </summary>
class ROMV2_DISPLAY_LUX
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_LUX();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="dataLuminosity"></param>
    void Init(TFT_eSPI* tft, DataSensorLuminosity* dataLuminosity);

    /// <summary>
    /// Actualisation de l'affichage
    /// </summary>
    void UpdateDisplay();

    /// <summary>
    /// Force la mise à jour complète de l'affichage
    /// </summary>
    void ForceRedraw();

private:
    // Fonctions
    void ClearDisplay();
    void DisplayLux();

    // Instanciation des objets internes
    TFT_eSPI* _tft;

    // Membres internes
    bool _forceRedraw = true;
    DataSensorLuminosity* _dataLuminosity;
};