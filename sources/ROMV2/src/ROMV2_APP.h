#pragma once

// Include des Objets et Librairies
#include <ROMV2_COMMON_APP.h>
#include <ROMV2_TFT.h>
#include <ROMV2_MLX90614.h>
#include <ROMV2_ADXL345.h>
#include <ROMV2_GPS.h>

/// <summary>
/// Objet ROMV2_APP : Objet applicatif permettant le pilotage complet de l'application ROMV2_APP
/// </summary>
class ROMV2_APP : public ROMV2_COMMON_APP
{
  public:
    /// <summary>
    /// Constructeur
    /// </summary>
    ROMV2_APP();

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
    /// Ordre complet des écrans navigables au joystick pour le modèle ROMV2
    /// </summary>
    const DISPLAY_SCREEN_TYPE* GetScreenOrder(uint8_t& count) const override;

  private:
    // Fonctions

    // Instanciation des objets internes
    ROMV2_MLX90614  _mlx90614       = ROMV2_MLX90614();
    ROMV2_ADXL345   _adxl345        = ROMV2_ADXL345();
    ROMV2_GPS       _gps            = ROMV2_GPS();

    // Membres internes
};