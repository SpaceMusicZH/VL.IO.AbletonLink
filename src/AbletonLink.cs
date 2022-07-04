#region usings
using System;

using System.Runtime.InteropServices;

#endregion usings

namespace VL.IO.AbletonLink
{
    public class AbletonLink : IDisposable
    {

        private static volatile AbletonLink singletonInstance;
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


        [DllImport("AbletonLinkDLL")]
        private static extern IntPtr CreateAbletonLink();
        private AbletonLink()
        {
            nativeInstance = CreateAbletonLink();
        }

        ~AbletonLink()
        {
            this.Dispose();
        }

        [DllImport("AbletonLinkDLL")]
        private static extern void DestroyAbletonLink(IntPtr ptr);
        public void Dispose()
        {
            singletonInstance = null;
            if (nativeInstance != IntPtr.Zero)
            {
                DestroyAbletonLink(nativeInstance);
                nativeInstance = IntPtr.Zero;
            }
        }

        [DllImport("AbletonLinkDLL")]
        private static extern void setup(IntPtr ptr, double bpm);

        private void Setup(double bpm)
        {
            setup(nativeInstance, bpm);
        }

        [DllImport("AbletonLinkDLL")]
        private static extern void setTempo(IntPtr ptr, double bpm);


        [DllImport("AbletonLinkDLL")]
        private static extern double tempo(IntPtr ptr);

        public double Tempo
        {
            get { return tempo(nativeInstance); }
            set { setTempo(nativeInstance, value); }
        }


        [DllImport("AbletonLinkDLL")]
        private static extern void setQuantum(IntPtr ptr, double quantum);


        [DllImport("AbletonLinkDLL")]
        private static extern double quantum(IntPtr ptr);

        public double Quantum
        {
            get { return quantum(nativeInstance); }
            set { setQuantum(nativeInstance, value); }
        }

        [DllImport("AbletonLinkDLL")]
        private static extern void forceBeatAtTime(IntPtr ptr, double beat);
        public void ForceBeatAtTime(double beat)
        {
            forceBeatAtTime(nativeInstance, beat);
        }


        [DllImport("AbletonLinkDLL")]
        private static extern void requestBeatAtTime(IntPtr ptr, double beat);
        public void RequestBeatAtTime(double beat)
        {
            requestBeatAtTime(nativeInstance, beat);
        }

        [DllImport("AbletonLinkDLL")]
        private static extern void enable(IntPtr ptr, bool bEnable);

        [DllImport("AbletonLinkDLL")]
        private static extern bool isEnabled(IntPtr ptr);

        public bool Enabled
        {
            get { return isEnabled(nativeInstance); }
            set { enable(nativeInstance, value); }
        }


        [DllImport("AbletonLinkDLL")]
        private static extern void enableStartStopSync(IntPtr ptr, bool bEnable);
        public void EnableStartStopSync(bool enable)
        {
            enableStartStopSync(nativeInstance, enable);
        }


        [DllImport("AbletonLinkDLL")]
        private static extern void startPlaying(IntPtr ptr);
        public void StartPlaying()
        {
            startPlaying(nativeInstance);
        }


        [DllImport("AbletonLinkDLL")]
        private static extern void stopPlaying(IntPtr ptr);
        public void StopPlaying()
        {
            stopPlaying(nativeInstance);
        }


        [DllImport("AbletonLinkDLL")]
        private static extern bool isPlaying(IntPtr ptr);
        public bool IsPlaying
        {
            get { return isPlaying(nativeInstance); }
        }


        [DllImport("AbletonLinkDLL")]
        private static extern int numPeers(IntPtr ptr);
        public int NumPeers
        {
            get { return numPeers(nativeInstance); }
        }


        [DllImport("AbletonLinkDLL")]
        private static extern void update(IntPtr ptr, out double rbeat, out double rphase, out double rtempo, out double rquantum, out double rtime, out int rnumPeers);
        public void Update(out double beat, out double phase, out double tempo, out double quantum, out double time, out int numPeers)
        {
            update(nativeInstance, out beat, out phase, out tempo, out quantum, out time, out numPeers);
        }
    }
}
