// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_TSL2591_CALIBRATION : Objet applicatif permettant l'affichage du panneau de calibration du TSL2591
/// </summary>
class ROMV2_DISPLAY_TSL2591_CALIBRATION
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_TSL2591_CALIBRATION();

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

    /// <summary>
    /// Met à jour la valeur courante de la calibration du TSL2591
    /// </summary>
    void SetNewCalibrationValue(int newCalibrationValue);

    /// <summary>
    /// Augmente la calibration du TSL2591 de 0.01 si < TSL2591_CALIBRATION_MAX_VALUE
    /// </summary>
    void IncreaseCalibrationValue();

    /// <summary>
    /// Diminue la calibration du TSL2591 de 0.01 si > TSL2591_CALIBRATION_MIN_VALUE
    /// </summary>
    void DecreaseCalibrationValue();

    /// <summary>
    /// Renvoi la nouvelle valeur de la calibration du TSL2591
    /// </summary>
    int GetNewCalibrationValue();

private:
    // Fonctions
    void ClearDisplay();
    void DisplayCalibration();

    // Instanciation des objets internes
    TFT_eSPI* _tft;

    // Membres internes
    bool _forceRedraw = true;
    DataSensorLuminosity* _dataLuminosity;
    int _newCalibrationValue = 0;
};