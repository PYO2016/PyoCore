using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PyoCore;

namespace Csharp_Import
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("C# import DLL examples\n");
            PyoCore.PyoCore.helloWorld();
            Console.WriteLine();

            StringBuilder str1 = new StringBuilder();
            PyoCore.PyoCore.importTestA("ANSI-test-안시-테스트", PyoCore.ImageFileType.IMAGE_FILE_TYPE_PNG, str1);
            Console.WriteLine(str1 + "\n");

            StringBuilder str2 = new StringBuilder();
            PyoCore.PyoCore.importTestW("WIDE-test-와이드-테스트", PyoCore.ImageFileType.IMAGE_FILE_TYPE_PNG, str2);
            Console.WriteLine(str2 + "\n");

            int errorCode = PyoCore.PyoCore.getErrorCode();
            Console.WriteLine("ErrorCode : " + errorCode);
        }
    }
}
