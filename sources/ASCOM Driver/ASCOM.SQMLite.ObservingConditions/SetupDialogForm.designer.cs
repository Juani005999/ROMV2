namespace ASCOM.SQMLite
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
            this.groupBoxParametres = new System.Windows.Forms.GroupBox();
            this.listBoxBLEDevices = new System.Windows.Forms.ListBox();
            this.labelBLEDevice = new System.Windows.Forms.Label();
            this.cmdCancel = new System.Windows.Forms.Button();
            this.cmdOK = new System.Windows.Forms.Button();
            this.linkLabelGitHub = new System.Windows.Forms.LinkLabel();
            this.labelAuthor = new System.Windows.Forms.Label();
            this.labelVersion = new System.Windows.Forms.Label();
            this.labelTitre = new System.Windows.Forms.Label();
            this.pictureBoxROM = new System.Windows.Forms.PictureBox();
            this.chkTrace = new System.Windows.Forms.CheckBox();
            this.groupBoxParametres.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxROM)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBoxParametres
            // 
            this.groupBoxParametres.Controls.Add(this.listBoxBLEDevices);
            this.groupBoxParametres.Controls.Add(this.labelBLEDevice);
            this.groupBoxParametres.Location = new System.Drawing.Point(14, 88);
            this.groupBoxParametres.Margin = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.groupBoxParametres.Name = "groupBoxParametres";
            this.groupBoxParametres.Padding = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.groupBoxParametres.Size = new System.Drawing.Size(415, 152);
            this.groupBoxParametres.TabIndex = 21;
            this.groupBoxParametres.TabStop = false;
            this.groupBoxParametres.Text = "Paramètres du périphérique SQMLite";
            // 
            // listBoxBLEDevices
            // 
            this.listBoxBLEDevices.FormattingEnabled = true;
            this.listBoxBLEDevices.ItemHeight = 16;
            this.listBoxBLEDevices.Location = new System.Drawing.Point(8, 44);
            this.listBoxBLEDevices.Margin = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.listBoxBLEDevices.Name = "listBoxBLEDevices";
            this.listBoxBLEDevices.Size = new System.Drawing.Size(399, 100);
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
            // cmdCancel
            // 
            this.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cmdCancel.Location = new System.Drawing.Point(341, 248);
            this.cmdCancel.Margin = new System.Windows.Forms.Padding(4);
            this.cmdCancel.Name = "cmdCancel";
            this.cmdCancel.Size = new System.Drawing.Size(80, 31);
            this.cmdCancel.TabIndex = 20;
            this.cmdCancel.Text = "Cancel";
            this.cmdCancel.UseVisualStyleBackColor = true;
            this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
            // 
            // cmdOK
            // 
            this.cmdOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.cmdOK.Location = new System.Drawing.Point(253, 248);
            this.cmdOK.Margin = new System.Windows.Forms.Padding(4);
            this.cmdOK.Name = "cmdOK";
            this.cmdOK.Size = new System.Drawing.Size(80, 30);
            this.cmdOK.TabIndex = 19;
            this.cmdOK.Text = "OK";
            this.cmdOK.UseVisualStyleBackColor = true;
            this.cmdOK.Click += new System.EventHandler(this.cmdOK_Click);
            // 
            // linkLabelGitHub
            // 
            this.linkLabelGitHub.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.linkLabelGitHub.Location = new System.Drawing.Point(271, 57);
            this.linkLabelGitHub.Name = "linkLabelGitHub";
            this.linkLabelGitHub.Size = new System.Drawing.Size(300, 19);
            this.linkLabelGitHub.TabIndex = 27;
            this.linkLabelGitHub.TabStop = true;
            this.linkLabelGitHub.Text = "Github du projet SQMLite";
            this.linkLabelGitHub.TextAlign = System.Drawing.ContentAlignment.TopRight;
            this.linkLabelGitHub.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelGitHub_LinkClicked);
            // 
            // labelAuthor
            // 
            this.labelAuthor.Location = new System.Drawing.Point(16, 57);
            this.labelAuthor.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelAuthor.Name = "labelAuthor";
            this.labelAuthor.Size = new System.Drawing.Size(412, 22);
            this.labelAuthor.TabIndex = 25;
            this.labelAuthor.Text = "Juanito del Pepito";
            // 
            // labelVersion
            // 
            this.labelVersion.Location = new System.Drawing.Point(16, 35);
            this.labelVersion.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelVersion.Name = "labelVersion";
            this.labelVersion.Size = new System.Drawing.Size(412, 22);
            this.labelVersion.TabIndex = 24;
            this.labelVersion.Text = "Version";
            // 
            // labelTitre
            // 
            this.labelTitre.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTitre.Location = new System.Drawing.Point(14, 9);
            this.labelTitre.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelTitre.Name = "labelTitre";
            this.labelTitre.Size = new System.Drawing.Size(471, 32);
            this.labelTitre.TabIndex = 23;
            this.labelTitre.Text = "ASCOM Driver for SQMLite";
            // 
            // pictureBoxROM
            // 
            this.pictureBoxROM.Image = global::ASCOM.SQMLite.Properties.Resources.SQMLite;
            this.pictureBoxROM.Location = new System.Drawing.Point(437, 78);
            this.pictureBoxROM.Margin = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.pictureBoxROM.Name = "pictureBoxROM";
            this.pictureBoxROM.Size = new System.Drawing.Size(134, 182);
            this.pictureBoxROM.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBoxROM.TabIndex = 26;
            this.pictureBoxROM.TabStop = false;
            // 
            // chkTrace
            // 
            this.chkTrace.AutoSize = true;
            this.chkTrace.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.chkTrace.Location = new System.Drawing.Point(14, 254);
            this.chkTrace.Margin = new System.Windows.Forms.Padding(5);
            this.chkTrace.Name = "chkTrace";
            this.chkTrace.Size = new System.Drawing.Size(136, 20);
            this.chkTrace.TabIndex = 22;
            this.chkTrace.Text = "Mode trace activé";
            this.chkTrace.UseVisualStyleBackColor = true;
            // 
            // SetupDialogForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(595, 292);
            this.Controls.Add(this.groupBoxParametres);
            this.Controls.Add(this.cmdCancel);
            this.Controls.Add(this.cmdOK);
            this.Controls.Add(this.linkLabelGitHub);
            this.Controls.Add(this.labelAuthor);
            this.Controls.Add(this.labelVersion);
            this.Controls.Add(this.labelTitre);
            this.Controls.Add(this.pictureBoxROM);
            this.Controls.Add(this.chkTrace);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SetupDialogForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "SQMLite";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.SetupDialogForm_FormClosing);
            this.groupBoxParametres.ResumeLayout(false);
            this.groupBoxParametres.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxROM)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBoxParametres;
        private System.Windows.Forms.ListBox listBoxBLEDevices;
        private System.Windows.Forms.Label labelBLEDevice;
        private System.Windows.Forms.Button cmdCancel;
        private System.Windows.Forms.Button cmdOK;
        private System.Windows.Forms.LinkLabel linkLabelGitHub;
        private System.Windows.Forms.Label labelAuthor;
        private System.Windows.Forms.Label labelVersion;
        private System.Windows.Forms.Label labelTitre;
        private System.Windows.Forms.PictureBox pictureBoxROM;
        private System.Windows.Forms.CheckBox chkTrace;
    }
}