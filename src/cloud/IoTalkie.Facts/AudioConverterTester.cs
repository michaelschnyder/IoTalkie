using System.IO;
using Concentus.Oggfile;
using Concentus.Structs;
using IoTalkie.Media;
using Xunit;

namespace IoTalkie.Facts
{
    public class AudioConverterTester
    {
        [Fact]
        public void ConvertWavToOgg_NoException()
        {
            var converter = new AudioConverter();

            var result = converter.ConvertWavToOgg(File.OpenRead("CantinaBand3.wav"));
            var file = File.OpenWrite("result.ogg");
            result.CopyTo(file);
            file.Close();
        }

        [Fact]
        public void SampleFile_NoException()
        {
            var converter = new AudioConverter();

            var result = converter.ConvertWavToOgg(File.OpenRead("esp32-example-upload.wav"));
            var file = File.OpenWrite("esp32-example-upload.ogg");
            result.CopyTo(file);
            file.Close();
        }

        [Fact]
        public void ConvertWavToOgg_SameLength()
        {
            var converter = new AudioConverter();

            var result = converter.ConvertWavToOgg(File.OpenRead("CantinaBand3.wav"));

            var read = new OpusOggReadStream(new OpusDecoder(48000, 1), result);
            Assert.Equal(3, read.TotalTime.Seconds);
        }
    }
}
