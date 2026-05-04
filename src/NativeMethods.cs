using System;
using System.Runtime.InteropServices;

namespace VL.IO.AbletonLink
{
    internal static class NativeMethods
    {
        const string Lib = "AbletonLinkDLL";

        // ----- Lifecycle -----------------------------------------------------

        [DllImport(Lib)] public static extern IntPtr CreateAbletonLink();
        [DllImport(Lib)] public static extern void DestroyAbletonLink(IntPtr ptr);
        [DllImport(Lib)] public static extern void setup(IntPtr ptr, double bpm);

        [DllImport(Lib)] public static extern void setTempo(IntPtr ptr, double bpm);
        [DllImport(Lib)] public static extern double tempo(IntPtr ptr);

        [DllImport(Lib)] public static extern void setQuantum(IntPtr ptr, double quantum);
        [DllImport(Lib)] public static extern double quantum(IntPtr ptr);

        [DllImport(Lib)] public static extern void enable(IntPtr ptr, [MarshalAs(UnmanagedType.I1)] bool bEnable);
        [DllImport(Lib)] [return: MarshalAs(UnmanagedType.I1)] public static extern bool isEnabled(IntPtr ptr);

        [DllImport(Lib)] public static extern void enableStartStopSync(IntPtr ptr, [MarshalAs(UnmanagedType.I1)] bool bEnable);
        [DllImport(Lib)] public static extern void startPlaying(IntPtr ptr);
        [DllImport(Lib)] public static extern void stopPlaying(IntPtr ptr);
        [DllImport(Lib)] [return: MarshalAs(UnmanagedType.I1)] public static extern bool isPlaying(IntPtr ptr);

        [DllImport(Lib)] public static extern void forceBeatAtTime(IntPtr ptr, double beat);
        [DllImport(Lib)] public static extern void requestBeatAtTime(IntPtr ptr, double beat);

        [DllImport(Lib)] public static extern int numPeers(IntPtr ptr);

        [DllImport(Lib)]
        public static extern void update(IntPtr ptr,
                                         out double rbeat,
                                         out double rphase,
                                         out double rtempo,
                                         out double rquantum,
                                         out double rtime,
                                         out int rnumPeers);

        // ----- Link Audio (4.0+) --------------------------------------------

        [DllImport(Lib)] public static extern void enableLinkAudio(IntPtr ptr, [MarshalAs(UnmanagedType.I1)] bool bEnable);
        [DllImport(Lib)] [return: MarshalAs(UnmanagedType.I1)] public static extern bool isLinkAudioEnabled(IntPtr ptr);

        [DllImport(Lib, CharSet = CharSet.Ansi)]
        public static extern void setPeerName(IntPtr ptr, [MarshalAs(UnmanagedType.LPUTF8Str)] string utf8);

        [DllImport(Lib)]
        public static extern IntPtr peerName(IntPtr ptr);

        [DllImport(Lib)] public static extern ulong channelsChangeCounter(IntPtr ptr);
        [DllImport(Lib)] public static extern int refreshChannels(IntPtr ptr);

        [DllImport(Lib)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool getChannel(IntPtr ptr,
                                             int index,
                                             out ulong outId,
                                             out IntPtr outName,
                                             out ulong outPeerId,
                                             out IntPtr outPeerName);

        [DllImport(Lib, CharSet = CharSet.Ansi)]
        public static extern int createSink(IntPtr ptr,
                                            [MarshalAs(UnmanagedType.LPUTF8Str)] string name,
                                            int maxNumSamples);

        [DllImport(Lib, CharSet = CharSet.Ansi)]
        public static extern void setSinkName(IntPtr ptr,
                                              int sinkHandle,
                                              [MarshalAs(UnmanagedType.LPUTF8Str)] string name);

        [DllImport(Lib)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern unsafe bool sinkSend(IntPtr ptr,
                                                  int sinkHandle,
                                                  float* interleaved,
                                                  int numFrames,
                                                  int numChannels,
                                                  uint sampleRate);

        [DllImport(Lib)] public static extern void destroySink(IntPtr ptr, int sinkHandle);

        [DllImport(Lib)] public static extern int createSource(IntPtr ptr, ulong channelId, int ringCapacitySamples);

        [DllImport(Lib)]
        public static extern unsafe int sourceRead(IntPtr ptr,
                                                   int sourceHandle,
                                                   float* dest,
                                                   int requestedFrames,
                                                   int numChannels);

        [DllImport(Lib)] public static extern int sourceAvailable(IntPtr ptr, int sourceHandle);
        [DllImport(Lib)] public static extern int sourceChannels(IntPtr ptr, int sourceHandle);
        [DllImport(Lib)] public static extern int sourceSampleRate(IntPtr ptr, int sourceHandle);
        [DllImport(Lib)] public static extern void destroySource(IntPtr ptr, int sourceHandle);

        public static string PtrToStringUtf8(IntPtr p) =>
            p == IntPtr.Zero ? string.Empty : Marshal.PtrToStringUTF8(p) ?? string.Empty;
    }
}
