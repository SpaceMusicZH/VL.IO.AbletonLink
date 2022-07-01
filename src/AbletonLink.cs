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
        private static extern void Setup(IntPtr ptr, double bpm);
    
        private void Setup(double bpm)
        {
            Setup(nativeInstance, bpm);
        }


        [DllImport ("AbletonLinkDLL")]
        private static extern void setTempo(IntPtr ptr, double bpm);

   
        [DllImport ("AbletonLinkDLL")]
        private static extern double tempo(IntPtr ptr);

        public double Tempo
        {
            get { return tempo(nativeInstance); }
            set { setTempo(nativeInstance, value); }
        }


        [DllImport ("AbletonLinkDLL")]
        private static extern void setQuantum(IntPtr ptr, double quantum);


        [DllImport ("AbletonLinkDLL")]
        private static extern double quantum(IntPtr ptr);

        public double Quantum
        {
            get { return quantum(nativeInstance); }
            set { setQuantum(nativeInstance, value); }
        }


        [DllImport ("AbletonLinkDLL")]
        private static extern bool isEnabled(IntPtr ptr);

        [DllImport ("AbletonLinkDLL")]
        private static extern void enable(IntPtr ptr, bool bEnable);

        public bool Enabled
        {
            get { return isEnabled(nativeInstance); }
            set { enable(nativeInstance, value); }
        }


        [DllImport("AbletonLinkDLL")]
        private static extern void EnableStartStopSync(IntPtr ptr, bool bEnable);

        public void EnableStartStopSync(bool bEnable)
        {
            EnableStartStopSync(nativeInstance, bEnable);
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
            get { return isEnabled(nativeInstance); }
        }


        [DllImport ("AbletonLinkDLL")]
        private static extern int numPeers(IntPtr ptr);
 
        public int NumPeers
        {
            get { return numPeers(nativeInstance); }
        }


        [DllImport ("AbletonLinkDLL")]
        private static extern void Update(IntPtr ptr, out double beat, out double phase);
        public void Update(out double beat, out double phase)
        {
            Update(nativeInstance, out beat, out phase);
        }

    }
}
