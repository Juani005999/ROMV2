#pragma once

// Include des librairies
#include <ROMV2_TFT_COMMON.h>
#include <ROMV2_DISPLAY_IRTEMP.h>
#include <ROMV2_DISPLAY_ACCELERATION.h>
#include <ROMV2_DISPLAY_GPS.h>

/// <summary>
/// Objet ROMV2_TFT : Objet applicatif permettant le pilotage de l'écran TFT
/// </summary>
class ROMV2_TFT : public ROMV2_TFT_COMMON
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_TFT();

    /// <summary>
    /// Actualisation de l'affichage
    /// </summary>
    void UpdateDisplay() override;

protected:
    /// <summary>
    /// Affichage de l'écran d'initialisation de l'application
    /// </summary>
    void DisplaySplashApp() override;

    /// <summary>
    /// Force la mise à jour complète de l'affichage
    /// </summary>
    void ForceRedraw() override;

    /// <summary>
    /// Initialisation des écrans propres au modèle.
    /// </summary>
    void InitModelDisplays() override;

  private:
    // Fonctions

    // Instanciation des objets internes
    ROMV2_DISPLAY_IRTEMP                _displayIRTemp              = ROMV2_DISPLAY_IRTEMP();               // Objet pour l'affichage du panneau Etat du ciel par température IR
    ROMV2_DISPLAY_ACCELERATION          _displayAcceleration        = ROMV2_DISPLAY_ACCELERATION();         // Objet pour l'affichage du panneau Accélération (niveau à bulle)
    ROMV2_DISPLAY_GPS                   _displayGPS                 = ROMV2_DISPLAY_GPS();                  // Objet pour l'affichage du panneau GPS

    // Membres internes
};