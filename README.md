# ROMV2

![ROM V2](screenshots/ROMV2.jpg)

## Sommaire
- [Description](#description)
    - [Fonctionnalités](#fonctionnalités)
    - [Echelle de Bortle](#echelle-de-bortle)
- [Liste des pièces](#liste-des-pièces)
    - [Amazon](#amazon)
    - [Ali Express](#ali-express)
    - [Visserie & Divers](#visserie--divers)
- [Montage](#montage)
    - [Breadboard](#breadboard)
    - [PCB](#pcb)
    - [Boxes](#boxes)
    - [ESP32](#esp32)
    - [Capteurs](#capteurs)
    - [Optique du TSL2591](#optique-du-tsl2591)
- [Firmware](#firmware)
    - [Bibliothèques nécessaires à l'application](#bibliothèques-nécessaires-à-lapplication)
    - [Sélection de la carte](#sélection-de-la-carte)
    - [Paramètres de l'application dans Arduino IDE](#paramètres-de-lapplication-dans-arduino-ide)
    - [Configuration de l'application](#configuration-de-lapplication)
    - [Configuration de l'écran TFT ST7735](#configuration-de-lécran-tft-st7735)
    - [Implémentation](#implémentation)
    - [Mode \"Debug\"](#mode-debug)
- [Utilisation](#utilisation)
    - [Matériel](#matériel)
    - [Calibration](#calibration)
    - [Mise en veille](#mise-en-veille)
    - [Logiciel](#logiciel)
        - [En-tête](#en-tête)
        - [Ecran d'accueil](#ecran-daccueil)
        - [Ecran Luminosité](#ecran-luminosité)
        - [Ecran Environnement](#ecran-environnement)
        - [Ecran Ir Température](#ecran-ir-température)
        - [Ecran Niveaux](#ecran-niveaux)
        - [Ecran GPS](#ecran-gps)
- [Driver ASCOM](#driver-ascom)
    - [Installation](#installation)
    - [Détection du périphérique bluetooth dans Windows](#détection-du-périphérique-bluetooth-dans-windows)
    - [Utilisation dans NINA](#utilisation-dans-nina)
        - [Sélection du ROM V2 dans NINA](#sélection-du-rom-v2-dans-nina)
        - [Première utilisation dans NINA](#première-utilisation-dans-nina)
- [Retex](#retex)
- [Révisions](#révisions)

## Description
Le ROMV2 est un Sky Quality Meter (SQM) DIY (Do It Yourself) contenant les composants suivant :
- TSL2591
- MLX90614
- BME280
- ADXL345
- GPS NEO 8M
- TFT ST7735

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Fonctionnalités

- Lecture de la luminosité (full / ir / visible), calcul de la quantité de lux, du SQM et du Bortle.
- Lecture de la luminosité dans une tâche dédiée FreeRTOS s'exécutant en paralèlle sur le 2ème coeur de l'ESP 32.
- Moyenne mobile du lux sur 50 valeurs permettant de réduire l'incertitude mesurée. Calcul du SQM et du Bortle sur la moyenne mobile du lux.
- Possibilité de remise à zéro de la moyenne mobile.
- Réglage automatique de la sensibilité (1x, 25x, 428x, 9876x) et de la durée de mesure (100ms, 200ms, 300ms, 400ms, 500ms, 600ms) du capteur de luminosité TSL2591.
- Lecture de la qualité du ciel (3 états : ciel clair / peu nuageux / nuageux) via le capteur MLX90614 permettant la lecture de la température du ciel.
- Lecture des conditions d'environnement (Température, taux d'humidité, pression atmosphérique) via le capteur BME280.
- Calcul du point de rosée.
- Niveaux à bulle permettant de diriger l'optique vers le zénith grâce au capteur ADXL345.
- Coordonnées GPS et Date/Heure UTC issues du capteur GPS NEO 8M.
- Connection Bluetooth BLE.
- Driver ASCOM permettant de se connecter via Bluetooth BLE.
- Indicateur de mesure de luminosité en cours.
- Indicateur d'état d'une connection Bluetooth BLE client.
- Indicateur d'état du GPS.
- Indicateur de charge restante dans la batterie.
- Mise en veille de l'écran après 5 minutes d'inactivité ou sur demande.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Echelle de Bortle

Voici l'échelle de Bortle utilisée dans l'application.

![Bortle Scale](screenshots/Bortle_Scale.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

## Liste des pièces
### Amazon

| Composant | Prix | Nb. pièces | Prix unitaire |
| --- | --- | --- | --- |
| [ESP32 with Battery](https://www.amazon.fr/dp/B07YXFBFWF) | 18.77 € | 1 | 18.77 € |
| [TSL2591](https://www.amazon.fr/dp/B0GGD3FNKM) | 15.10 € | 1 | 15.10 € |
| [BME280](https://www.amazon.fr/dp/B07HMQMW6M) | 30.99 € | 5 | 6.20 € |
| [ADXL345](https://www.amazon.fr/ADXL345-Acc%C3%A9l%C3%A9rom%C3%A8tres-Acc%C3%A9l%C3%A9rom%C3%A8tre-Num%C3%A9rique-Transmission/dp/B0CMHXKVFK) | 10.56 € | 5 | 2.12 € |
| [MLX90614](https://www.amazon.fr/Fyearfly-Capteur-Temp%C3%A9rature-Infrarouge-MLX90614ESF/dp/B0CG9VL65J) | 33.49 € | 2 | 16.75 € |
| [GPS NEO 8M](https://www.amazon.fr/ElectroWorldFR-Module-NEO-8M-antenne-c%C3%A9ramique/dp/B09TX372XH) | 20.99 € | 1 | 20.99 € |
| [TFT ST7735](https://www.amazon.fr/AZDelivery-TFT-Compatible-Raspberry-incluant/dp/B07TJGF8HS) | 27.99 € | 5 | 5.60 € |
| [Lentille D23 F30](https://www.amazon.fr/Lentille-biconvexe-Double-Convexe-Optique/dp/B0GVHDZD22) | 25.94 € | 1 | 25.94 € |
| [Joystick](https://www.amazon.fr/Gebildet-Joystick-Commande-Contr%C3%B4leur-Raspberry/dp/B0DPMN19G6) | 10.99 € | 8 | 1.37 € |
| [Boot Button](https://www.amazon.fr/dp/B07S1MNB8C) | 10.99 € | 5 | 2.20 € |
| [Interrupteur à bascule](https://www.amazon.fr/dp/B076VKRKQQ) | 6.49 € | 5 | 1.30 € |
| [Rallonge Micro USB](https://www.amazon.fr/dp/B08HCTZ9P5) | 11.79 € | 2 | 5.90 € |
| [Testeur de capacité](https://www.amazon.fr/dp/B0DKHT664R) | 13.19 € | 4 | 3.30 € |
| | | Total | **125.54 €** |

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Ali Express
| Composant | Prix | Nb. pièces | Prix unitaire |
| --- | --- | --- | --- |
| [ESP32 with Battery](https://fr.aliexpress.com/item/1005007579177706.html) | 4.19 € | 1 | 4.19 € |
| [TSL2591](https://fr.aliexpress.com/item/1005009222536594.html) | 4.19 € | 1 | 4.19 € |
| [BME280](https://fr.aliexpress.com/item/1005010256393146.html) | 2.99 € | 1 | 2.99 € |
| [ADXL345](https://fr.aliexpress.com/item/1005007076396937.html) | 2.04 € | 1 | 2.04 € |
| [MLX90614](https://fr.aliexpress.com/item/32853499710.html) | 5.93 € | 1 | 5.93 € |
| [GPS NEO 8M](https://fr.aliexpress.com/item/1005006323393609.html) | 9.29 € | 1 | 9.29 € |
| [TFT ST7735](https://fr.aliexpress.com/item/1005008974306385.html) | 3.20 € | 1 | 3.20 € |
| [Lentille D23 F30](https://fr.aliexpress.com/item/1005007033633969.html) | 2.06 € | 1 | 2.06 € |
| [Joystick](https://fr.aliexpress.com/item/1005009009852719.html) | 0.51 € | 1 | 0.51 € |
| [Boot Button](https://fr.aliexpress.com/item/1005007876916629.html) | 6.69 € | 5 | 1.34 € |
| [Interrupteur à bascule](https://fr.aliexpress.com/item/1005010311259797.html) | 1.65 € | 10 | 0.17 € |
| [Rallonge Micro USB](https://fr.aliexpress.com/item/1005005292730047.html) | 1.09 € | 1 | 1.09 € |
| [Testeur de capacité](https://fr.aliexpress.com/item/1005006350742151.html) | 1.45 € | 1 | 1.45 € |
| | | Total | **38.45 €** |

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Visserie & Divers

| Composant | Propriétés | Quantité |
| --- | --- | --- |
| Vis & rondelles | M4 x 20mm | 8 |
| Vis & rondelles | M2 x 20mm | 4 |
| Vis & rondelles | M2 x 8mm | 21 |
| Vis & rondelles | M2 x 6mm | 8 |
| Ecrou Nylstop autobloquants | M2 | 7 |
| Condensateur | 100 µF | 1 |
| Condensateur céramique | 100 nF | 1 |
| [PCB EPLZON](https://www.amazon.fr/dp/B0B5DCHM7K) | ... | 1 |
| [Câbles Dupont](https://www.amazon.fr/Breadboard-Jumper-Wires-Dupont-Cable/dp/B0BTT48V7P) | ... | ... |
| [PCB Vis Connecteur](https://www.amazon.fr/dp/B082ZNRQMJ?th=1) | ... | ... |
| [Jumper Breadboard](https://www.amazon.fr/dp/B0F6LJS4L2?ref=ppx_yo2ov_dt_b_fed_asin_title) | ... | ... |
| [Pile Rechargeable 18650](https://bestpiles.fr/piles-rechargeables-18650/991-pile-rechargeable-18650-inr18650mj1-li-ion-37v-3500mah-10a-8438493099829.html) | Li-ion 3,7V 3500mAh 10A | 1 |

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

## Montage

![Montage](screenshots/Montage.jpg)
![Montage](screenshots/Montage_2.jpg)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Breadboard
![Breadboard](screenshots/ROMV2_Fritzing_1.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### PCB
![PCB](screenshots/ROMV2_Fritzing_2.png)
![PCB](screenshots/PCB.jpg)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Boxes
Vous trouverez dans [ce dossier](./Boxes/) tous les fichiers STL nécessaires à l'impression 3D.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### ESP32
Des branchements direct sur l'ESP32 sont nécessaires :
- 2 câbles Dupont branchés directement sur l'interrupteur de l'ESP et connectés à l'interrupteur du ROM.
- 2 câbles branchés sur les pôles + et - de l'ESP et connectés à l'indicateur de charge restante de la batterie.

![ESP32](screenshots/ESP32.jpg)

> [!WARNING]
> Attention, le connecteur **micro USB** présent sur l'ESP est **très très fragile** (j'en ai personnellement cassé plusieurs sur d'autres projets en faisant des branchements). Je vous conseil donc de rajouter 2 points de soudure de part et d'autre du connecteur  afin d'augmenter sa solidité.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Capteurs
![Capteurs](screenshots/Capteur_1.jpg)
![Capteurs](screenshots/Capteur_2.jpg)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

#### Optique du TSL2591
![TSL2591](screenshots/Optique_tsl2591_1.jpg)
![TSL2591](screenshots/Optique_tsl2591_2.jpg)
![TSL2591](screenshots/Optique_tsl2591_3.jpg)

> [!IMPORTANT]
> La partie encadrée en jaune correspond au côté des vis du TSL2591.

> [!TIP]
> La lentille est optionnelle. Vous pouvez uniquement positionner le cache du capteur et le tube sans la lentille. Vous pourrez ensuite corriger la [calibration](#calibration) si nécessaire.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

## Firmware

- Télécharger le fichier **ROMV2_Firmware.zip**
- Depuis votre Arduino IDE, installer la bibliothèque **ROMV2**, en sélectionnant le fichier téléchargé via le menu : \
***Croquis*** -> ***Importer une bibliothèque*** -> ***Ajouter la bibliothèque .ZIP ...***

![ROMV2_LIB](screenshots/Instal_Lib.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Bibliothèques nécessaires à l'application
Voici la liste des bibliothèques utilisées par l'application que vous devez installer via le gestionnaire de librairies dans votre *Arduino IDE* :

- Adafruit_TSL2591
- Adafruit_BME280
- Adafruit_MLX90614
- Adafruit_ADXL345
- TFT_eSPI

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Sélection de la carte
Dans votre *Arduino IDE*, sélectionnez la carte **ESP32 Dev Module**.
![ESP32 Dev Module](screenshots/ESP32_Dev_Module.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Paramètres de l'application dans Arduino IDE

> [!WARNING]
> Attention : l'application nécessite de se positionner en mode **"Huge App"** (cf. image ci-dessous), permettant ainsi de disposer de l'espace mémoire suffisant pour le fonctionnement sur l'ESP32.

![Arduino IDE Huge App](screenshots/IDE_HugeApp.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Configuration de l'application
Vous pouvez accéder à la configuration de l'application via le fichier suivant : \
\<***Arduino librairies***\> -> ***ROMV2*** -> ***src*** -> ***ROMV2_APP_CONFIG.h***

![ROMV2_APP_CONFIG.h](screenshots/ROMV2_APP_CONFIG.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Configuration de l'écran TFT ST7735
Afin de faire fonctionner votre écran, il vous sera nécessaire de le configurer via la bibliothèque **TFT_eSPI** en modifiant le fichier suivant : \
\<***Arduino librairies***\> -> ***TFT_sSPI*** -> ***User_Setup.h***

Vous devez modifier 2 paramètres :

- La sélection de votre écran :
![TFT_eSPI Select Screen](screenshots/TFT_SelectScreen.png)
- Les PINs associés à votre écran :
![TFT_eSPI Select PINs](screenshots/TFT_SelectPins.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Implémentation
Pour démarrer un nouveau projet ROMV2, il vous suffit d'aller dans les examples de votre Arduino IDE, et de sélectionner **ROMV2**.

![ROMV2 Application example](screenshots/ROMV2_ino.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Mode \"Debug\"
Pendant le développement de votre projet, vous pouvez activer le mode **Debug**.


> [!WARNING]
> Afin d'améliorer les performances, **n'oubliez pas de le désactiver une fois le fois le projet abouti**.

Dans le fichier suivant : \
\<***Arduino librairies***\> -> ***ROMV2*** -> ***src*** -> ***JUANITO_APP.h*** \
Enlevez les commentaires devant la ligne : \
`#define DEBUG`

![ROMV2 Debug](screenshots/ROMV2_Debug.png)

En activant le mode **Debug**, vous pourrez voir les traces de l'application dans le *Moniteur série* de votre *Arduino IDE* pendant l'exécution. 

![ROMV2 Logs](screenshots/ROMV2_Log.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

## Utilisation

### Matériel
![ROMV2 Logs](screenshots/ROMV2_Bottom.jpg)

Vous trouverez au bas du ROM V2 :
- l'interrupteur On/Off de l'appareil.
- 1 connecteur **Micro USB** permettant de recharger le ROM V2 et servant également à la mise à jour du firmware.
- 1 interrupteur (bouton poussoir) permettant de mettre l'ESP en mode Boot lors de la mise à jour du firmware.
- L'indicateur de charge restante dans la batterie. Appuyez sur le bouton **"TEST"** de l'indicateur pour connaître la charge restante.

Le pilotage du ROM V2 s'effectue via le joystick :
- Droite et gauche permettent de naviguer parmis les différents écrans.
- Haut et Bas permettent un retour direct à l'écran d'accueil.
- Sur l'écran d'acceuil, un clic sur le joystick permet d'alterner l'affichage Lux et SQM.
- Sur l'écran de luminosité, un clic sur le joystick permet de réinitialiser la moyenne mobile en cours.
- Un clic long sur le joystick permet de basculer l'écran en mode veille.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Calibration

Dans le fichier de configuration, vous trouverez une constante définie permettant de calibrer le ROM V2 dans sa lecture de luminosité via le TSL2591.
La constante de calibration est : `TSL2591_CALIBRATION`

![Calibration](screenshots/Calibration.png)

> [!TIP]
> La calibration nécessaire avec la lentille donne par le calcul une valeure de 1.121, mais dans la pratique, la valeure de 0 donne des résultats plus cohérents. Vous pouvez également donner des valeurs négatives si nécessaire.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Mise en veille

L'écran du ROM V2 s'éteint automatiquement après 5 min d'inactivité (modifiable dans le fichier de configuration).
Cette mise en veille peut être déclenchée à tout moment par un "click long" sur le joystick.

Pour sortir du mode veille, il suffit d'effectuer une action sur le joystick, peu importe laquelle.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Logiciel

Le firmware est constitué de 6 écrans, avec un en-tête commun à tous les écrans.

#### En-tête

![Header](screenshots/Header.png)

L'en-tête comprend les éléments suivants :
- Un icone principal spécifique à l'écran en cours d'affichage.
- La date et l'heure UTC remontée par le capteur GPS.
- Un icone indiquant qu'une prise de mesure de la luminosité est en cours.
- Un icone indiquant la présence d'une connexion client via Blutooth (2 états : rouge et vert).
- Un icone indiquant si le GPS est "fixé", remontant ainsi les informations de localisation (2 états : rouge et vert). Est également précisé le nombre de satellites à partir desquels le capteur récupère les informations. 

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

#### Ecran d'accueil

L'écran d'accueil comprend les éléments suivants :
- L'affichage des coordonnées GPS.
- Un icone représentant l'état du ciel. L'icone à trois états : 
    - Ciel clair.
    - Peu nuageux.
    - Nuageux.
- Un icone représentant le point de rosée ainsi que la température du point de rosée. L'icone à trois états :
    - Bleu : pas de risque de buée.
    - Orange : point de rosée à moins de 5°c de la température ambiante, risque de buée imminente.
    - Rouge : point de rosée atteint, pare-buée obligatoire.
- Un icone affichant la valeur de Bortle en cours.
- Un espace affichant la valeur de luminosité mesurée. 2 modes d'affichage :
    - Affichage du Lux
    - Affichage du SQM (Mag / Arcsec²).\
> [!TIP]
> On passe d'un mode à l'autre (Lux / SQM) en cliquant sur le joystick.
- Un espace affichant la valeur de la température ambiante.

![Screen Home Lux](screenshots/Screen_Home_Lux.png)

![Screen Home SQM](screenshots/Screen_Home_SQM.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

#### Ecran Luminosité

![Screen Lux](screenshots/Screen_Lux.png)

L'écran Luminosité comprend les éléments suivants :
- La luminosité mesurée totale et dans les Infrarouge.
- La luminosité mesurée dans le visible.
- La quantité de Lux issue des mesures Ir et Full.
- La quantité de Lux sur une moyenne mobile. Le nombre de mesures permettant de calculer la moyenne est entre parenthèse. Le nombre maximum de valeurs pour le calcul de la moyenne mobile est de 50 (ce nombre est modifiable dans le fichier de configuration via la constante `TSL2591_MOVING_AVERAGE_COUNT`).
- Le SQM (Mag / Arcsec²) calculé à partir du Lux moyen.
- Le Bortle issu de la valeur du SQM calculé.
- Le gain du capteur lors de la dernière mesure de luminosité effectuée.
- Le temps d'exposition du capteur lors de la dernière mesure de luminosité effectuée.

> [!NOTE]
> Le gain et le temps d'exposition du capteur sont ajustés automatiquement par le firmware afin d'avoir les valeurs les plus appropriées en fonction du contexte.

> [!IMPORTANT]
> Pour réinitialiser la moyenne mobile effectuée sur la mesure de Lux, il suffit d'effectuer un **clic sur le joystick**.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

#### Ecran Environnement

![Screen Lux](screenshots/Screen_Environment.png)

L'écran Environnement comprend les éléments suivants :
- La température ambiante (°C).
- Le taux d'humidité (%).
- La pression atmosphérique (Pa).
- Un icone représentant le point de rosée ainsi que la température du point de rosée. L'icone à trois états :
    - Bleu : pas de risque de buée.
    - Orange : point de rosée à moins de 5°c de la température ambiante, risque de buée imminente.
    - Rouge : point de rosée atteint, pare-buée obligatoire.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

#### Ecran Ir Température

![Screen Lux](screenshots/Screen_IrTemp.png)

L'écran Ir Température permet d'afficher l'état du ciel. Il comprend les éléments suivants :
- Un icone représentant l'état du ciel. L'icone à trois états : 
    - Ciel clair.\
    ![Sky State Clear](screenshots/SkyState_Clear.png)
    - Peu nuageux.\
    ![Sky State Partly Cloudy](screenshots/SkyState_PartlyCloudy.png)
    - Nuageux.\
    ![Sky State Cloudy](screenshots/SkyState_Cloudy.png)
- La température ambiante mesurée.
- La température de l'objet mesurée.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

#### Ecran Niveaux

![Screen Lux](screenshots/Screen_Niveaux.png)

L'écran Niveaux permet d'afficher un niveau à bulle afin de positionner correctement le capteur de luminosité au zénith. Il comprend les éléments suivants :
- Un niveau à bulle de forme circulaire.
- L'accélération en X (m / s²).
- L'accélération en Y (m / s²).
- L'accélération en Z (m / s²).

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

#### Ecran GPS

![Screen Lux](screenshots/Screen_GPS.png)

L'écran GPS permet d'afficher les données issues du GPS. Il comprend les éléments suivants :
- Les coordonnées longitude et latitude en degrés, minutes, secondes et dizaines de secondes, orientation.
- Le nombre de satellites à partir desquels le capteur GPS récupère les informations.
- L'état "fixé" ou non.
- L'altitude courante (m).

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

## Driver ASCOM

![Driver Setup Dialog](screenshots/Driver_SetupDialog.png)

### Installation

- Téléchargez et exécutez le fichier **RemoteOpenMeteoV2-SQM Setup.exe**

![Driver Installer screen 1](screenshots/Installer_1.png)

![Driver Installer screen 2](screenshots/Installer_2.png)

![Driver Installer screen 3](screenshots/Installer_3.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Détection du périphérique bluetooth dans Windows

Afin de pouvoir utiliser le Driver ASCOM ROM V2 - SQM depuis votre application Astro comme N.I.N.A., vous devez au préalable déclarer votre périphérique Bluetooth auprès de votre système Windows.

> [!IMPORTANT]
> Cette opération de reconnaissance de votre périphérique Bluetooth auprès de votre système Windows n'est à faire qu'une seule fois, lors de la première utilisation.

![Windows bluetooth](screenshots/Windows%20bluetooth.png)

Dans la fenêtre des paramètres windows, allez dans la section "***Bluetooth et appareils***", si nécessaire, cliquez sur "***Afficher d'autres périphériques***", puis cliquez sur le bouton "***Ajouter un appareil***" et suivez les étapes de l'assistant.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

### Utilisation dans NINA

![NINA](screenshots/NINA%20Connected.png)

#### Sélection du ROM V2 dans NINA

Afin de sélectionner le ROM V2 - SQM dans N.I.N.A., allez dans l'onglet "***Equipements***", et dans le sous-onglet "***Météo***".\
Dans la liste déroulante, sélectionnez "***ROM V2 - Sky Quality Meter***"

![NINA Sélection ROM](screenshots/NINA%20Select%20ROMV2.png)

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

#### Première utilisation dans NINA

Lors de la première utilisation dans N.I.N.A., un appairage du ROM V2 - SQM est nécessaire.\
Pour cela, cliquez sur le bouton "***Paramètres***".

![NINA ROM settings](screenshots/NINA%20Toolbar.png)

Dans la boîte de dialogue qui s'ouvre, sélectionnez le périphérique "***REMOTE OPEN METEO***" détecté puis validez.

![Driver Setup Dialog](screenshots/Driver_SetupDialog.png)

> [!NOTE]
> Voilà, l'appairage est effectué. Il ne sera plus nécessaire d'effectuer cette opération lors des prochaines utilisations.

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>

Voilà, vous pouvez désormais profiter des données du ROM V2 - SQM dans NINA.\
Enjoy

![NINA démo](screenshots/NINA%20demo.png)

<p align="right"><b><i>Juanito del Pepito</i></b></p>

## Retex

Voici un petit retour d'expérience des mesures de l'état de mon ciel à mon domicile :

![NINA démo](screenshots/observations_retex.png)

## Révisions

| Date | Version | Commentaires |
| --- | --- | --- |
| 17/05/2026 | 2.0.1.1 | Simplification du train optique du TSL2591 |
| 08/05/2026 | 2.0.1.1 | Lecture de la luminosité dans une tâche dédiée FreeRTOS s'exécutant en paralèlle sur le 2ème coeur de l'ESP 32 |

<p align="right"><a href="#sommaire">Retour au sommaire</a></p>
