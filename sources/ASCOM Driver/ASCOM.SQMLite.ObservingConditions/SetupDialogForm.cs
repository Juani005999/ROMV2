using ASCOM.SQMLite.Properties;
using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.Advertisement;

namespace ASCOM.SQMLite
{
    /// <summary>
    /// Formulaire des paramètres du périphérique ASCOM SQMLite
    /// <para>Pas d'enregistrement du composant pour le formulaire</para>
    /// </summary>
    [ComVisible(false)]
    public partial class SetupDialogForm : Form
    {
        #region Champs

        /// <summary>
        /// Instance de l'objet <see cref="ObservingConditions"/> en cours
        /// </summary>
        readonly ObservingConditions sqmliteObject;

        /// <summary>
        /// Objet <see cref="BluetoothLEAdvertisementWatcher"/> permettant la lecture des Advertising
        /// </summary>
        BluetoothLEAdvertisementWatcher watcher;

        /// <summary>
        /// Adresse du périphérique BLE
        /// </summary>
        ulong bleDeviceAddress;

        /// <summary>
        /// Flag de fermeture de la fenêtre
        /// </summary>
        bool formClosing = false;

        #endregion

        #region Constructeur

        /// <summary>
        /// Constructeur
        /// </summary>
        /// <param name="tlDriver"></param>
        public SetupDialogForm(ObservingConditions sqmliteObject)
        {
            InitializeComponent();

            // Valorisation des objets interne
            this.sqmliteObject = sqmliteObject;

            // Initialisation du formulaire
            InitUI();
        }

        #endregion

        #region Méthodes Private

        /// <summary>
        /// Initialisation du formulaire
        /// </summary>
        private void InitUI()
        {
            // Update des membres internes
            chkTrace.Checked = sqmliteObject.tl.Enabled;
            bleDeviceAddress = sqmliteObject.bleDeviceAddress;

            // Clear de la liste des périphériques
            listBoxBLEDevices.Items.Clear();
            listBoxBLEDevices.SelectedIndex = -1;
            listBoxBLEDevices.ClearSelected();

            // Internationalisation des libellés
            labelTitre.Text = Resources.DriverASCOMPourModuleSQMLite;
            labelAuthor.Text = Resources.JuanitoDelPepito;
            groupBoxParametres.Text = Resources.ParametresDuPeripheriqueSQMLite;
            labelBLEDevice.Text = Resources.PortSerie;
            chkTrace.Text = Resources.ModeTraceActive;
            Version version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
            labelVersion.Text = $"Version {version.Major}.{version.Minor}.{version.MajorRevision}.{version.MinorRevision}";
            linkLabelGitHub.Text = Resources.GithubDuSQMLite;

            // Actualisation de l'affichage si un périphérique est connecté
            if (sqmliteObject.Connected)
            {
                chkTrace.Enabled = false;
                listBoxBLEDevices.Enabled = false;
            }
            else
            {
                StartAdvertisingWatcher();
            }
            cmdOK.Enabled = false;
        }

        /// <summary>
        /// Renvoi l'adresse formatée
        /// </summary>
        /// <param name="address"></param>
        /// <returns></returns>
        /// <exception cref="Exception"></exception>
        private string getFormattedBluetoothAddress(ulong address)
        {
            string hexValue = address.ToString("X");
            if (hexValue.Length == 12)
            {
                return hexValue.Insert(10, ":").Insert(8, ":").Insert(6, ":").Insert(4, ":").Insert(2, ":");
            }
            throw new Exception("Addresse Bluetooth incorrecte : " + address);
        }

        /// <summary>
        /// Démarrage du watcher d'Advertising
        /// </summary>
        private void StartAdvertisingWatcher()
        {
            // Création du watcher en charge de scanner les Advertising BLE reçus
            watcher = new BluetoothLEAdvertisementWatcher
            {
                ScanningMode = BluetoothLEScanningMode.Active
            };

            // Ajout d'un évennement sur la réception d'un Avertising
            watcher.Received += async (w, args) =>
            {
                // Lecture et parcours des UUIDs
                var uuids = args.Advertisement.ServiceUuids;
                foreach (var uuid in uuids)
                {
                    // Si l'Advertising reçu correspond au périphérique BLE ROMV2
                    if (uuid.Equals(ObservingConditions.BLE_SERVICE_UUID))
                    {
                        // On arrête le watcher
                        watcher.Stop();

                        // Lecture des propriétés du périphérique BLE
                        BluetoothLEDevice bleDevice = await BluetoothLEDevice.FromBluetoothAddressAsync(args.BluetoothAddress);

                        if (bleDevice != null)
                        {
                            if (!formClosing)
                            {
                                // Ajout du périphérique dans la Combo
                                listBoxBLEDevices.Invoke(new Action(() =>
                                {
                                    // On vérifie que le périphérique n'a pas déjà été ajouté
                                    bool found = listBoxBLEDevices.Items.Cast<ListBoxItem>().Any(x => x.BluetoothAddress == args.BluetoothAddress);

                                    // Ajout du nouvel Item
                                    if (!found)
                                    {
                                        ListBoxItem item = new ListBoxItem
                                        {
                                            Name = bleDevice.Name,
                                            Text = $"{bleDevice.Name} [{getFormattedBluetoothAddress(args.BluetoothAddress)}]",
                                            BluetoothAddress = args.BluetoothAddress
                                        };
                                        listBoxBLEDevices.Items.Add(item);
                                    }
                                }));
                            }
                        }
                    }
                }
            };

            watcher.Start();
        }

        /// <summary>
        /// Click sur le lien Github
        /// </summary>
        private void VisitLink()
        {
            // Change la couleur du lien visité
            linkLabelGitHub.LinkVisited = true;

            // Appel l'URL dans l'explorateur par défaut
            System.Diagnostics.Process.Start("https://github.com/Juani005999/SQMLite");
        }

        #endregion

        private void cmdOK_Click(object sender, EventArgs e)
        {
            try
            {
                // Positionnement des membres internes
                sqmliteObject.tl.Enabled = chkTrace.Checked;
                sqmliteObject.bleDeviceAddress = bleDeviceAddress;
            }
            catch (Exception err)
            {
                // Trace
                sqmliteObject.tl.LogMessage("SetupDialogForm", "cmdOK_Click ERROR : " + err.Message);
            }
        }

        private void cmdCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void listBoxBLEDevices_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Actualisation des valeurs sélectionés
            if (listBoxBLEDevices.SelectedIndex != -1)
            {
                ListBoxItem item = listBoxBLEDevices.SelectedItem as ListBoxItem;
                bleDeviceAddress = item.BluetoothAddress;
            }
            else
            {
                bleDeviceAddress = 0;
            }
            cmdOK.Enabled = listBoxBLEDevices.SelectedIndex != -1;
        }

        private void SetupDialogForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            formClosing = true;
            if (watcher != null)
            {
                watcher.Stop();
            }
        }

        private void listBoxBLEDevices_Click(object sender, EventArgs e)
        {
            cmdOK.Enabled = listBoxBLEDevices.SelectedIndex != -1;
        }

        private void linkLabelGitHub_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            try
            {
                VisitLink();
            }
            catch (Exception err)
            {
                // Trace
                sqmliteObject.tl.LogMessage("SetupDialogForm", "linkLabelGitHub_LinkClicked ERROR : " + err.Message);
            }
        }
    }

    class ListBoxItem
    {
        public string Text { get; set; }
        public ulong BluetoothAddress { get; set; }
        public string Name { get; set; }

        public override string ToString()
        {
            return Text;
        }
    }
}