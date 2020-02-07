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

            Console.WriteLine($"Header:     {frame.Header}");
            Console.WriteLine($"Protocol:   {frame.Protocol}");
            Console.WriteLine($"Bindstate:  {frame.BindBit}");
            Console.WriteLine($"RangeCheck: {frame.RangeCheckBit}");
            Console.WriteLine($"Autobind:   {frame.AutoBindBit}");
            Console.WriteLine($"RxNum:      {frame.RxNum}");
            Console.WriteLine($"Power:      {frame.Power}");
            Console.WriteLine($"SubProtocol:{frame.SubProtocol}");
            Console.WriteLine($"Option:     {frame.Option}");
            var c = 0;
            foreach (var ch in frame.Channels)
            {
                c++;
                Console.WriteLine($"Channel {c}:    {ch}");
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
            frame.Protocol = (byte)(bytes[1] & 0x1F);
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

/*
Serial: 100000 Baud 8e2      _ xxxx xxxx p --
  Total of 26 bytes
  Stream[0]   =  0x55 for protocol 0-31 0x54 for 32-63
   header
  Stream[1]   = sub_protocol|BindBit|RangeCheckBit|AutoBindBit;
   sub_protocol is 0..31 (bits 0..4) =>	
					Reserved	0
					Flysky		1
					Hubsan		2
					Frsky		3
					Hisky		4
					V2x2		5
					DSM2		6
					Devo		7
					YD717		8
					KN			9
					SymaX		10
					SLT			11
					CX10		12
					CG023		13
					Bayang		14
					FrskyX		15
					ESky		16
					MT99XX		17
					MJXQ		18
   BindBit=>		0x80	1=Bind/0=No
   AutoBindBit=>	0x40	1=Yes /0=No
   RangeCheck=>		0x20	1=Yes /0=No
  Stream[2]   = RxNum | Power | Type;
   RxNum value is 0..15 (bits 0..3)
   Type is 0..7 <<4     (bit 4..6)
		sub_protocol==Flysky
			Flysky	0
			V9x9	1
			V6x6	2
			V912	3
		sub_protocol==Hisky
			Hisky	0
			HK310	1
		sub_protocol==DSM2
			DSM2	0
			DSMX	1
		sub_protocol==YD717
			YD717	0
			SKYWLKR	1
			SYMAX4	2
			XINXUN	3
			NIHUI	4
		sub_protocol==KN
			WLTOYS	0
			FEILUN	1
		sub_protocol==SYMAX
			SYMAX	0
			SYMAX5C	1
		sub_protocol==CX10
			CX10_GREEN	0
			CX10_BLUE	1	// also compatible with CX10-A, CX12
			DM007		2
			Q282		3
			JC3015_1	4
			JC3015_2	5
			MK33041		6
			Q242		7
		sub_protocol==CG023
			CG023		0
			YD829		1
			H8_3D		2
		sub_protocol==MT99XX
			MT99		0
			H7			1
			YZ			2
		sub_protocol==MJXQ
			WLH08		0
			X600		1
			X800		2
			H26D		3
   Power value => 0x80	0=High/1=Low
  Stream[3]   = option_protocol;
   option_protocol value is -127..127
  Stream[4] to [25] = Channels
   16 Channels on 11 bits (0..2047)
	0		-125%
    204		-100%
	1024	   0%
	1843	+100%
	2047	+125%
   Channels bits are concatenated to fit in 22 bytes like in SBUS protocol
*/
