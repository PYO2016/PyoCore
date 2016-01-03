using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Csharp_Example
{
    class Program
    {
        static void Main(string[] args)
        {
            const int len = 10000;

            Console.WriteLine("*********** PyoCore in C# Example 1 ***********\n");
            Console.WriteLine();

            StringBuilder buffer1 = new StringBuilder(len);
            PyoCore.PyoCore.processImageFileW("example.png", PyoCore.ImageFileType.IMAGE_FILE_TYPE_PNG,
                buffer1, len, true);
            Console.WriteLine(buffer1.ToString() + "\n");

            Console.WriteLine("*********** PyoCore in C# Example 2 ***********\n");
            StringBuilder buffer2 = new StringBuilder(len);
            PyoCore.PyoCore.processImageFileA("example.png", PyoCore.ImageFileType.IMAGE_FILE_TYPE_PNG,
                buffer2, len, true);
            Console.WriteLine(buffer2.ToString() + "\n");

            int errorCode = PyoCore.PyoCore.getErrorCode();
            Console.WriteLine("ErrorCode : " + errorCode);
        }
    }
}
