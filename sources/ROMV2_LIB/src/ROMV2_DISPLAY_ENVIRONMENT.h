// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_ENVIRONMENT : Objet applicatif permettant l'affichage du panneau Environnement
/// </summary>
class ROMV2_DISPLAY_ENVIRONMENT
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_ENVIRONMENT();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="dataEnvironment"></param>
    void Init(TFT_eSPI* tft, DataSensorEnvironment* dataEnvironment);

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
    void DisplayEnvironment();

    // Instanciation des objets internes
    TFT_eSPI* _tft;

    // Membres internes
    bool _forceRedraw = true;
    DataSensorEnvironment* _dataEnvironment;
    DEWPOINT_STATE _dewPointStateLast = DEWPOINT_STATE_UNKNOWN;
};