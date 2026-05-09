// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_ACCELERATION : Objet applicatif permettant l'affichage du panneau Accélération (niveau à bulle)
/// </summary>
class ROMV2_DISPLAY_ACCELERATION
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_ACCELERATION();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="dataAcceleration"></param>
    void Init(TFT_eSPI* tft, DataSensorAcceleration* dataAcceleration);

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
    void DisplayAcceleration();
    void DisplayValues();
    void DrawBubble();

    // Instanciation des objets internes
    TFT_eSPI* _tft;

    // Membres internes
    bool _forceRedraw = true;
    DataSensorAcceleration* _dataAcceleration;
};