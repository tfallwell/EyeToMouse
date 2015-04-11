namespace EyeToMouseConfigurationTool {
    partial class frmConfigTool {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.btnCalibrate = new System.Windows.Forms.Button();
            this.btnCapture = new System.Windows.Forms.Button();
            this.chkBxDebug = new System.Windows.Forms.CheckBox();
            this.btnStop = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnCalibrate
            // 
            this.btnCalibrate.Location = new System.Drawing.Point(87, 29);
            this.btnCalibrate.Name = "btnCalibrate";
            this.btnCalibrate.Size = new System.Drawing.Size(88, 35);
            this.btnCalibrate.TabIndex = 0;
            this.btnCalibrate.Text = "Calibrate";
            this.btnCalibrate.UseVisualStyleBackColor = true;
            this.btnCalibrate.Click += new System.EventHandler(this.btnCalibrate_Click);
            // 
            // btnCapture
            // 
            this.btnCapture.Location = new System.Drawing.Point(87, 86);
            this.btnCapture.Name = "btnCapture";
            this.btnCapture.Size = new System.Drawing.Size(88, 35);
            this.btnCapture.TabIndex = 1;
            this.btnCapture.Text = "Capture";
            this.btnCapture.UseVisualStyleBackColor = true;
            this.btnCapture.Click += new System.EventHandler(this.btnCapture_Click);
            // 
            // chkBxDebug
            // 
            this.chkBxDebug.AutoSize = true;
            this.chkBxDebug.Location = new System.Drawing.Point(87, 197);
            this.chkBxDebug.Name = "chkBxDebug";
            this.chkBxDebug.Size = new System.Drawing.Size(111, 21);
            this.chkBxDebug.TabIndex = 2;
            this.chkBxDebug.Text = "Debug Mode";
            this.chkBxDebug.UseVisualStyleBackColor = true;
            this.chkBxDebug.CheckStateChanged += new System.EventHandler(this.chkBxDebug_CheckStateChanged);
            // 
            // btnStop
            // 
            this.btnStop.Location = new System.Drawing.Point(87, 144);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(88, 35);
            this.btnStop.TabIndex = 3;
            this.btnStop.Text = "Stop";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // frmConfigTool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(282, 255);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.chkBxDebug);
            this.Controls.Add(this.btnCapture);
            this.Controls.Add(this.btnCalibrate);
            this.Name = "frmConfigTool";
            this.Text = "Config Tool";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmConfigTool_FormClosing);
            this.Load += new System.EventHandler(this.frmConfigTool_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnCalibrate;
        private System.Windows.Forms.Button btnCapture;
        private System.Windows.Forms.CheckBox chkBxDebug;
        private System.Windows.Forms.Button btnStop;
    }
}

