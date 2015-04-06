using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EyeToMouseConfigurationTool {
    public partial class frmConfigTool : Form {

        ConfigModel _model;

        public frmConfigTool() {
            InitializeComponent();
        }

        private void frmConfigTool_Load(object sender, EventArgs e) {
            _model = new ConfigModel();
        }

        private void btnCalibrate_Click(object sender, EventArgs e) {
            _model.callCalibrate();
        }

        private void btnCapture_Click(object sender, EventArgs e) {
            _model.callCapture();
        }

        private void chkBxDebug_CheckStateChanged(object sender, EventArgs e) {
            _model.toggleDebugMode(chkBxDebug.Checked);
        }

        private void frmConfigTool_FormClosing(object sender, FormClosingEventArgs e) {
            _model.kill();
        }

        
    }
}
