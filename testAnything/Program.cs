using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace testAnything
{
    class Program
    {
        static void Main(string[] args)
        {
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = args[0];
            startInfo.Arguments = args[1];
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;
            Process proc = new Process();
            proc.StartInfo = startInfo;
            proc.Start();
            proc.WaitForExit();
        }
    }
}
