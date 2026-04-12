// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_GPS : Objet applicatif permettant l'affichage du panneau GPS
/// </summary>
class ROMV2_DISPLAY_GPS
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_GPS();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="dataGPS"></param>
    void Init(TFT_eSPI* tft, DataSensorGPS* dataGPS);

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
    void DisplayGPS();

    // Instanciation des objets internes
    TFT_eSPI* _tft;

    // Membres internes
    bool _forceRedraw = true;
    DataSensorGPS* _dataGPS;
};