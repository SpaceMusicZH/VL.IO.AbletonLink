using System;
using System.Collections.Generic;

namespace VL.IO.AbletonLink
{
    public class AbletonLink : IDisposable
    {
        private static volatile AbletonLink? singletonInstance;
        private IntPtr nativeInstance = IntPtr.Zero;
        private const double INITIAL_TEMPO = 120.0;

        public static AbletonLink Instance
        {
            get
            {
                if (singletonInstance == null)
                {
                    singletonInstance = new AbletonLink();
                    singletonInstance.Setup(INITIAL_TEMPO);
                }
                return singletonInstance;
            }
        }

        // Exposed to AbletonLinkSink / AbletonLinkSource for raw P/Invoke.
        internal IntPtr NativeHandle => nativeInstance;

        private AbletonLink()
        {
            nativeInstance = NativeMethods.CreateAbletonLink();
        }

        ~AbletonLink()
        {
            Dispose();
        }

        public void Dispose()
        {
            singletonInstance = null;
            if (nativeInstance != IntPtr.Zero)
            {
                NativeMethods.DestroyAbletonLink(nativeInstance);
                nativeInstance = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        private void Setup(double bpm) => NativeMethods.setup(nativeInstance, bpm);

        // ----- Existing v1 API ------------------------------------------------

        public double Tempo
        {
            get => NativeMethods.tempo(nativeInstance);
            set => NativeMethods.setTempo(nativeInstance, value);
        }

        public double Quantum
        {
            get => NativeMethods.quantum(nativeInstance);
            set => NativeMethods.setQuantum(nativeInstance, value);
        }

        public void ForceBeatAtTime(double beat) => NativeMethods.forceBeatAtTime(nativeInstance, beat);
        public void RequestBeatAtTime(double beat) => NativeMethods.requestBeatAtTime(nativeInstance, beat);

        public bool Enabled
        {
            get => NativeMethods.isEnabled(nativeInstance);
            set => NativeMethods.enable(nativeInstance, value);
        }

        public void EnableStartStopSync(bool enable) => NativeMethods.enableStartStopSync(nativeInstance, enable);
        public void StartPlaying() => NativeMethods.startPlaying(nativeInstance);
        public void StopPlaying() => NativeMethods.stopPlaying(nativeInstance);
        public bool IsPlaying => NativeMethods.isPlaying(nativeInstance);

        public int NumPeers => NativeMethods.numPeers(nativeInstance);

        public void Update(out double beat, out double phase, out double tempo, out double quantum, out double time, out int numPeers)
        {
            NativeMethods.update(nativeInstance, out beat, out phase, out tempo, out quantum, out time, out numPeers);
            CheckChannelsChanged();
        }

        // ----- Link Audio (4.0+) ---------------------------------------------

        public bool LinkAudioEnabled
        {
            get => NativeMethods.isLinkAudioEnabled(nativeInstance);
            set => NativeMethods.enableLinkAudio(nativeInstance, value);
        }

        public string PeerName
        {
            get => NativeMethods.PtrToStringUtf8(NativeMethods.peerName(nativeInstance));
            set => NativeMethods.setPeerName(nativeInstance, value ?? string.Empty);
        }

        ulong _lastChannelsCounter;
        IReadOnlyList<AbletonLinkChannel> _channels = Array.Empty<AbletonLinkChannel>();

        /// <summary>
        /// Snapshot of currently visible Link Audio channels. Refreshed automatically
        /// when peers add or remove channels; <see cref="ChannelsChanged"/> fires on the
        /// thread that called <see cref="Update"/> (typically the VL main loop).
        /// </summary>
        public IReadOnlyList<AbletonLinkChannel> Channels
        {
            get
            {
                CheckChannelsChanged();
                return _channels;
            }
        }

        public event EventHandler? ChannelsChanged;

        void CheckChannelsChanged()
        {
            var counter = NativeMethods.channelsChangeCounter(nativeInstance);
            if (counter == _lastChannelsCounter) return;
            _lastChannelsCounter = counter;

            int n = NativeMethods.refreshChannels(nativeInstance);
            var arr = new AbletonLinkChannel[n];
            for (int i = 0; i < n; i++)
            {
                if (NativeMethods.getChannel(nativeInstance, i,
                                             out var id, out var namePtr,
                                             out var peerId, out var peerNamePtr))
                {
                    arr[i] = new AbletonLinkChannel(
                        id,
                        NativeMethods.PtrToStringUtf8(namePtr),
                        peerId,
                        NativeMethods.PtrToStringUtf8(peerNamePtr));
                }
            }
            _channels = arr;
            ChannelsChanged?.Invoke(this, EventArgs.Empty);
        }
    }
}
