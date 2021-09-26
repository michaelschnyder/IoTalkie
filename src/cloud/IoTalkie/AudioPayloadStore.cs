﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Azure.Storage.Blobs;
using Azure.Storage.Sas;
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
            await blobClient.UploadAsync(content, true);

            return new AzureBlobPayload(messageId, filename, mimeType, content.Length);
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

        public async Task<string> GetDownloadLink(string messageId, string mimeType)
        {
            var extension = GetExtensionFromMimeType(mimeType);
            var filename = messageId + extension;

            // Create a BlobServiceClient object which will be used to create a container client
            var blobServiceClient = new BlobServiceClient(_settings.BlobStorageConnectionString);

            // Create the container and return a container client object
            var containerClient = blobServiceClient.GetBlobContainerClient(_settings.AudioMessagesContainerName);

            // Get a reference to a blob
            var targetFile = containerClient.GetBlobClient(filename);

            if (!await targetFile.ExistsAsync())
            {
                var oggSoureFile = containerClient.GetBlobClient(messageId + ".ogg");

                if (await oggSoureFile.ExistsAsync())
                {
                    var tempFileName = Path.GetTempFileName() + ".ogg";

                    await oggSoureFile.DownloadToAsync(tempFileName);
                    var mp3TargetFile = ConvertOggToMp3(tempFileName);
                    await containerClient.UploadBlobAsync(filename, File.OpenRead(mp3TargetFile));
                }
            }

            var sasBuilder = new BlobSasBuilder(BlobSasPermissions.Read, expiresOn: DateTimeOffset.UtcNow.AddDays(60))
            {
                StartsOn = DateTimeOffset.UtcNow.AddMinutes(-5),
                BlobContainerName = _settings.AudioMessagesContainerName,
                BlobName = filename
            };

            var downloadUrl = targetFile.GenerateSasUri(sasBuilder);

            return downloadUrl.ToString();
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