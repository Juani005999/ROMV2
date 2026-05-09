// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_IRTEMP : Objet applicatif permettant l'affichage du panneau Etat du ciel par température IR
/// </summary>
class ROMV2_DISPLAY_IRTEMP
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_IRTEMP();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="dataSkyState"></param>
    void Init(TFT_eSPI* tft, DataSensorSkyState* dataSkyState);

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
    void DisplayIRTemp();

    // Instanciation des objets internes
    TFT_eSPI* _tft;

    // Membres internes
    bool _forceRedraw = true;
    DataSensorSkyState* _dataSkyState;
    SKY_STATE _skyStateLast = SKY_STATE_UNKNOWN;
};