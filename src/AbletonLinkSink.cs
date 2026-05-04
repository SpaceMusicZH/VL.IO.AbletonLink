using System;

namespace VL.IO.AbletonLink
{
    /// <summary>
    /// Publishes an audio channel to the Link session. Samples are sent only when at
    /// least one peer has subscribed; otherwise <see cref="Send"/> returns false and
    /// audio is dropped silently.
    /// </summary>
    public sealed class AbletonLinkSink : IDisposable
    {
        readonly IntPtr _native;
        int _handle;

        public AbletonLinkSink(string name, int maxNumSamples = 8192)
        {
            _native = AbletonLink.Instance.NativeHandle;
            _handle = NativeMethods.createSink(_native, name ?? "vvvv", maxNumSamples);
            if (_handle == 0)
                throw new InvalidOperationException("createSink failed (Link Audio not enabled?)");
        }

        public string Name
        {
            set
            {
                if (_handle != 0)
                    NativeMethods.setSinkName(_native, _handle, value ?? string.Empty);
            }
        }

        /// <summary>
        /// Send <paramref name="numFrames"/> frames of interleaved float32 audio (mono or
        /// stereo). Values outside [-1, 1] are clamped. Returns true on a successful
        /// commit, false when the local sink has no remote source listening or the
        /// arguments are invalid.
        /// </summary>
        public unsafe bool Send(ReadOnlySpan<float> interleaved, int numFrames, int numChannels, int sampleRate)
        {
            if (_handle == 0) return false;
            if (numChannels != 1 && numChannels != 2) return false;
            if (numFrames <= 0) return false;
            int needed = numFrames * numChannels;
            if (interleaved.Length < needed) return false;

            fixed (float* p = interleaved)
            {
                return NativeMethods.sinkSend(_native, _handle, p, numFrames, numChannels, (uint)sampleRate);
            }
        }

        public void Dispose()
        {
            if (_handle != 0)
            {
                NativeMethods.destroySink(_native, _handle);
                _handle = 0;
            }
        }
    }
}
