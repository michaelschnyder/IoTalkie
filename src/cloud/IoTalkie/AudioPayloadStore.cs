using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Azure.Storage.Blobs;
using Azure.Storage.Sas;
using Concentus.Enums;
using Concentus.Oggfile;
using Concentus.Structs;
using IoTalkie.Common;
using Microsoft.Azure.Amqp.Framing;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using NAudio.Lame;
using NAudio.Vorbis;
using NAudio.Wave;

namespace IoTalkie
{
    public class AudioPayloadStore
    {
        private readonly ILogger<AudioPayloadStore> _logger;
        private readonly AzureSettings _settings;

        public AudioPayloadStore(IOptions<AzureSettings> options, ILogger<AudioPayloadStore> logger)
        {
            _logger = logger;
            _settings = options.Value;
        }

        public async Task<AzureBlobPayload> Store(string messageId, Stream content, string mimeType)
        {
            // Create a BlobServiceClient object which will be used to create a container client
            BlobServiceClient blobServiceClient = new BlobServiceClient(_settings.BlobStorageConnectionString);

            // Create the container and return a container client object
            BlobContainerClient containerClient =
                blobServiceClient.GetBlobContainerClient(_settings.AudioMessagesContainerName);

            var extension = GetExtensionFromMimeType(mimeType);

            // Get a reference to a blob
            var filename = messageId + extension;

            BlobClient blobClient = containerClient.GetBlobClient(filename);

            _logger.LogInformation("Uploading to Blob storage as blob: {0}", blobClient.Uri);

            // Open the file and upload its data
            var response = await blobClient.UploadAsync(content, true);

            return new AzureBlobPayload(messageId, filename, mimeType, 0);
        }

        private static string GetExtensionFromMimeType(string mimeType)
        {
            var extension = ".audio";

            if (mimeType == "audio/ogg")
            {
                extension = ".ogg";
            }

            if (mimeType == "audio/x-wav")
            {
                extension = ".wav";
            }

            if (mimeType == "audio/mpeg")
            {
                extension = ".mp3";
            }

            return extension;
        }

        public async Task DownloadTo(string messageId, MemoryStream targetMemoryStream, string mimeType)
        {
            var file = await GetCloudFile(messageId, mimeType);
            await file.DownloadToAsync(targetMemoryStream);
        }

        public async Task<string> GetDownloadLink(string messageId, string mimeType)
        {
            var targetFile = await GetCloudFile(messageId, mimeType);

            if (!await targetFile.ExistsAsync())
            {
                _logger.LogError($"Unable to find or create target format {mimeType} for {messageId}.");
                return null;
            }

            var sasBuilder = new BlobSasBuilder(BlobSasPermissions.Read, expiresOn: DateTimeOffset.UtcNow.AddDays(60))
            {
                StartsOn = DateTimeOffset.UtcNow.AddMinutes(-5),
                BlobContainerName = _settings.AudioMessagesContainerName,
                BlobName = targetFile.Name
            };

            var downloadUrl = targetFile.GenerateSasUri(sasBuilder);

            return downloadUrl.ToString();
        }

        private async Task<BlobClient> GetCloudFile(string messageId, string mimeType)
        {
            var filename = messageId + GetExtensionFromMimeType(mimeType);

            var containerClient = CreateBlobContainerClient();

            // Get a reference to a blob
            var targetFile = containerClient.GetBlobClient(filename);

            if (!await targetFile.ExistsAsync())
            {
                var oggSourceFile = containerClient.GetBlobClient(messageId + ".ogg");
                var wavSourceFile = containerClient.GetBlobClient(messageId + ".wav");

                if (await oggSourceFile.ExistsAsync() && mimeType == "audio/mpeg")
                {
                    var tempFileName = Path.GetTempFileName() + ".ogg";

                    await oggSourceFile.DownloadToAsync(tempFileName);
                    var mp3TargetFile = ConvertOggToMp3(tempFileName);
                    await containerClient.UploadBlobAsync(filename, File.OpenRead(mp3TargetFile));
                }

                if (await wavSourceFile.ExistsAsync() && mimeType == "audio/ogg")
                {
                    var waveMemoryStream = new MemoryStream();
                    await wavSourceFile.DownloadToAsync(waveMemoryStream);
                    var oggTargetFileMemoryStream = ConvertWavToOgg(waveMemoryStream);
                    await containerClient.UploadBlobAsync(filename, oggTargetFileMemoryStream);
                }
            }

            return targetFile;
        }

        private BlobContainerClient CreateBlobContainerClient()
        {
            // Create a BlobServiceClient object which will be used to create a container client
            var blobServiceClient = new BlobServiceClient(_settings.BlobStorageConnectionString);

            // Create the container and return a container client object
            var containerClient = blobServiceClient.GetBlobContainerClient(_settings.AudioMessagesContainerName);
            return containerClient;
        }

        private Stream ConvertWavToOgg(MemoryStream waveMemoryStream)
        {
            waveMemoryStream.Position = 0;

            using WaveFileReader reader = new WaveFileReader(waveMemoryStream);
            var tempFileName = Path.GetTempFileName() + ".ogg";
            var opusFileStream = new FileStream(tempFileName, FileMode.CreateNew);
            
            var writer = new OpusOggWriteStream(new OpusEncoder(48000, 1, OpusApplication.OPUS_APPLICATION_AUDIO), opusFileStream);

            for (int i = 0; i < reader.SampleCount; i++)
            {
                var frame = reader.ReadNextSampleFrame();

                if (frame != null)
                {
                    writer.WriteSamples(frame, 0, frame.Length);
                }
            }

            writer.Finish();

            opusFileStream.Close();

            var ms = new MemoryStream();
            new FileStream(tempFileName, FileMode.Open).CopyTo(ms);
            ms.Position = 0;
            // opusMemoryStream.Position = 0;
            return ms;
        }

        private string ConvertOggToMp3(string fileOgg)
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
    }
}
