// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_HOME : Objet applicatif permettant l'affichage du panneau Home
/// </summary>
class ROMV2_DISPLAY_HOME
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_HOME();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="dataEnvironment"></param>
    /// <param name="dataLuminosity"></param>
    /// <param name="dataSkyState"></param>
    /// <param name="dataGPS"></param>
    void Init(APP_TYPE appType,
                TFT_eSPI* tft,
                DataSensorEnvironment* dataEnvironment,
                DataSensorLuminosity* dataLuminosity,
                DataSensorSkyState* dataSkyState,
                DataSensorGPS* dataGPS);

    /// <summary>
    /// Actualisation de l'affichage
    /// </summary>
    void UpdateDisplay();

    /// <summary>
    /// Met à jour les variables de l'objet
    /// </summary>
    /// <param name="displayHomeType"></param>
    void SetCurrentDisplay(DISPLAY_HOME_TYPE displayHomeType);

    /// <summary>
    /// Force la mise à jour complète de l'affichage
    /// </summary>
    void ForceRedraw();

private:
    // Fonctions
    void ClearDisplay();
    void DisplayLux();
    void DisplayTemperature();
    void DisplayCoordinates();
    void DisplayDewPoint();
    void DisplayBortle();
    void DisplaySkyState();
    void DisplaySensorValues();

    // Instanciation des objets internes
    TFT_eSPI* _tft;

    // Membres internes
    APP_TYPE                _appType            = APP_ROMV2;
    DISPLAY_HOME_TYPE       _displayHomeType    = DISPLAY_HOME_SQM;
    bool                    _forceRedraw        = true;
    DataSensorEnvironment*  _dataEnvironment    = nullptr;
    DataSensorLuminosity*   _dataLuminosity     = nullptr;
    DataSensorSkyState*     _dataSkyState       = nullptr;
    DataSensorGPS*          _dataGPS            = nullptr;
    SKY_STATE               _skyStateLast       = SKY_STATE_UNKNOWN;
    DEWPOINT_STATE          _dewPointStateLast  = DEWPOINT_STATE_UNKNOWN;
};