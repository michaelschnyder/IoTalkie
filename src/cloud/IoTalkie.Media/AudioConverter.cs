using System;
using System.IO;
using Concentus.Enums;
using Concentus.Oggfile;
using Concentus.Structs;
using NAudio.Lame;
using NAudio.Wave;
using NAudio.Wave.SampleProviders;

namespace IoTalkie.Media
{
    public class AudioConverter
    {
        public string ConvertOggToMp3(string fileOgg)
        {
            var tempFile = Path.GetTempFileName();
            var fileMp3 = tempFile + ".mp3";

            using (FileStream fileIn = new FileStream($"{fileOgg}", FileMode.Open))
            using (MemoryStream pcmStream = new MemoryStream())
            {
                OpusDecoder decoder = OpusDecoder.Create(48000, 1);
                OpusOggReadStream oggIn = new OpusOggReadStream(decoder, fileIn);
                while (oggIn.HasNextPacket)
                {
                    short[] packet = oggIn.DecodeNextPacket();
                    if (packet != null)
                    {
                        for (int i = 0; i < packet.Length; i++)
                        {
                            var bytes = BitConverter.GetBytes(packet[i]);
                            pcmStream.Write(bytes, 0, bytes.Length);
                        }
                    }
                }

                pcmStream.Position = 0;
                var wavStream = new RawSourceWaveStream(pcmStream, new WaveFormat(48000, 1));

                pcmStream.Position = 0;
                using (var writer = new LameMP3FileWriter(fileMp3, wavStream.WaveFormat, 48000))
                {
                    pcmStream.CopyTo(writer);
                }

            }

            var tempFileName = fileMp3;

            return tempFileName;
        }

        public Stream ConvertWavToOgg(Stream waveMemoryStream)
        {
            waveMemoryStream = UpsampleWave(waveMemoryStream, 48000);

            var opusTargetStream = new PersistentMemoryStream();

            var anySampleRateReader = new WaveFileReader(waveMemoryStream);
            var writer = new OpusOggWriteStream(new OpusEncoder(48000, 1, OpusApplication.OPUS_APPLICATION_AUDIO), opusTargetStream);

            for (int i = 0; i < anySampleRateReader.SampleCount; i++)
            {
                var frame = anySampleRateReader.ReadNextSampleFrame();

                if (frame != null)
                {
                    writer.WriteSamples(frame, 0, frame.Length);
                }
            }

            writer.Finish();

            opusTargetStream.Close();

            var ms = new MemoryStream();
            opusTargetStream.Position = 0;
            opusTargetStream.CopyTo(ms);
            ms.Position = 0;
            return ms;
        }

        private static MemoryStream UpsampleWave(Stream waveMemoryStream, int outRate)
        {
            var outStream = new MemoryStream();

            using (var reader = new WaveFileReader(waveMemoryStream))
            {
                var resampler = new WdlResamplingSampleProvider(reader.ToSampleProvider(), outRate);
                WaveFileWriter.WriteWavFileToStream(outStream, resampler.ToWaveProvider());
            }

            outStream.Position = 0;
            return outStream;
        }
    }
}