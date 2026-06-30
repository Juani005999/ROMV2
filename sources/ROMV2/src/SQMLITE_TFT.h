#pragma once

// Include des librairies
#include <ROMV2_TFT_COMMON.h>

/// <summary>
/// Objet SQMLITE_TFT : Objet applicatif permettant le pilotage de l'écran TFT
/// </summary>
class SQMLITE_TFT : public ROMV2_TFT_COMMON
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    SQMLITE_TFT();

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

    // Membres internes
};