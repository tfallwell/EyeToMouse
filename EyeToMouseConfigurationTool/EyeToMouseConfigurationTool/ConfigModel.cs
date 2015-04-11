using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EyeToMouseConfigurationTool {
    class ConfigModel {
        //arguments
        private string _debug = "debug";
        private string _calibrate = "calibrate";
        private string _capture = "capture";
        private string _programPath;
        private bool _debugMode = false;

        private Process myProcess;
        private bool eventHandled = false;
        //public properties
        public string programPath {
            get { return _programPath; }
            set { _programPath = value; }
        }

        //constructor
        public ConfigModel(){

            string dir = Directory.GetParent(Environment.CurrentDirectory).Parent.Parent.Parent.FullName;
            //string value = dir + "\\EyeToMouse\\res\\haarcascade_frontalface_alt.xml";
            //Environment.SetEnvironmentVariable("cascadePath", value, EnvironmentVariableTarget.User);
            _programPath = dir + "\\EyeToMouse\\Debug\\EyeToMouse.exe";

            myProcess = new Process();
        }

        internal void toggleDebugMode(bool check) {
            _debugMode = check;
        }

        internal void callCalibrate() {
            startProcess(_calibrate);
        }

        internal void callCapture() {
            startProcess(_capture);
        }

        internal void stopCapture() {
            if (eventHandled) {
                myProcess.Kill();
                eventHandled = false;
            }
        }

        //private void myProcess_Exited(object sender, System.EventArgs e) {

        //    eventHandled = false;
        //    MessageBox.Show(String.Format("{0}  {1}", myProcess.ExitTime, myProcess.ExitCode));
        //}

        internal void startProcess(string arg) {
            //kill older processes
            if (eventHandled)
                myProcess.Kill();

            arg += " configTool";
            eventHandled = true;
            myProcess.StartInfo.FileName = @_programPath;
            myProcess.StartInfo.Arguments = _debugMode == true ? @arg + ' ' + _debug : @arg;
            myProcess.StartInfo.UseShellExecute = false;
            myProcess.StartInfo.CreateNoWindow = true;
            myProcess.StartInfo.Verb = "runas";
            myProcess.EnableRaisingEvents = true;
 
            //myProcess.StartInfo.RedirectStandardOutput = true;
            //myProcess.StartInfo.RedirectStandardError = true;
            //process.StartInfo.UseShellExecute = false;
            try {
                //myProcess.Exited += new EventHandler(myProcess_Exited);
                myProcess.Start();
            }
            catch (Exception e) {

            }
        }

        internal void kill() {
            try {
                myProcess.Kill();
            }
            catch (Exception e) {

            }
        }

    }
}
