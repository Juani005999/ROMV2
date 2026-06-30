#pragma once

// Include des Objets et Librairies
#include <ROMV2_COMMON_APP.h>
#include <SQMLITE_TFT.h>

/// <summary>
/// Objet SQMLITE_APP : Objet applicatif permettant le pilotage complet de l'application SQMLITE_APP
/// </summary>
class SQMLITE_APP : public ROMV2_COMMON_APP
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    SQMLITE_APP();

  protected:
    /// <summary>
    /// Initialisation des capteurs propres au modèle
    /// </summary>
    void InitModelSensors() override;

    /// <summary>
    /// Lectures cadencées des capteurs propres au modèle complet
    /// </summary>
    void LoopModelSensors() override;

    /// <summary>
    /// Lecture de l'état du Joystick
    /// </summary>
    void ReadJoystickState() override;

    /// <summary>
    /// Ordre complet des écrans navigables au joystick pour le modèle SQMLITE_APP
    /// </summary>
    const DISPLAY_SCREEN_TYPE* GetScreenOrder(uint8_t& count) const override;

  private:
    // Fonctions

    // Instanciation des objets internes

    // Membres internes
};