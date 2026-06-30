// --------------------------------------------------------------------------------
// ASCOM ObservingConditions driver for RemoteOpenMeteo
//
// Description:	SQMLite
//              Sky Quality Meter (SQM) DIY
//
// Implements:	ASCOM ObservingConditions interface version: 6.6
// Author:		Juanito del Pepito
//
// Date			Who	        Vers	    Description
// -----------	---	        -----	    -------------------------------------------------------
// 30/05/2026	JUANITO	    1.1.0.1	    Initial edit, created from ASCOM driver template
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
using ASCOM.SQMLite.Properties;
using ASCOM.Utilities;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.Advertisement;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Storage.Streams;

namespace ASCOM.SQMLite
{
    /// <summary>
    /// ASCOM ObservingConditions Driver for SQMLite.
    /// <para>Your driver's DeviceID is ASCOM.SQMLite.ObservingConditions.</para>
    /// <para>The Guid attribute sets the CLSID for ASCOM.SQMLite.ObservingConditions.</para>
    /// <para>The ClassInterface/None attribute prevents an empty interface called.</para>
    /// <para>_SQMLite from being created and used as the [default] interface.</para>
    /// </summary>
    [Guid("bd914017-2fc0-4831-b485-faa34bc8e6bf")]
    [ClassInterface(ClassInterfaceType.None)]
    public class ObservingConditions : IObservingConditions
    {
        #region Champs

        /// <summary>
        /// ASCOM DeviceID (COM ProgID) du driver.
        /// Le DeviceID est utilisé par l'application ASCOM pour accéder au driver.
        /// </summary>
        private const string driverID = "ASCOM.SQMLite.ObservingConditions";

        /// <summary>
        /// Driver description affiché dans le ASCOM Chooser.
        /// </summary>
        private const string driverDescription = "SQMLite - Sky Quality Meter";

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
        /// Objet représentant la caractéristique Luminosité du ciel du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicSkyBrightness;

        /// <summary>
        /// Objet représentant la caractéristique Qualité du ciel du périphérique BLE actuellement connecté
        /// </summary>
        private GattCharacteristic bleCharacteristicSkyQuality;

        /// <summary>
        /// UUID du Service BLE
        /// </summary>
        public static Guid BLE_SERVICE_UUID = new Guid("d45872d6-b482-488e-b620-a0bb4a9c4d86");

        /// <summary>
        /// UUID de la caractéristique Température
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_TEMPERATURE = new Guid("fd1839f2-f064-4f47-9e18-d212cbe538a9");

        /// <summary>
        /// UUID de la caractéristique Humidité
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_HUMIDITY = new Guid("634fb69b-62b4-4fec-bce9-bc70e6b8166c");

        /// <summary>
        /// UUID de la caractéristique Pression atmosphérique
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_PRESSURE = new Guid("b8e5b7e1-4693-40e7-adbf-7784299e3b51");

        /// <summary>
        /// UUID de la caractéristique Point de rosée
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_DEWPOINT = new Guid("41ce96a2-b73b-46be-90be-34e5dcd59c56");

        /// <summary>
        /// UUID de la caractéristique Luminosité du ciel
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_SKYBRIGHTNESS = new Guid("6fcf4961-63d5-4a3c-9b4a-60163aefa479");

        /// <summary>
        /// UUID de la caractéristique Qualité du ciel
        /// </summary>
        public static Guid BLE_CHARACTERISTIC_UUID_SKYQUALITY = new Guid("09c6e80f-55ed-4d70-bff2-62fc6ba07b3e");

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
        /// Valeur de la caractéristique SkyBrightness
        /// </summary>
        private double? valueSkyBrightness = null;

        /// <summary>
        /// Valeur de la caractéristique SkyQuality
        /// </summary>
        private double? valueSkyQuality = null;

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
            tl = new TraceLogger("", Resources.AppName);
            tl.LogMessage("ObservingConditions", Resources.DemarageDeLInitialisation);

            // Initialisation des Flags
            connectedState = false;

            // Lecture de la configuration du périphérique ROM depuis la base de registre (ASCOM)
            ReadProfile();

            // Trace
            tl.LogMessage("ObservingConditions", Resources.InitialisationEffectueeAvecSucces);
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
                System.Windows.Forms.MessageBox.Show(Resources.ModuleSQMLiteDejaConnecte
                                                        + Environment.NewLine
                                                        + Resources.VeuillezDeconnecterLeModuleSQMLitePourAccederAuxParametres,
                                                    Resources.AppName,
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
                string driverInfo = Resources.DriverASCOMPourModuleSQMLite;
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
                string name = Resources.AppName;
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
                tl.LogMessage("SupportedActions Get", Resources.PasDeCustomActionsSupporteesRenvoiDUneListeVide);
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
        /// Vérifie l'état de la connexion au module SQMLite
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
        /// Lance la connexion au SQMLite
        /// </summary>
        /// <exception cref="DriverException"></exception>
        private void StartConnection()
        {
            try
            {
                // Trace
                LogMessage("StartConnection", Resources.Connexion);

                // On vérifie si un périphérique BLE est connecté
                if (bleDeviceAddress == bleDeviceAddressDefault)
                {
                    throw new ASCOM.DriverException(Resources.VousNAvezAppaireAucunPeripherique);
                }

                // On place un Timeout de 15s
                using (var cts = new CancellationTokenSource(TimeSpan.FromSeconds(15)))
                {
                    try
                    {
                        connectedState = Task.Run(() => ConnectToDevice(cts.Token)).GetAwaiter().GetResult();
                        if (!connectedState)
                        {
                            throw new ASCOM.DriverException(Resources.ImpossibleDeSeConnecterAuPeripheriqueBLE);
                        }

                        // Trace
                        LogMessage("StartConnection", Resources.ConnexionAuPeripheriqueEffectueeAvecSucces, bleDevice.Name);
                    }
                    catch (OperationCanceledException)
                    {
                        // Mise à jour du flag interne et throw de l'exception
                        CloseConnection();
                        throw new ASCOM.DriverException(Resources.TimeoutConnexionBLEImpossibleApres15Secondes);
                    }
                }

            }
            catch (Exception ex)
            {
                LogMessage("StartConnection", Resources.ErreurX, ex.Message);

                // Mise à jour du flag interne et throw de l'exception
                CloseConnection();
                throw new DriverException(ex.Message);
            }
        }

        /// <summary>
        /// Fermeture de la connexion au SQMLite
        /// </summary>
        /// <exception cref="DriverException"></exception>
        private void CloseConnection()
        {
            try
            {
                string currentDeviceName = bleDevice?.Name;

                // Trace
                LogMessage("CloseConnection", Resources.Deconnexion);

                // Déconnexion et Dispose du Service
                bleCharacteristicTemperature?.Service?.Session?.Dispose();
                bleCharacteristicTemperature?.Service?.Dispose();

                // RAZ des caractéristiques
                bleCharacteristicTemperature = null;
                bleCharacteristicHumidity = null;
                bleCharacteristicPressure = null;
                bleCharacteristicDewPoint = null;
                bleCharacteristicSkyBrightness = null;
                bleCharacteristicSkyQuality = null;

                // Dispose du périphérique BLE
                bleDevice?.Dispose();
                bleDevice = null;

                // Mise à jour du flag interne
                connectedState = false;

                // Trace
                LogMessage("CloseConnection", Resources.DeconnexionDuPeripheriqueEffectueeAvecSucces, currentDeviceName);
            }
            catch (Exception ex)
            {
                LogMessage("CloseConnection", Resources.ErreurX, ex.Message);

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
                LogMessage("StartConnection", Resources.SQMLiteBLEDeviceTrouveTentativeDeConnexion);

                try
                {
                    // Vérification du token avant chaque opération longue
                    cancellationToken.ThrowIfCancellationRequested();

                    // Connection au ROMV2 BLE Device
                    bleDevice = await BluetoothLEDevice.FromBluetoothAddressAsync(args.BluetoothAddress);
                    if (bleDevice == null)
                    {
                        tcs.TrySetException(new ASCOM.DriverException(Resources.ImpossibleDeSeConnecterAuPeripheriqueBLE));
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
                        LogMessage("StartConnection", Resources.SQMLiteBLEServiceTrouve);

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
                                LogMessage("StartConnection", Resources.CaractéristiqueXTrouvee, characteristic.UserDescription);
                            }
                            // Caractéristiques Humidité
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_HUMIDITY))
                            {
                                bleCharacteristicHumidity = characteristic;
                                LogMessage("StartConnection", Resources.CaractéristiqueXTrouvee, characteristic.UserDescription);
                            }
                            // Caractéristiques Pression atmosphérique
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_PRESSURE))
                            {
                                bleCharacteristicPressure = characteristic;
                                LogMessage("StartConnection", Resources.CaractéristiqueXTrouvee, characteristic.UserDescription);
                            }
                            // Caractéristiques Point de rosée
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_DEWPOINT))
                            {
                                bleCharacteristicDewPoint = characteristic;
                                LogMessage("StartConnection", Resources.CaractéristiqueXTrouvee, characteristic.UserDescription);
                            }
                            // Caractéristiques Luminosité du ciel
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_SKYBRIGHTNESS))
                            {
                                bleCharacteristicSkyBrightness = characteristic;
                                LogMessage("StartConnection", Resources.CaractéristiqueXTrouvee, characteristic.UserDescription);
                            }
                            // Caractéristiques Qualité du ciel
                            else if (characteristic.Uuid.Equals(BLE_CHARACTERISTIC_UUID_SKYQUALITY))
                            {
                                bleCharacteristicSkyQuality = characteristic;
                                LogMessage("StartConnection", Resources.CaractéristiqueXTrouvee, characteristic.UserDescription);
                            }
                        }

                        // On vérifie que toutes les caractéristiques ont été détectées
                        if (bleCharacteristicTemperature != null
                            && bleCharacteristicHumidity != null
                            && bleCharacteristicPressure != null
                            && bleCharacteristicDewPoint != null
                            && bleCharacteristicSkyBrightness != null
                            && bleCharacteristicSkyQuality != null)
                        {
                            tcs.TrySetResult(true);
                        }
                        else
                        {
                            bleCharacteristicTemperature = null;
                            bleCharacteristicHumidity = null;
                            bleCharacteristicPressure = null;
                            bleCharacteristicDewPoint = null;
                            bleCharacteristicSkyBrightness = null;
                            bleCharacteristicSkyQuality = null;
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
                LogMessage("CloudCover", "get - not implemented");
                throw new PropertyNotImplementedException("CloudCover", false);
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
                        LogMessage("DewPoint", Resources.ValeurXLectureEnYMs, valueDewPoint.Value, sw.ElapsedMilliseconds);
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
                    LogMessage("Point de rosée", Resources.ErreurX, ex.Message);

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
                        LogMessage("Humidity", Resources.ValeurXLectureEnYMs, valueHumidity.Value, sw.ElapsedMilliseconds);
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
                    LogMessage("Humidité", Resources.ErreurX, ex.Message);

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
                        LogMessage("Pressure", Resources.ValeurXLectureEnYMs, valuePressure.Value, sw.ElapsedMilliseconds);
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
                    LogMessage("Pression atmosphérique", Resources.ErreurX, ex.Message);

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
                    return Resources.TauxDHumidite;

                case "pressure":
                    return Resources.PressionAtmospherique;

                case "temperature":
                    return Resources.TemperatureAmbiante;

                case "dewpoint":
                    return Resources.PointDeRosee;

                case "skybrightness":
                    return Resources.LuminositeDuCiel;

                case "skyquality":
                    return Resources.QualiteDuCiel;

                case "skytemperature":
                case "rainrate":
                case "cloudcover":
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
                        LogMessage("SkyBrightness", Resources.ValeurXLectureEnYMs, valueSkyBrightness.Value, sw.ElapsedMilliseconds);
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
                    LogMessage("SkyBrightness", Resources.ErreurX, ex.Message);

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
                        LogMessage("SkyQuality", Resources.ValeurXLectureEnYMs, valueSkyQuality.Value, sw.ElapsedMilliseconds);
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
                    LogMessage("SkyQuality", Resources.ErreurX, ex.Message);

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
                LogMessage("SkyTemperature", "get - not implemented");
                throw new PropertyNotImplementedException("SkyTemperature", false);
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
                        LogMessage("Temperature", Resources.ValeurXLectureEnYMs, valueTemperature.Value, sw.ElapsedMilliseconds);
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
                    LogMessage("Temperature", Resources.ErreurX, ex.Message);

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
            LogMessage("ReadCharacteristicValue", Resources.LectureDeLaValeurDeLaCaracteristiqueX, characteristic.UserDescription);

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

                LogMessage("ReadCharacteristicValue", Resources.LectureCaracteristiqueXValeurY, CharacteristicUserDescription, finalValue);
                return true;
            }
            catch (Exception ex)
            {
                finalValue = null;
                LogMessage("ReadCharacteristicValue", Resources.ErreurLectureCaracteristiqueXY, CharacteristicUserDescription, ex.Message);
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
                    LogMessage("ReadProfile", Resources.ErreurX, ex.Message);
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
                    LogMessage("WriteProfile", Resources.ErreurX, ex.Message);
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
