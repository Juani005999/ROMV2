// --------------------------------------------------------------------------------
// ASCOM ObservingConditions driver for RemoteOpenMeteo
//
// Description:	ROMV2 - Remote Open Meteo V2
//              Sky Quality Meter (SQM) DIY
//
// Implements:	ASCOM ObservingConditions interface version: 6.6
// Author:		Juanito del Pepito
//
// Date			Who	        Vers	    Description
// -----------	---	        -----	    -------------------------------------------------------
// 10/04/2026	JUANITO	    6.6.0.1	    Initial edit, created from ASCOM driver template
// --------------------------------------------------------------------------------------------
//

using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using ASCOM.DeviceInterface;
using ASCOM.ROMV2.Properties;
using ASCOM.Utilities;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.Advertisement;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Storage.Streams;

namespace ASCOM.ROMV2
{
    /// <summary>
    /// ASCOM ObservingConditions Driver for RemoteOpenMeteoV2.
    /// <para>Your driver's DeviceID is ASCOM.ROMV2.ObservingConditions.</para>
    /// <para>The Guid attribute sets the CLSID for ASCOM.ROMV2.ObservingConditions.</para>
    /// <para>The ClassInterface/None attribute prevents an empty interface called.</para>
    /// <para>_ROMV2 from being created and used as the [default] interface.</para>
    /// </summary>
    [Guid("a875a8f7-4458-4a94-b05f-533beb81db88")]
    [ClassInterface(ClassInterfaceType.None)]
    public class ObservingConditions : IObservingConditions
    {
        #region Champs

        /// <summary>
        /// ASCOM DeviceID (COM ProgID) du driver.
        /// Le DeviceID est utilisé par l'application ASCOM pour accéder au driver.
        /// </summary>
        private const string driverID = "ASCOM.ROMV2.ObservingConditions";

        /// <summary>
        /// Driver description affiché dans le ASCOM Chooser.
        /// </summary>
        private const string driverDescription = "ROM V2 - Sky Quality Meter";

        /// <summary>
        /// Variables nécessaires à la persistence du Profil : Nom adresse du périphérique BLE
        /// </summary>
        private const string bleDeviceAddressProfileName = "BLE Device Address";

        /// <summary>
        /// Variables nécessaires à la persistence du Profil : Adresse par défaut du périphérique BLE
        /// </summary>
        private const ulong bleDeviceAddressDefault = 0;

        /// <summary>
        /// Variables nécessaires à la persistence du Profil : Nom de profil pour les traces
        /// </summary>
        private const string traceStateProfileName = "Trace Level";

        /// <summary>
        /// Variables nécessaires à la persistence du Profil : Trace ON/OFF par défaut
        /// </summary>
        private const string traceStateDefault = "false";

        /// <summary>
        /// Adresse du périphérique BLE actuellement conecté
        /// </summary>
        internal ulong bleDeviceAddress = bleDeviceAddressDefault;

        /// <summary>
        /// Etat de la connexion au module ROM
        /// </summary>
        private bool connectedState;

        /// <summary>
        /// Objet représentant le périphérique BLE actuellement connecté
        /// </summary>
        private BluetoothLEDevice bleDevice;

        /// <summary>
        /// Objet représentant la caractéristique Temérature du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicTemperature;

        /// <summary>
        /// Objet représentant la caractéristique Humidité du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicHumidity;

        /// <summary>
        /// Objet représentant la caractéristique Pression atmosphérique du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicPressure;

        /// <summary>
        /// Objet représentant la caractéristique Point de rosée du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicDewPoint;

        /// <summary>
        /// Objet représentant la caractéristique Couverture du ciel du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicCloudCover;

        /// <summary>
        /// Objet représentant la caractéristique Luminosité du ciel du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicSkyBrightness;

        /// <summary>
        /// Objet représentant la caractéristique Qualité du ciel du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicSkyQuality;

        /// <summary>
        /// Objet représentant la caractéristique Température du ciel du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicSkyTemperature;

        /// <summary>
        /// UUID du Service BLE
        /// </summary>
        public static Guid BLE_SERVICE_UUID = new Guid("b97ef9b6-d251-4d15-a748-53997037d0b8");

        /// <summary>
        /// UUID de la caractéristique Température
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_TEMPERATURE = new Guid("cba1d466-344c-4be3-ab3f-189f80dd7518");

        /// <summary>
        /// UUID de la caractéristique Humidité
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_HUMIDITY = new Guid("ca73b3ba-39f6-4ab3-91ae-186dc9577d99");

        /// <summary>
        /// UUID de la caractéristique Pression atmosphérique
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_PRESSURE = new Guid("fb6feb39-6b4a-4b9d-8529-ef6c311b4e0d");

        /// <summary>
        /// UUID de la caractéristique Point de rosée
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_DEWPOINT = new Guid("e121cbc7-ef2c-4924-8a87-b0d83e1a9c05");

        /// <summary>
        /// UUID de la caractéristique Couverture du ciel
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_CLOUDCOVER = new Guid("cd213e3e-94d9-4cf3-be60-03702c0b9b9b");

        /// <summary>
        /// UUID de la caractéristique Luminosité du ciel
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_SKYBRIGHTNESS = new Guid("67be4745-e3c4-4513-ab75-7bb9744214da");

        /// <summary>
        /// UUID de la caractéristique Qualité du ciel
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_SKYQUALITY = new Guid("ebd480d1-15fd-4be7-b2fb-22fd97c71ecb");

        /// <summary>
        /// UUID de la caractéristique Température du ciel
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_SKYTEMPERATURE = new Guid("9f0fc588-e1b5-4c3c-8a6a-2ce78c84166d");

        /// <summary>
        /// Valeur de la caractéristique Temperature
        /// </summary>
        private double? valueTemperature = null;

        /// <summary>
        /// Valeur de la caractéristique Humidity
        /// </summary>
        private double? valueHumidity = null;

        /// <summary>
        /// Valeur de la caractéristique Pressure
        /// </summary>
        private double? valuePressure = null;

        /// <summary>
        /// Valeur de la caractéristique DewPoint
        /// </summary>
        private double? valueDewPoint = null;

        /// <summary>
        /// Valeur de la caractéristique CloudCover
        /// </summary>
        private double? valueCloudCover = null;

        /// <summary>
        /// Valeur de la caractéristique SkyBrightness
        /// </summary>
        private double? valueSkyBrightness = null;

        /// <summary>
        /// Valeur de la caractéristique SkyQuality
        /// </summary>
        private double? valueSkyQuality = null;

        /// <summary>
        /// Valeur de la caractéristique SkyTemperature
        /// </summary>
        private double? valueSkyTemperature = null;

        /// <summary>
        /// Objet permettant d'accéder au modèle objet ASCOM.Utilities.TraceLogger
        /// </summary>
        internal TraceLogger tl;

        #endregion

        #region Constructeur

        /// <summary>
        /// Constructeur
        /// <para>Initialise une nouvelle instance de la classe <see cref="ObservingConditions"/></para>
        /// </summary>
        public ObservingConditions()
        {
            // Initialisation des objets internes
            tl = new TraceLogger("", "ROMV2");
            tl.LogMessage("ObservingConditions", "Démarage de l'initialisation");

            // Initialisation des Flags
            connectedState = false;

            // Lecture de la configuration du périphérique ROM depuis la base de registre (ASCOM)
            ReadProfile();

            // Trace
            tl.LogMessage("ObservingConditions", "Initialisation effectuée avec succès");
        }

        #endregion

        #region Méthodes publiques

        /// <summary>
        /// Ouvre la boîte de dialogue des paramètres du ROM
        /// <para>Sur OK, les paramètres sont sauvegardés en base de registre (ASCOM)</para>
        /// </summary>
        public void SetupDialog()
        {
            // Pas d'ouverture de la boîte de dialogue si la connexion est déjà établie
            if (IsConnected)
                System.Windows.Forms.MessageBox.Show(Resources.ModuleROMDejaConnecte
                                                        + Environment.NewLine
                                                        + Resources.VeuillezDeconnecterLeModuleROMPourAccederAuxParametres,
                                                    "Remote Open Meteo",
                                                    System.Windows.Forms.MessageBoxButtons.OK,
                                                    System.Windows.Forms.MessageBoxIcon.Information);
            else
            {
                // Déclaration et ouverture boîte de dialogue
                using (SetupDialogForm F = new SetupDialogForm(this))
                {
                    if (F.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                    {
                        // Ecriture dans la registry
                        WriteProfile();
                    }
                }
            }
        }

        /// <summary>Invokes the specified device-specific custom action.</summary>
        /// <param name="ActionName">A well known name agreed by interested parties that represents the action to be carried out.</param>
        /// <param name="ActionParameters">List of required parameters or an <see cref="String.Empty">Empty String</see> if none are required.</param>
        /// <returns>A string response. The meaning of returned strings is set by the driver author.
        /// <para>Suppose filter wheels start to appear with automatic wheel changers; new actions could be <c>QueryWheels</c> and <c>SelectWheel</c>. The former returning a formatted list
        /// of wheel names and the second taking a wheel name and making the change, returning appropriate values to indicate success or failure.</para>
        /// </returns>
        public string Action(string actionName, string actionParameters)
        {
            LogMessage("", "Action {0}, parameters {1} not implemented", actionName, actionParameters);
            throw new ASCOM.ActionNotImplementedException("Action " + actionName + " is not implemented by this driver");
        }

        /// <summary>
        /// Transmits an arbitrary string to the device and does not wait for a response.
        /// Optionally, protocol framing characters may be added to the string before transmission.
        /// </summary>
        /// <param name="Command">The literal command string to be transmitted.</param>
        /// <param name="Raw">
        /// if set to <c>true</c> the string is transmitted 'as-is'.
        /// If set to <c>false</c> then protocol framing characters may be added prior to transmission.
        /// </param>
        public void CommandBlind(string command, bool raw)
        {
            CheckConnected("CommandBlind");
            // TODO The optional CommandBlind method should either be implemented OR throw a MethodNotImplementedException
            // If implemented, CommandBlind must send the supplied command to the device and return immediately without waiting for a response

            throw new ASCOM.MethodNotImplementedException("CommandBlind");
        }

        /// <summary>
        /// Transmits an arbitrary string to the device and waits for a boolean response.
        /// Optionally, protocol framing characters may be added to the string before transmission.
        /// </summary>
        /// <param name="Command">The literal command string to be transmitted.</param>
        /// <param name="Raw">
        /// if set to <c>true</c> the string is transmitted 'as-is'.
        /// If set to <c>false</c> then protocol framing characters may be added prior to transmission.
        /// </param>
        /// <returns>
        /// Returns the interpreted boolean response received from the device.
        /// </returns>
        public bool CommandBool(string command, bool raw)
        {
            CheckConnected("CommandBool");
            // TODO The optional CommandBool method should either be implemented OR throw a MethodNotImplementedException
            // If implemented, CommandBool must send the supplied command to the mount, wait for a response and parse this to return a True or False value

            // string retString = CommandString(command, raw); // Send the command and wait for the response
            // bool retBool = XXXXXXXXXXXXX; // Parse the returned string and create a boolean True / False value
            // return retBool; // Return the boolean value to the client

            throw new ASCOM.MethodNotImplementedException("CommandBool");
        }

        /// <summary>
        /// Transmits an arbitrary string to the device and waits for a string response.
        /// Optionally, protocol framing characters may be added to the string before transmission.
        /// </summary>
        /// <param name="Command">The literal command string to be transmitted.</param>
        /// <param name="Raw">
        /// if set to <c>true</c> the string is transmitted 'as-is'.
        /// If set to <c>false</c> then protocol framing characters may be added prior to transmission.
        /// </param>
        /// <returns>
        /// Returns the string response received from the device.
        /// </returns>
        public string CommandString(string command, bool raw)
        {
            CheckConnected("CommandString");
            // TODO The optional CommandString method should either be implemented OR throw a MethodNotImplementedException
            // If implemented, CommandString must send the supplied command to the mount and wait for a response before returning this to the client

            throw new ASCOM.MethodNotImplementedException("CommandString");
        }

        /// <summary>
        /// Dispose the late-bound interface, if needed. Will release it via COM
        /// if it is a COM object, else if native .NET will just dereference it
        /// for GC.
        /// </summary>
        public void Dispose()
        {
            // Fermeture de la connexion
            Connected = false;

            // Clean up des objets internes
            tl.Enabled = false;
            tl.Dispose();
            tl = null;
        }

        #endregion

        #region Propriétés : Informations Driver

        /// <summary>
        /// Description du Driver
        /// </summary>
        public string Description
        {
            get
            {
                tl.LogMessage("Description Get", driverDescription);
                return driverDescription;
            }
        }

        /// <summary>
        /// Informations sur le Driver
        /// </summary>
        public string DriverInfo
        {
            get
            {
                Version version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
                string driverInfo = Resources.DriverASCOMPourModuleROM;
                tl.LogMessage("DriverInfo Get", driverInfo);
                return driverInfo;
            }
        }

        /// <summary>
        /// Version du Driver
        /// </summary>
        public string DriverVersion
        {
            get
            {
                Version version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
                string driverVersion = $"Version: {version.Major}.{version.Minor}.{version.MajorRevision}.{version.MinorRevision}";
                tl.LogMessage("DriverVersion Get", driverVersion);
                return driverVersion;
            }
        }

        /// <summary>
        /// Numéro de version de l'interface
        /// </summary>
        public short InterfaceVersion
        {
            // set by the driver wizard
            get
            {
                LogMessage("InterfaceVersion Get", "1");
                return Convert.ToInt16("1");
            }
        }

        /// <summary>
        /// Nom court
        /// </summary>
        public string Name
        {
            get
            {
                string name = "Remote Open Meteo V2";
                tl.LogMessage("Name Get", name);
                return name;
            }
        }

        /// <summary>
        /// Renvoi la liste des Custom Actions supportées par le Driver
        /// </summary>
        /// <value><see cref="ArrayList"/> de string</value>
        public ArrayList SupportedActions
        {
            get
            {
                tl.LogMessage("SupportedActions Get", "Pas de custom actions supportées. Renvoi d'une liste vide");
                return new ArrayList();
            }
        }

        #endregion

        #region Propriétés, Champs et Méthodes liés à la gestion de la connexion

        /// <summary>
        /// Etat de la connexion au port série
        /// </summary>
        public bool Connected
        {
            get
            {
                // Trace
                LogMessage("Connected", "Get {0}", IsConnected);

                // Renvoi la valeur du champ interne
                return IsConnected;
            }
            set
            {
                // Trace
                LogMessage("Connected", "Set {0}", value);

                // On ne fait rien si l'état souhaité est l'état actuel
                if (value == IsConnected)
                    return;

                // Connexion au périphérique BLE
                if (value)
                {
                    StartConnection();
                }
                // Déconnexion du périphérique BLE
                else
                {
                    CloseConnection();
                }
            }
        }

        /// <summary>
        /// Renvoi la valeur du flag interne <see cref="connectedState"/>
        /// </summary>
        private bool IsConnected
        {
            get
            {
                return connectedState;
            }
        }

        /// <summary>
        /// Vérifie l'état de la connexion au module ROM
        /// </summary>
        /// <param name="message"></param>
        private void CheckConnected(string message)
        {
            // Vérification sur valeur du flag interne
            if (!IsConnected)
            {
                throw new NotConnectedException(message);
            }
        }

        /// <summary>
        /// Lance la connexion au port série défini dans les Settings du périphérique ROM
        /// <para>Après la connexion au port série, envoi des commandes [IA] et [SET]DEBUG:OFF</para>
        /// </summary>
        /// <exception cref="DriverException"></exception>
        private void StartConnection()
        {
            try
            {
                // Trace
                LogMessage("StartConnection", "Connexion...");

                // On vérifie si un périphérique BLE est connecté
                if (bleDeviceAddress == bleDeviceAddressDefault)
                {
                    throw new ASCOM.DriverException("Vous n'avez appairé aucun périphérique");
                }

                // On place un Timeout de 15s
                using (var cts = new CancellationTokenSource(TimeSpan.FromSeconds(15)))
                {
                    try
                    {
                        connectedState = Task.Run(() => ConnectToDevice(cts.Token)).GetAwaiter().GetResult();
                        if (!connectedState)
                        {
                            throw new ASCOM.DriverException("Impossible de se connecter au périphérique BLE");
                        }

                        // Trace
                        LogMessage("StartConnection", "Connexion au périphérique {0} effectuée avec succès", bleDevice.Name);
                    }
                    catch (OperationCanceledException)
                    {
                        // Mise à jour du flag interne et throw de l'exception
                        CloseConnection();
                        throw new ASCOM.DriverException("Timeout : connexion BLE impossible après 15 secondes");
                    }
                }

            }
            catch (Exception ex)
            {
                LogMessage("StartConnection", "Erreur : {0}", ex.Message);

                // Mise à jour du flag interne et throw de l'exception
                CloseConnection();
                throw new DriverException(ex.Message);
            }
        }

        /// <summary>
        /// Fermeture de la connexion au port série
        /// </summary>
        /// <exception cref="DriverException"></exception>
        private void CloseConnection()
        {
            try
            {
                string currentDeviceName = bleDevice?.Name;

                // Trace
                LogMessage("CloseConnection", "Déconnexion...");

                // Déconnexion et Dispose du Service
                bleCharacteristicTemperature?.Service?.Session?.Dispose();
                bleCharacteristicTemperature?.Service?.Dispose();

                // RAZ des caractéristiques
                bleCharacteristicTemperature = null;
                bleCharacteristicHumidity = null;
                bleCharacteristicPressure = null;
                bleCharacteristicDewPoint = null;
                bleCharacteristicCloudCover = null;
                bleCharacteristicSkyBrightness = null;
                bleCharacteristicSkyQuality = null;
                bleCharacteristicSkyTemperature = null;

                // Dispose du périphérique BLE
                bleDevice?.Dispose();
                bleDevice = null;

                // Mise à jour du flag interne
                connectedState = false;

                // Trace
                LogMessage("CloseConnection", "Déconnexion du périphérique {0} effectuée avec succès", currentDeviceName);
            }
            catch (Exception ex)
            {
                LogMessage("CloseConnection", "Erreur : {0}", ex.Message);

                // Mise à jour du flag interne
                connectedState = false;
            }
        }

        /// <summary>
        /// Connection au Périphérique BLE
        /// </summary>
        /// <returns></returns>
        private Task<bool> ConnectToDevice(CancellationToken cancellationToken)
        {
            // Déclaration de la tâche chargée de la connexion
            var tcs = new TaskCompletionSource<bool>();

            // Création d'un watcher en charge de scanner les Advertising BLE reçus
            var watcher = new BluetoothLEAdvertisementWatcher()
            {
                ScanningMode = BluetoothLEScanningMode.Active
            };

            // Filtrer directement par UUID de service dans le watcher
            var filter = new BluetoothLEAdvertisementFilter();
            filter.Advertisement.ServiceUuids.Add(BLE_SERVICE_UUID);
            watcher.AdvertisementFilter = filter;

            // Flag pour éviter que le watcher re-rentre pendant les await
            bool isConnecting = false;

            // Annulation via le token => on stoppe le watcher et on termine la tâche
            cancellationToken.Register(() =>
            {
                if (!tcs.Task.IsCompleted)
                {
                    watcher.Stop();
                    // TrySetCanceled au lieu de TrySetResult(false)
                    tcs.TrySetCanceled();
                }
            });

            // Ajout d'un évennement sur la réception d'un Avertising
            watcher.Received += async (w, args) =>
            {
                // Guard : déjà en cours de connexion ou terminé
                if (isConnecting || tcs.Task.IsCompleted) return;
                isConnecting = true;

                // On arrête le watcher
                watcher.Stop();

                // Trace
                LogMessage("StartConnection", "ROMV2 BLE Device trouvé. Tentative de connexion ...");

                try
                {
                    // Vérification du token avant chaque opération longue
                    cancellationToken.ThrowIfCancellationRequested();

                    // Connection au ROMV2 BLE Device
                    bleDevice = await BluetoothLEDevice.FromBluetoothAddressAsync(args.BluetoothAddress);
                    if (bleDevice == null)
                    {
                        tcs.TrySetException(new ASCOM.DriverException("Could not connect to device."));
                        return;
                    }

                    // Vérification du token avant chaque opération longue
                    cancellationToken.ThrowIfCancellationRequested();

                    // Connection au service
                    GattDeviceServicesResult servicesResult = await bleDevice.GetGattServicesAsync(BluetoothCacheMode.Uncached);
                    if (servicesResult.Status != GattCommunicationStatus.Success)
                    {
                        tcs.TrySetResult(false);
                        return;
                    }

                    // On parcours les services
                    var services = servicesResult.Services;
                    foreach (var service in services)
                    {
                        // BLE Service non correspondant au ROMV2
                        if (!service.Uuid.Equals(BLE_SERVICE_UUID)) continue;

                        // Trace
                        LogMessage("StartConnection", "ROMV2 BLE Service trouvé.");

                        // Vérification du token avant chaque opération longue
                        cancellationToken.ThrowIfCancellationRequested();

                        // Lecture des caractéristiques du service ROMV2
                        var characteristicsResult = await service.GetCharacteristicsAsync(BluetoothCacheMode.Uncached);
                        if (characteristicsResult.Status != GattCommunicationStatus.Success)
                        {
                            tcs.TrySetResult(false);
                            return;
                        }

                        // On parcours les caractéristiques
                        var characteristics = characteristicsResult.Characteristics;
                        foreach (var characteristic in characteristics)
                        {
                            // Caractéristiques Température
                            if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_TEMPERATURE))
                            {
                                bleCharacteristicTemperature = characteristic;
                                LogMessage("StartConnection", $"Caractéristique {characteristic.UserDescription} trouvée.");
                            }
                            // Caractéristiques Humidité
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_HUMIDITY))
                            {
                                bleCharacteristicHumidity = characteristic;
                                LogMessage("StartConnection", $"Caractéristique {characteristic.UserDescription} trouvée.");
                            }
                            // Caractéristiques Pression atmosphérique
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_PRESSURE))
                            {
                                bleCharacteristicPressure = characteristic;
                                LogMessage("StartConnection", $"Caractéristique {characteristic.UserDescription} trouvée.");
                            }
                            // Caractéristiques Point de rosée
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_DEWPOINT))
                            {
                                bleCharacteristicDewPoint = characteristic;
                                LogMessage("StartConnection", $"Caractéristique {characteristic.UserDescription} trouvée.");
                            }
                            // Caractéristiques Couverture du ciel
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_CLOUDCOVER))
                            {
                                bleCharacteristicCloudCover = characteristic;
                                LogMessage("StartConnection", $"Caractéristique {characteristic.UserDescription} trouvée.");
                            }
                            // Caractéristiques Luminosité du ciel
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_SKYBRIGHTNESS))
                            {
                                bleCharacteristicSkyBrightness = characteristic;
                                LogMessage("StartConnection", $"Caractéristique {characteristic.UserDescription} trouvée.");
                            }
                            // Caractéristiques Qualité du ciel
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_SKYQUALITY))
                            {
                                bleCharacteristicSkyQuality = characteristic;
                                LogMessage("StartConnection", $"Caractéristique {characteristic.UserDescription} trouvée.");
                            }
                            // Caractéristiques Température du ciel
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_SKYTEMPERATURE))
                            {
                                bleCharacteristicSkyTemperature = characteristic;
                                LogMessage("StartConnection", $"Caractéristique {characteristic.UserDescription} trouvée.");
                            }
                        }

                        // On vérifie que toutes les caractéristiques ont été détectées
                        if (bleCharacteristicTemperature != null
                            && bleCharacteristicHumidity != null
                            && bleCharacteristicPressure != null
                            && bleCharacteristicDewPoint != null
                            && bleCharacteristicCloudCover != null
                            && bleCharacteristicSkyBrightness != null
                            && bleCharacteristicSkyQuality != null
                            && bleCharacteristicSkyTemperature != null)
                        {
                            tcs.TrySetResult(true);
                        }
                        else
                        {
                            bleCharacteristicTemperature = null;
                            bleCharacteristicHumidity = null;
                            bleCharacteristicPressure = null;
                            bleCharacteristicDewPoint = null;
                            bleCharacteristicCloudCover = null;
                            bleCharacteristicSkyBrightness = null;
                            bleCharacteristicSkyQuality = null;
                            bleCharacteristicSkyTemperature = null;
                            bleDevice?.Dispose();
                            bleDevice = null;
                            tcs.TrySetResult(false);
                        }
                        return;
                    }

                    // Aucun service correspondant trouvé
                    tcs.TrySetResult(false);
                }
                catch (OperationCanceledException)
                {
                    // Le token a expiré pendant un await => on laisse le Register gérer
                    tcs.TrySetCanceled();
                }
                catch (Exception ex)
                {
                    tcs.TrySetException(ex);
                }
            };

            // Démarrage du watcher
            watcher.Start();

            // Renvoi de la tâche en cours
            return tcs.Task;
        }

        #endregion

        #region IObservingConditions Implementation

        /// <summary>
        /// Gets and sets the time period over which observations will be averaged
        /// </summary>
        public double AveragePeriod
        {
            get
            {
                LogMessage("AveragePeriod", "get - 0");
                return 0;
            }
            set
            {
                LogMessage("AveragePeriod", "set - {0}", value);
                if (value != 0)
                    throw new InvalidValueException("AveragePeriod", value.ToString(), "0 only");
            }
        }

        /// <summary>
        /// Amount of sky obscured by cloud
        /// </summary>
        public double CloudCover
        {
            get
            {
                try
                {
                    Stopwatch sw = Stopwatch.StartNew();

                    // Vérif de connexion et de présence d'un module ROM
                    CheckConnected("CloudCover");

                    // Lecture de la valeur
                    if (ReadCharacteristicValue(bleCharacteristicCloudCover).GetAwaiter().GetResult() && valueCloudCover.HasValue)
                    {
                        LogMessage("CloudCover", $"Valeur : {valueCloudCover.Value} - Lecture en {sw.ElapsedMilliseconds} ms");
                        return valueCloudCover.Value;
                    }

                    // Pas de réponse valide
                    throw new InvalidValueException();
                }
                catch (InvalidValueException)
                {
                    throw;
                }
                catch (Exception ex)
                {
                    LogMessage("CloudCover", "Erreur : {0}", ex.Message);

                    // Mise à jour du flag interne et throw de l'exception
                    CloseConnection();
                    throw new DriverException(ex.Message);
                }
            }
        }

        /// <summary>
        /// Atmospheric dew point at the observatory in deg C
        /// </summary>
        public double DewPoint
        {
            get
            {
                try
                {
                    Stopwatch sw = Stopwatch.StartNew();

                    // Vérif de connexion et de présence d'un module ROM
                    CheckConnected("DewPoint");

                    // Lecture de la valeur
                    if (ReadCharacteristicValue(bleCharacteristicDewPoint).GetAwaiter().GetResult() && valueDewPoint.HasValue)
                    {
                        LogMessage("DewPoint", $"Valeur : {valueDewPoint.Value} - Lecture en {sw.ElapsedMilliseconds} ms");
                        return valueDewPoint.Value;
                    }

                    // Pas de réponse valide
                    throw new InvalidValueException();
                }
                catch (InvalidValueException)
                {
                    throw;
                }
                catch (Exception ex)
                {
                    LogMessage("Point de rosée", "Erreur : {0}", ex.Message);

                    // Mise à jour du flag interne et throw de l'exception
                    CloseConnection();
                    throw new DriverException(ex.Message);
                }
            }
        }

        /// <summary>
        /// Atmospheric relative humidity at the observatory in percent
        /// </summary>
        public double Humidity
        {
            get
            {
                try
                {
                    Stopwatch sw = Stopwatch.StartNew();

                    // Vérif de connexion et de présence d'un module ROM
                    CheckConnected("Humidity");

                    // Lecture de la valeur
                    if (ReadCharacteristicValue(bleCharacteristicHumidity).GetAwaiter().GetResult() && valueHumidity.HasValue)
                    {
                        LogMessage("Humidity", $"Valeur : {valueHumidity.Value} - Lecture en {sw.ElapsedMilliseconds} ms");
                        return valueHumidity.Value;
                    }

                    // Pas de réponse valide
                    throw new InvalidValueException();
                }
                catch (InvalidValueException)
                {
                    throw;
                }
                catch (Exception ex)
                {
                    LogMessage("Humidité", "Erreur : {0}", ex.Message);

                    // Mise à jour du flag interne et throw de l'exception
                    CloseConnection();
                    throw new DriverException(ex.Message);
                }
            }
        }

        /// <summary>
        /// Atmospheric pressure at the observatory in hectoPascals (hPa)
        /// </summary>
        public double Pressure
        {
            get
            {
                try
                {
                    Stopwatch sw = Stopwatch.StartNew();

                    // Vérif de connexion et de présence d'un module ROM
                    CheckConnected("Pressure");

                    // Lecture de la valeur
                    if (ReadCharacteristicValue(bleCharacteristicPressure).GetAwaiter().GetResult() && valuePressure.HasValue)
                    {
                        LogMessage("Pressure", $"Valeur : {valuePressure.Value} - Lecture en {sw.ElapsedMilliseconds} ms");
                        return valuePressure.Value;
                    }

                    // Pas de réponse valide
                    throw new InvalidValueException();
                }
                catch (InvalidValueException)
                {
                    throw;
                }
                catch (Exception ex)
                {
                    LogMessage("Pression atmosphérique", "Erreur : {0}", ex.Message);

                    // Mise à jour du flag interne et throw de l'exception
                    CloseConnection();
                    throw new DriverException(ex.Message);
                }
            }
        }

        /// <summary>
        /// Rain rate at the observatory, in millimeters per hour
        /// </summary>
        public double RainRate
        {
            get
            {
                LogMessage("RainRate", "get - not implemented");
                throw new PropertyNotImplementedException("RainRate", false);
            }
        }

        /// <summary>
        /// Forces the driver to immediately query its attached hardware to refresh sensor
        /// values
        /// </summary>
        public void Refresh()
        {
            throw new MethodNotImplementedException();
        }

        /// <summary>
        /// Provides a description of the sensor providing the requested property
        /// </summary>
        /// <param name="propertyName">Name of the property whose sensor description is required</param>
        /// <returns>The sensor description string</returns>
        public string SensorDescription(string propertyName)
        {
            switch (propertyName.Trim().ToLowerInvariant())
            {
                case "averageperiod":
                    return "Average period in hours, immediate values are only available";
                case "humidity":
                    return "Taux d'humidité";
                case "pressure":
                    return "Pression atmosphérique";
                case "temperature":
                    return "Température ambiante";
                case "cloudcover":
                    return "Couverture du ciel";
                case "dewpoint":
                    return "Point de rosée";
                case "rainrate":
                case "skybrightness":
                    return "Luminosité du ciel";
                case "skyquality":
                    return "Qualité du ciel";
                case "skytemperature":
                    return "Température du ciel";
                case "starfwhm":
                case "winddirection":
                case "windgust":
                case "windspeed":
                    // Throw an exception on the properties that are not implemented
                    LogMessage("SensorDescription", $"Property {propertyName} is not implemented");
                    throw new MethodNotImplementedException($"SensorDescription - Property {propertyName} is not implemented");
                default:
                    LogMessage("SensorDescription", $"Invalid sensor name: {propertyName}");
                    throw new InvalidValueException($"SensorDescription - Invalid property name: {propertyName}");
            }
        }

        /// <summary>
        /// Sky brightness at the observatory, in Lux (lumens per square meter)
        /// </summary>
        public double SkyBrightness
        {
            get
            {
                try
                {
                    Stopwatch sw = Stopwatch.StartNew();

                    // Vérif de connexion et de présence d'un module ROM
                    CheckConnected("SkyBrightness");

                    // Lecture de la valeur
                    if (ReadCharacteristicValue(bleCharacteristicSkyBrightness).GetAwaiter().GetResult() && valueSkyBrightness.HasValue)
                    {
                        LogMessage("SkyBrightness", $"Valeur : {valueSkyBrightness.Value} - Lecture en {sw.ElapsedMilliseconds} ms");
                        return valueSkyBrightness.Value;
                    }

                    // Pas de réponse valide
                    throw new InvalidValueException();
                }
                catch (InvalidValueException)
                {
                    throw;
                }
                catch (Exception ex)
                {
                    LogMessage("SkyBrightness", "Erreur : {0}", ex.Message);

                    // Mise à jour du flag interne et throw de l'exception
                    CloseConnection();
                    throw new DriverException(ex.Message);
                }
            }
        }

        /// <summary>
        /// Sky quality at the observatory, in magnitudes per square arc-second
        /// </summary>
        public double SkyQuality
        {
            get
            {
                try
                {
                    Stopwatch sw = Stopwatch.StartNew();

                    // Vérif de connexion et de présence d'un module ROM
                    CheckConnected("SkyQuality");

                    // Lecture de la valeur
                    if (ReadCharacteristicValue(bleCharacteristicSkyQuality).GetAwaiter().GetResult() && valueSkyQuality.HasValue)
                    {
                        LogMessage("SkyQuality", $"Valeur : {valueSkyQuality.Value} - Lecture en {sw.ElapsedMilliseconds} ms");
                        return valueSkyQuality.Value;
                    }

                    // Pas de réponse valide
                    throw new InvalidValueException();
                }
                catch (InvalidValueException)
                {
                    throw;
                }
                catch (Exception ex)
                {
                    LogMessage("SkyQuality", "Erreur : {0}", ex.Message);

                    // Mise à jour du flag interne et throw de l'exception
                    CloseConnection();
                    throw new DriverException(ex.Message);
                }
            }
        }

        /// <summary>
        /// Seeing at the observatory, measured as the average star full width half maximum (FWHM in arc secs) 
        /// within a star field
        /// </summary>
        public double StarFWHM
        {
            get
            {
                LogMessage("StarFWHM", "get - not implemented");
                throw new PropertyNotImplementedException("StarFWHM", false);
            }
        }

        /// <summary>
        /// Sky temperature at the observatory in deg C
        /// </summary>
        public double SkyTemperature
        {
            get
            {
                try
                {
                    Stopwatch sw = Stopwatch.StartNew();

                    // Vérif de connexion et de présence d'un module ROM
                    CheckConnected("SkyTemperature");

                    // Lecture de la valeur
                    if (ReadCharacteristicValue(bleCharacteristicSkyTemperature).GetAwaiter().GetResult() && valueSkyTemperature.HasValue)
                    {
                        LogMessage("SkyTemperature", $"Valeur : {valueSkyTemperature.Value} - Lecture en {sw.ElapsedMilliseconds} ms");
                        return valueSkyTemperature.Value;
                    }

                    // Pas de réponse valide
                    throw new InvalidValueException();
                }
                catch (InvalidValueException)
                {
                    throw;
                }
                catch (Exception ex)
                {
                    LogMessage("SkyTemperature", "Erreur : {0}", ex.Message);

                    // Mise à jour du flag interne et throw de l'exception
                    CloseConnection();
                    throw new DriverException(ex.Message);
                }
            }
        }

        /// <summary>
        /// Temperature at the observatory in deg C
        /// </summary>
        public double Temperature
        {
            get
            {
                try
                {
                    Stopwatch sw = Stopwatch.StartNew();

                    // Vérif de connexion et de présence d'un module ROM
                    CheckConnected("Temperature");

                    // Lecture de la valeur
                    if (ReadCharacteristicValue(bleCharacteristicTemperature).GetAwaiter().GetResult() && valueTemperature.HasValue)
                    {
                        LogMessage("Temperature", $"Valeur : {valueTemperature.Value} - Lecture en {sw.ElapsedMilliseconds} ms");
                        return valueTemperature.Value;
                    }

                    // Pas de réponse valide
                    throw new InvalidValueException();
                }
                catch (InvalidValueException)
                {
                    throw;
                }
                catch (Exception ex)
                {
                    LogMessage("Temperature", "Erreur : {0}", ex.Message);

                    // Mise à jour du flag interne et throw de l'exception
                    CloseConnection();
                    throw new DriverException(ex.Message);
                }
            }
        }

        /// <summary>
        /// Provides the time since the sensor value was last updated
        /// </summary>
        /// <param name="propertyName">Name of the property whose time since last update Is required</param>
        /// <returns>Time in seconds since the last sensor update for this property</returns>
        public double TimeSinceLastUpdate(string propertyName)
        {
            // Test for an empty property name, if found, return the time since the most recent update to any sensor
            if (!string.IsNullOrEmpty(propertyName))
            {
                switch (propertyName.Trim().ToLowerInvariant())
                {
                    // Return the time for properties that are implemented, otherwise fall through to the MethodNotImplementedException
                    case "averageperiod":
                    case "cloudcover":
                    case "dewpoint":
                    case "humidity":
                    case "pressure":
                    case "rainrate":
                    case "skybrightness":
                    case "skyquality":
                    case "skytemperature":
                    case "starfwhm":
                    case "temperature":
                    case "winddirection":
                    case "windgust":
                    case "windspeed":
                        // Throw an exception on the properties that are not implemented
                        LogMessage("TimeSinceLastUpdate", $"Property {propertyName} is not implemented");
                        throw new MethodNotImplementedException($"TimeSinceLastUpdate - Property {propertyName} is not implemented");
                    default:
                        LogMessage("TimeSinceLastUpdate", $"Invalid sensor name: {propertyName}");
                        throw new InvalidValueException($"TimeSinceLastUpdate - Invalid property name: {propertyName}");
                }
            }

            // Return the time since the most recent update to any sensor
            LogMessage("TimeSinceLastUpdate", $"The time since the most recent sensor update is not implemented");
            throw new MethodNotImplementedException("TimeSinceLastUpdate(" + propertyName + ")");
        }

        /// <summary>
        /// Wind direction at the observatory in degrees
        /// </summary>
        public double WindDirection
        {
            get
            {
                LogMessage("WindDirection", "get - not implemented");
                throw new PropertyNotImplementedException("WindDirection", false);
            }
        }

        /// <summary>
        /// Peak 3 second wind gust at the observatory over the last 2 minutes in m/s
        /// </summary>
        public double WindGust
        {
            get
            {
                LogMessage("WindGust", "get - not implemented");
                throw new PropertyNotImplementedException("WindGust", false);
            }
        }

        /// <summary>
        /// Wind speed at the observatory in m/s
        /// </summary>
        public double WindSpeed
        {
            get
            {
                LogMessage("WindSpeed", "get - not implemented");
                throw new PropertyNotImplementedException("WindSpeed", false);
            }
        }

        #endregion

        #region Méthodes privées utilitaires

        /// <summary>
        /// Lecture asynchrone de la valeur d'une caractéristique
        /// </summary>
        private async Task<bool> ReadCharacteristicValue(GattCharacteristic characteristic)
        {
            // Trace
            LogMessage("ReadCharacteristicValue", "Lecture de la valeur de la caracteristique {0}", characteristic.UserDescription);

            // Lecture de la valeur d la caractéristique
            GattReadResult result = await characteristic?.ReadValueAsync(BluetoothCacheMode.Uncached);
            if (result.Status == GattCommunicationStatus.Success)
            {
                // Caractéristiques Température
                if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_TEMPERATURE))
                {
                    return ParseCharacteristicValue(result, characteristic.UserDescription, ref valueTemperature);
                }
                // Caractéristiques Humidité
                else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_HUMIDITY))
                {
                    return ParseCharacteristicValue(result, characteristic.UserDescription, ref valueHumidity);
                }
                // Caractéristiques Pression atmosphérique
                else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_PRESSURE))
                {
                    return ParseCharacteristicValue(result, characteristic.UserDescription, ref valuePressure);
                }
                // Caractéristiques Point de rosée
                else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_DEWPOINT))
                {
                    return ParseCharacteristicValue(result, characteristic.UserDescription, ref valueDewPoint);
                }
                // Caractéristiques Couverture du ciel
                else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_CLOUDCOVER))
                {
                    return ParseCharacteristicValue(result, characteristic.UserDescription, ref valueCloudCover);
                }
                // Caractéristiques Luminosité du ciel
                else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_SKYBRIGHTNESS))
                {
                    return ParseCharacteristicValue(result, characteristic.UserDescription, ref valueSkyBrightness);
                }
                // Caractéristiques Qualité du ciel
                else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_SKYQUALITY))
                {
                    return ParseCharacteristicValue(result, characteristic.UserDescription, ref valueSkyQuality);
                }
                // Caractéristiques Température du ciel
                else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_SKYTEMPERATURE))
                {
                    return ParseCharacteristicValue(result, characteristic.UserDescription, ref valueSkyTemperature);
                }
            }

            return false;
        }

        /// <summary>
        /// Parse d'une lecture de caractéristique
        /// </summary>
        /// <param name="result"></param>
        /// <param name="CharacteristicUserDescription"></param>
        /// <param name="finalValue"></param>
        /// <returns></returns>
        private bool ParseCharacteristicValue(GattReadResult result, string CharacteristicUserDescription, ref double? finalValue)
        {
            try
            {
                byte[] status = new byte[result.Value.Length];
                DataReader.FromBuffer(result.Value).ReadBytes(status);
                string valueStr = System.Text.Encoding.Default.GetString(status);
                finalValue = Convert.ToDouble(valueStr, CultureInfo.InvariantCulture);

                LogMessage("ReadCharacteristicValue", $"Lecture caracteristique {CharacteristicUserDescription}, valeur = {finalValue}");
                return true;
            }
            catch (Exception ex)
            {
                finalValue = null;
                LogMessage("ReadCharacteristicValue", $"Erreur lecture caracteristique {CharacteristicUserDescription} : {ex.Message}");
                return false;
            }
        }

        /// <summary>
        /// Lis la configuration du périphérique dans la base de registre (ASCOM)
        /// </summary>
        internal void ReadProfile()
        {
            using (Profile driverProfile = new Profile())
            {
                driverProfile.DeviceType = "ObservingConditions";

                try
                {
                    tl.Enabled = Convert.ToBoolean(driverProfile.GetValue(driverID, traceStateProfileName, string.Empty, traceStateDefault));
                    bleDeviceAddress = ulong.Parse(driverProfile.GetValue(driverID, bleDeviceAddressProfileName, string.Empty, bleDeviceAddressDefault.ToString()));
                }
                catch (Exception ex)
                {
                    LogMessage("ReadProfile", "Erreur : {0}", ex.Message);
                }
            }
        }

        /// <summary>
        /// Ecris la configuration du périphérique dans la base de registre (ASCOM)
        /// </summary>
        internal void WriteProfile()
        {
            using (Profile driverProfile = new Profile())
            {
                driverProfile.DeviceType = "ObservingConditions";
                try
                {
                    driverProfile.WriteValue(driverID, traceStateProfileName, tl.Enabled.ToString());
                    driverProfile.WriteValue(driverID, bleDeviceAddressProfileName, bleDeviceAddress.ToString());
                }
                catch (Exception ex)
                {
                    LogMessage("WriteProfile", "Erreur : {0}", ex.Message);
                }
            }
        }

        /// <summary>
        /// Trace via le TraceLogger
        /// </summary>
        /// <param name="identifier"></param>
        /// <param name="message"></param>
        /// <param name="args"></param>
        internal void LogMessage(string identifier, string message, params object[] args)
        {
            var msg = string.Format(message, args);
            tl.LogMessage(identifier, msg);
        }

        #endregion

        #region Méthode permettant le register du driver ASCOM dans le système

        // Register or unregister driver for ASCOM. This is harmless if already
        // registered or unregistered. 
        //
        /// <summary>
        /// Register or unregister the driver with the ASCOM Platform.
        /// This is harmless if the driver is already registered/unregistered.
        /// </summary>
        /// <param name="bRegister">If <c>true</c>, registers the driver, otherwise unregisters it.</param>
        private static void RegUnregASCOM(bool bRegister)
        {
            using (var P = new ASCOM.Utilities.Profile())
            {
                P.DeviceType = "ObservingConditions";
                if (bRegister)
                {
                    P.Register(driverID, driverDescription);
                }
                else
                {
                    P.Unregister(driverID);
                }
            }
        }

        /// <summary>
        /// This function registers the driver with the ASCOM Chooser and
        /// is called automatically whenever this class is registered for COM Interop.
        /// </summary>
        /// <param name="t">Type of the class being registered, not used.</param>
        /// <remarks>
        /// This method typically runs in two distinct situations:
        /// <list type="numbered">
        /// <item>
        /// In Visual Studio, when the project is successfully built.
        /// For this to work correctly, the option <c>Register for COM Interop</c>
        /// must be enabled in the project settings.
        /// </item>
        /// <item>During setup, when the installer registers the assembly for COM Interop.</item>
        /// </list>
        /// This technique should mean that it is never necessary to manually register a driver with ASCOM.
        /// </remarks>
        [ComRegisterFunction]
        public static void RegisterASCOM(Type t)
        {
            RegUnregASCOM(true);
        }

        /// <summary>
        /// This function unregisters the driver from the ASCOM Chooser and
        /// is called automatically whenever this class is unregistered from COM Interop.
        /// </summary>
        /// <param name="t">Type of the class being registered, not used.</param>
        /// <remarks>
        /// This method typically runs in two distinct situations:
        /// <list type="numbered">
        /// <item>
        /// In Visual Studio, when the project is cleaned or prior to rebuilding.
        /// For this to work correctly, the option <c>Register for COM Interop</c>
        /// must be enabled in the project settings.
        /// </item>
        /// <item>During uninstall, when the installer unregisters the assembly from COM Interop.</item>
        /// </list>
        /// This technique should mean that it is never necessary to manually unregister a driver from ASCOM.
        /// </remarks>
        [ComUnregisterFunction]
        public static void UnregisterASCOM(Type t)
        {
            RegUnregASCOM(false);
        }

        #endregion
    }
}
