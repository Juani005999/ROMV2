namespace ASCOM.ROMV2
{
    partial class SetupDialogForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SetupDialogForm));
            this.cmdOK = new System.Windows.Forms.Button();
            this.cmdCancel = new System.Windows.Forms.Button();
            this.groupBoxParametres = new System.Windows.Forms.GroupBox();
            this.listBoxBLEDevices = new System.Windows.Forms.ListBox();
            this.labelBLEDevice = new System.Windows.Forms.Label();
            this.labelAuthor = new System.Windows.Forms.Label();
            this.labelVersion = new System.Windows.Forms.Label();
            this.labelTitre = new System.Windows.Forms.Label();
            this.pictureBoxROM = new System.Windows.Forms.PictureBox();
            this.chkTrace = new System.Windows.Forms.CheckBox();
            this.linkLabelGitHub = new System.Windows.Forms.LinkLabel();
            this.groupBoxParametres.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxROM)).BeginInit();
            this.SuspendLayout();
            // 
            // cmdOK
            // 
            this.cmdOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.cmdOK.Location = new System.Drawing.Point(185, 250);
            this.cmdOK.Margin = new System.Windows.Forms.Padding(4);
            this.cmdOK.Name = "cmdOK";
            this.cmdOK.Size = new System.Drawing.Size(80, 30);
            this.cmdOK.TabIndex = 0;
            this.cmdOK.Text = "OK";
            this.cmdOK.UseVisualStyleBackColor = true;
            this.cmdOK.Click += new System.EventHandler(this.cmdOK_Click);
            // 
            // cmdCancel
            // 
            this.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cmdCancel.Location = new System.Drawing.Point(273, 250);
            this.cmdCancel.Margin = new System.Windows.Forms.Padding(4);
            this.cmdCancel.Name = "cmdCancel";
            this.cmdCancel.Size = new System.Drawing.Size(80, 31);
            this.cmdCancel.TabIndex = 1;
            this.cmdCancel.Text = "Cancel";
            this.cmdCancel.UseVisualStyleBackColor = true;
            this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
            // 
            // groupBoxParametres
            // 
            this.groupBoxParametres.Controls.Add(this.listBoxBLEDevices);
            this.groupBoxParametres.Controls.Add(this.labelBLEDevice);
            this.groupBoxParametres.Location = new System.Drawing.Point(17, 90);
            this.groupBoxParametres.Margin = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.groupBoxParametres.Name = "groupBoxParametres";
            this.groupBoxParametres.Padding = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.groupBoxParametres.Size = new System.Drawing.Size(336, 152);
            this.groupBoxParametres.TabIndex = 12;
            this.groupBoxParametres.TabStop = false;
            this.groupBoxParametres.Text = "Paramètres du périphérique ROM V2";
            // 
            // listBoxBLEDevices
            // 
            this.listBoxBLEDevices.FormattingEnabled = true;
            this.listBoxBLEDevices.ItemHeight = 16;
            this.listBoxBLEDevices.Location = new System.Drawing.Point(8, 44);
            this.listBoxBLEDevices.Margin = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.listBoxBLEDevices.Name = "listBoxBLEDevices";
            this.listBoxBLEDevices.Size = new System.Drawing.Size(320, 100);
            this.listBoxBLEDevices.TabIndex = 6;
            this.listBoxBLEDevices.Click += new System.EventHandler(this.listBoxBLEDevices_Click);
            this.listBoxBLEDevices.SelectedIndexChanged += new System.EventHandler(this.listBoxBLEDevices_SelectedIndexChanged);
            // 
            // labelBLEDevice
            // 
            this.labelBLEDevice.AutoSize = true;
            this.labelBLEDevice.Location = new System.Drawing.Point(9, 26);
            this.labelBLEDevice.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelBLEDevice.Name = "labelBLEDevice";
            this.labelBLEDevice.Size = new System.Drawing.Size(223, 16);
            this.labelBLEDevice.TabIndex = 5;
            this.labelBLEDevice.Text = "Liste des périphérique BLE détectés";
            // 
            // labelAuthor
            // 
            this.labelAuthor.Location = new System.Drawing.Point(19, 59);
            this.labelAuthor.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelAuthor.Name = "labelAuthor";
            this.labelAuthor.Size = new System.Drawing.Size(412, 22);
            this.labelAuthor.TabIndex = 16;
            this.labelAuthor.Text = "Juanito del Pepito";
            // 
            // labelVersion
            // 
            this.labelVersion.Location = new System.Drawing.Point(19, 37);
            this.labelVersion.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelVersion.Name = "labelVersion";
            this.labelVersion.Size = new System.Drawing.Size(412, 22);
            this.labelVersion.TabIndex = 15;
            this.labelVersion.Text = "Version";
            // 
            // labelTitre
            // 
            this.labelTitre.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTitre.Location = new System.Drawing.Point(17, 11);
            this.labelTitre.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelTitre.Name = "labelTitre";
            this.labelTitre.Size = new System.Drawing.Size(471, 32);
            this.labelTitre.TabIndex = 14;
            this.labelTitre.Text = "ASCOM Driver for Remote Open Meteo V2 - SQM";
            // 
            // pictureBoxROM
            // 
            this.pictureBoxROM.Image = ((System.Drawing.Image)(resources.GetObject("pictureBoxROM.Image")));
            this.pictureBoxROM.Location = new System.Drawing.Point(361, 80);
            this.pictureBoxROM.Margin = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.pictureBoxROM.Name = "pictureBoxROM";
            this.pictureBoxROM.Size = new System.Drawing.Size(213, 182);
            this.pictureBoxROM.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBoxROM.TabIndex = 17;
            this.pictureBoxROM.TabStop = false;
            // 
            // chkTrace
            // 
            this.chkTrace.AutoSize = true;
            this.chkTrace.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.chkTrace.Location = new System.Drawing.Point(17, 256);
            this.chkTrace.Margin = new System.Windows.Forms.Padding(5);
            this.chkTrace.Name = "chkTrace";
            this.chkTrace.Size = new System.Drawing.Size(136, 20);
            this.chkTrace.TabIndex = 13;
            this.chkTrace.Text = "Mode trace activé";
            this.chkTrace.UseVisualStyleBackColor = true;
            // 
            // linkLabelGitHub
            // 
            this.linkLabelGitHub.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.linkLabelGitHub.Location = new System.Drawing.Point(273, 59);
            this.linkLabelGitHub.Name = "linkLabelGitHub";
            this.linkLabelGitHub.Size = new System.Drawing.Size(300, 19);
            this.linkLabelGitHub.TabIndex = 18;
            this.linkLabelGitHub.TabStop = true;
            this.linkLabelGitHub.Text = "Github du ROM V2 - SQM";
            this.linkLabelGitHub.TextAlign = System.Drawing.ContentAlignment.TopRight;
            this.linkLabelGitHub.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelGitHub_LinkClicked);
            // 
            // SetupDialogForm
            // 
            this.AcceptButton = this.cmdOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cmdCancel;
            this.ClientSize = new System.Drawing.Size(585, 297);
            this.Controls.Add(this.linkLabelGitHub);
            this.Controls.Add(this.groupBoxParametres);
            this.Controls.Add(this.labelAuthor);
            this.Controls.Add(this.labelVersion);
            this.Controls.Add(this.labelTitre);
            this.Controls.Add(this.pictureBoxROM);
            this.Controls.Add(this.chkTrace);
            this.Controls.Add(this.cmdCancel);
            this.Controls.Add(this.cmdOK);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SetupDialogForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Remote Open Meteo V2 - SQM";
            this.groupBoxParametres.ResumeLayout(false);
            this.groupBoxParametres.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxROM)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button cmdOK;
        private System.Windows.Forms.Button cmdCancel;
        private System.Windows.Forms.GroupBox groupBoxParametres;
        private System.Windows.Forms.ListBox listBoxBLEDevices;
        private System.Windows.Forms.Label labelBLEDevice;
        private System.Windows.Forms.Label labelAuthor;
        private System.Windows.Forms.Label labelVersion;
        private System.Windows.Forms.Label labelTitre;
        private System.Windows.Forms.PictureBox pictureBoxROM;
        private System.Windows.Forms.CheckBox chkTrace;
        private System.Windows.Forms.LinkLabel linkLabelGitHub;
    }
}