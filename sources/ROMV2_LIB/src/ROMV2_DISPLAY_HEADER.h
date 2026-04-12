// Include des librairies
#include <ROMV2_APP_CONFIG.h>

/// <summary>
/// Objet ROMV2_DISPLAY_HEADER : Objet applicatif permettant l'affichage du header
/// </summary>
class ROMV2_DISPLAY_HEADER
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_DISPLAY_HEADER();

    /// <summary>
    /// Initialisation
    /// </summary>
    /// <param name="tft"></param>
    /// <param name="dataEnvironment"></param>
    /// <param name="dataGPS"></param>
    /// <param name="bluetoothConnected"></param>
    void Init(TFT_eSPI * tft, DataSensorEnvironment* dataEnvironment, DataSensorGPS* dataGPS, bool* bluetoothConnected);

    /// <summary>
    /// Actualisation de l'affichage
    /// </summary>
    void UpdateDisplay();

    /// <summary>
    /// Met à jour l'affichage en cours
    /// </summary>
    /// <param name="currentDisplayScreenType"></param>
    void SetCurrentDisplay(DISPLAY_SCREEN_TYPE currentDisplayScreenType);

    /// <summary>
    /// Force la mise à jour complète de l'affichage
    /// </summary>
    void ForceRedraw();

    /// <summary>
    /// Affiche l'icone d'état de lecture de la luminosité
    /// </summary>
    /// <param name="on"></param>
    void SetLuminosityIcon(bool on = false);

  private:
    // Fonctions
    void ClearDisplay();
    void DisplayDate();
    void DisplayMainIcon();
    void DisplayGPSIcon();
    void DisplayBluetoothIcon();

    // Instanciation des objets internes
    TFT_eSPI * _tft;

    // Membres internes
    DISPLAY_SCREEN_TYPE _currentDisplayScreenType = DISPLAY_HOME;
    bool _forceRedraw = true;
    DataSensorEnvironment* _dataEnvironment;
    DataSensorGPS* _dataGPS;
    bool* _bluetoothConnected;
    bool _iconGPSLastState = false;
    bool _iconBluetoothLastState = false;
    bool firstLuminosityIcon = true;
};