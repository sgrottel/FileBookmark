using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Diagnostics;
using Microsoft.Win32;

namespace SG.FileBookmark {

    /// <summary>
    /// Utilitiy class for UAC elevation
    /// </summary>
    static internal class Elevation {

        ///// <summary>
        ///// const for the vista shield button
        ///// </summary>
        //private const uint BCM_SETSHIELD = 0x0000160C;

        ///// <summary>
        ///// Send Message
        ///// </summary>
        //[DllImport("user32.dll", CharSet = CharSet.Unicode)]
        //private static extern IntPtr SendMessage(HandleRef hWnd, UInt32 Msg, IntPtr wParam, IntPtr lParam);

        ///// <summary>
        ///// shows the vista security shild on a button
        ///// </summary>
        ///// <param name="btn">The button to show the shield icon on</param>
        ///// <param name="show">The flag whether or not to show the shield icon</param>
        //internal static void ShowButtonShield(Button btn, bool show) {
        //    btn.FlatStyle = FlatStyle.System;
        //    SendMessage(new HandleRef(btn, btn.Handle), BCM_SETSHIELD, IntPtr.Zero, new IntPtr(show ? 1 : 0));
        //}

        /// <summary>
        /// Answer whether or not elevation is required to get write access to HKCR
        /// </summary>
        /// <returns>True if elevation is required</returns>
        static internal bool IsElevationRequired() {
            try {
                RegistryKey key = Registry.ClassesRoot.OpenSubKey("*", true);
                key.Close();
                return false;
            } catch {
            }
            return true;
        }

        /// <summary>
        /// Answers whether the user is an admin
        /// </summary>
        /// <returns>True if the user is an admin</returns>
        static internal bool IsElevated() {
            System.Security.Principal.WindowsIdentity id = System.Security.Principal.WindowsIdentity.GetCurrent();
            System.Security.Principal.WindowsPrincipal p = new System.Security.Principal.WindowsPrincipal(id);
            return p.IsInRole(System.Security.Principal.WindowsBuiltInRole.Administrator);
        }

        /// <summary>
        /// Restarts this application with "runas" verb for elevated rights
        /// </summary>
        /// <param name="cmdline">The command line arguments for the started application</param>
        /// <returns>The return code of the started application</returns>
        static internal int RestartElevated(string cmdline) {
            int rv = int.MinValue;
            ProcessStartInfo psi = new ProcessStartInfo();
            psi.UseShellExecute = true;
            psi.WorkingDirectory = Environment.CurrentDirectory;
            psi.FileName = Application.ExecutablePath;
            psi.Arguments = cmdline;
            psi.Verb = "runas";
            try {
                Process p = Process.Start(psi);
                p.WaitForExit();
                rv = p.ExitCode;
            } catch {
            }
            return rv;
        }

    }

}
