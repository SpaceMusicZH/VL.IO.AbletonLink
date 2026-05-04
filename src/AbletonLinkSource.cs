using System;

namespace VL.IO.AbletonLink
{
    /// <summary>
    /// Subscribes to an audio channel published by another peer. Received samples are
    /// converted to float32 and placed in a lock-free ring buffer; pull from it on the
    /// audio thread via <see cref="Read"/>.
    /// </summary>
    public sealed class AbletonLinkSource : IDisposable
    {
        readonly IntPtr _native;
        int _handle;

        public AbletonLinkChannel Channel { get; }

        public AbletonLinkSource(AbletonLinkChannel channel, int ringCapacitySamples = 32768)
        {
            if (!channel.IsValid)
                throw new ArgumentException("Channel is empty", nameof(channel));

            Channel = channel;
            _native = AbletonLink.Instance.NativeHandle;
            _handle = NativeMethods.createSource(_native, channel.Id, ringCapacitySamples);
            if (_handle == 0)
                throw new InvalidOperationException("createSource failed (Link Audio not enabled?)");
        }

        /// <summary>Number of float samples currently available to read.</summary>
        public int Available => _handle == 0 ? 0 : NativeMethods.sourceAvailable(_native, _handle);

        /// <summary>
        /// Channel count (1 or 2) of the most recently received buffer, or 0 if no
        /// buffer has arrived yet. Updates from the Link thread.
        /// </summary>
        public int ProducerChannels => _handle == 0 ? 0 : NativeMethods.sourceChannels(_native, _handle);

        /// <summary>Sample rate of the most recently received buffer in Hz, or 0.</summary>
        public int ProducerSampleRate => _handle == 0 ? 0 : NativeMethods.sourceSampleRate(_native, _handle);

        /// <summary>
        /// Read up to <paramref name="requestedFrames"/> * <paramref name="numChannels"/>
        /// interleaved samples into <paramref name="dest"/>. Returns the number of
        /// samples (frames × channels) actually written.
        /// </summary>
        public unsafe int Read(Span<float> dest, int requestedFrames, int numChannels)
        {
            if (_handle == 0) return 0;
            int want = requestedFrames * numChannels;
            if (dest.Length < want) want = dest.Length / numChannels * numChannels;
            if (want <= 0) return 0;

            fixed (float* p = dest)
            {
                return NativeMethods.sourceRead(_native, _handle, p, want / numChannels, numChannels);
            }
        }

        public void Dispose()
        {
            if (_handle != 0)
            {
                NativeMethods.destroySource(_native, _handle);
                _handle = 0;
            }
        }
    }
}
