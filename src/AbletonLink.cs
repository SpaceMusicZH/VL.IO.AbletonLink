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
                    singletonInstance.setup(INITIAL_TEMPO);
                
                }
                return singletonInstance;
            }
        }

    
        [DllImport ("AbletonLinkDLL")]
        private static extern IntPtr CreateAbletonLink();
        private AbletonLink()
        {
            nativeInstance = CreateAbletonLink();
        }

        ~AbletonLink()
        {
            this.Dispose();
        }

        [DllImport ("AbletonLinkDLL")]
        private static extern void DestroyAbletonLink(IntPtr ptr);
        public void Dispose()
        {
                singletonInstance = null;
                if (nativeInstance != IntPtr.Zero) {
                DestroyAbletonLink(nativeInstance);
                nativeInstance = IntPtr.Zero;
                
            }
        }

        [DllImport ("AbletonLinkDLL")]
        private static extern void setup(IntPtr ptr, double bpm);
    
        private void setup(double bpm)
        {
            setup(nativeInstance, bpm);
        }

        [DllImport ("AbletonLinkDLL")]
        private static extern void setTempo(IntPtr ptr, double bpm);
        public void setTempo(double bpm)
        {
            setTempo(nativeInstance, bpm);
        }

        [DllImport ("AbletonLinkDLL")]
        private static extern double tempo(IntPtr ptr);
        public double tempo()
        {
            return tempo(nativeInstance);
        }

        [DllImport ("AbletonLinkDLL")]
        private static extern void setQuantum(IntPtr ptr, double quantum);
        public void setQuantum(double quantum)
        {
            setQuantum(nativeInstance, quantum);
        }

        [DllImport ("AbletonLinkDLL")]
        private static extern double quantum(IntPtr ptr);
        public double quantum()
        {
            return quantum(nativeInstance);
        }

        [DllImport ("AbletonLinkDLL")]
        private static extern bool isEnabled(IntPtr ptr);
        public bool isEnabled()
        {
            return isEnabled(nativeInstance);
        }

        [DllImport ("AbletonLinkDLL")]
        private static extern void enable(IntPtr ptr, bool bEnable);
        public void enable(bool bEnable)
        {
            enable(nativeInstance, bEnable);
        }

        [DllImport("AbletonLinkDLL")]
        private static extern void enableStartStopSync(IntPtr ptr, bool bEnable);
        public void enableStartStopSync(bool bEnable)
        {
            enableStartStopSync(nativeInstance, bEnable);
        }

            [DllImport("AbletonLinkDLL")]
            private static extern void startPlaying(IntPtr ptr);
            public void startPlaying()
            {
                startPlaying(nativeInstance);
            }

            [DllImport("AbletonLinkDLL")]
            private static extern void stopPlaying(IntPtr ptr);
            public void stopPlaying()
            {
                stopPlaying(nativeInstance);
            }

            [DllImport("AbletonLinkDLL")]
            private static extern bool isPlaying(IntPtr ptr);
            public bool isPlaying()
            {
                return isPlaying(nativeInstance);
            }

            [DllImport ("AbletonLinkDLL")]
        private static extern int numPeers(IntPtr ptr);
        public int numPeers()
        {
            return numPeers(nativeInstance);
        }

        [DllImport ("AbletonLinkDLL")]
        private static extern void update(IntPtr ptr, out double beat, out double phase);
        public void update(out double beat, out double phase)
        {
            update(nativeInstance, out beat, out phase);
        }

    }
}
