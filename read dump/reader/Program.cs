using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace reader
{
    class Program
    {
        static void Main(string[] args)
        {
            var conv = new byte[26];

            using (Stream fs = new FileStream("dump.txt", FileMode.Open))
            {
                using (TextReader reader = new StreamReader(fs))
                {
                    var str = reader.ReadLine();

                    var bytes = str.Trim().Split(new[] { ' ' });
                    

                    for (var i = 0; i < 26; i++)
                    {
                        conv[i] = Convert.ToByte(bytes[i].Trim(), 16);
                    }
                }
            }
            var frame = Frame.Decode(conv);

            var protocol = Multiprotocol.GetProtocol(frame.Protocol);
           
            Console.WriteLine($"Header:\t\t{frame.Header}");
            Console.WriteLine($"Protocol:\t{frame.Protocol}\t{protocol.Name}");
            Console.WriteLine($"SubProtocol:\t{frame.SubProtocol}\t{protocol.SubProtocols[frame.SubProtocol]}");
            Console.WriteLine($"Option:\t\t{frame.Option}");
            Console.WriteLine($"RxNum:\t\t{frame.RxNum}");
            Console.WriteLine($"Bindstate:\t{frame.BindBit}");
            Console.WriteLine($"RangeCheck:\t{frame.RangeCheckBit}");
            Console.WriteLine($"Autobind:\t{frame.AutoBindBit}");
            Console.WriteLine($"Low Power:\t{frame.Power}");
            Console.WriteLine();
            var c = 0;
            foreach (var ch in frame.Channels)
            {
                c++;
                Console.WriteLine($"Channel {c}:\t{ch}");
            }

            Console.ReadKey();
        }
    }

    class Frame
    {
        private Frame()
        { 
        }

        public static Frame Decode(byte[] bytes)
        {
            var frame = new Frame();

            frame.Header = string.Format("{0:x2}", bytes[0]);
            frame.Protocol = (byte) ((bytes[0] & 1) == 0 ? (bytes[1] & 0x1F) + 32 : (bytes[1] & 0x1F));
            frame.BindBit = (bytes[1] & 0x80) > 0;
            frame.AutoBindBit = (bytes[1] & 0x40) > 0;
            frame.RangeCheckBit = (bytes[1] & 0x20) > 0;
            frame.RxNum = bytes[2] & 0x0F;
            frame.Power = (bytes[2] & 0x10) > 0;
            frame.SubProtocol = bytes[2] >> 5;
            frame.Option = bytes[3];

            StringBuilder sb = new StringBuilder();
            for (var i = 4; i < 26; i++)
            {
                sb.Append(Convert.ToString(bytes[i], 2).PadLeft(8, '0'));
            }

            frame.Channels = new short[16];
            var channels = sb.ToString();
            var c = 0;
            while (channels.Length >= 11)
            {
                var chan = Convert.ToInt16(channels.Substring(0, 11), 2);
                frame.Channels[c] = chan;
                c++;

                channels = channels.Substring(11);
            }
            return frame;
        }
        public string Header { get; set; }
        public byte Protocol { get; set; }
        public bool BindBit { get; set; }
        public bool RangeCheckBit { get; set; }
        public bool AutoBindBit { get; set; }

        public int RxNum { get; set; }
        public bool Power { get; set; }
        public int SubProtocol { get; set; }
        public int Option { get; set; }
        public short[] Channels { get; set; }
    }
}

