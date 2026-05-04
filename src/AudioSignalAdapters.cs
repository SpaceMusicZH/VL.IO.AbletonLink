using System;
using NAudio.Utils;
using VL.Audio;

namespace VL.IO.AbletonLink
{
    /// <summary>
    /// Audio sink that publishes its inputs as a Link Audio channel. Wires up to
    /// VL.Audio's engine: reads samples from <c>Audio In L</c> (and optionally
    /// <c>Audio In R</c>) on each audio buffer, interleaves them, and forwards to
    /// the underlying <see cref="AbletonLinkSink"/>.
    /// </summary>
    public sealed class LinkAudioSendSignal : AudioSignal, IAudioSink
    {
        readonly SigParamDiff<string> Name = new("Name", "vvvv");
        readonly SigParam<bool> Enabled = new("Enabled", initValue: true);
        readonly SigParamAudio InputL = new("Audio In L");
        readonly SigParamAudio InputR = new("Audio In R");

        AbletonLinkSink? _sink;
        string _appliedName = string.Empty;
        float[]? _scratchL;
        float[]? _scratchR;
        float[]? _interleaved;

        public LinkAudioSendSignal()
        {
            AudioService.AddSink(this);
            EnsureSink(Name.Value ?? "vvvv");
            Name.ValueChanged = newName =>
            {
                if (_sink != null && newName != _appliedName)
                {
                    _appliedName = newName ?? string.Empty;
                    _sink.Name = _appliedName;
                }
            };
        }

        void EnsureSink(string name)
        {
            try
            {
                _sink = new AbletonLinkSink(name);
                _appliedName = name;
            }
            catch
            {
                _sink = null;
            }
        }

        public void Read(int offset, int count)
        {
            if (!Enabled.Value || _sink == null) return;

            _scratchL = BufferHelpers.Ensure(_scratchL, count);
            InputL.Read(_scratchL, 0, count);

            bool stereo = InputR.GetValue() is AudioSignal;
            int channels = stereo ? 2 : 1;

            if (stereo)
            {
                _scratchR = BufferHelpers.Ensure(_scratchR, count);
                InputR.Read(_scratchR, 0, count);

                int total = count * 2;
                _interleaved = BufferHelpers.Ensure(_interleaved, total);
                for (int i = 0; i < count; i++)
                {
                    _interleaved[i * 2] = _scratchL[i];
                    _interleaved[i * 2 + 1] = _scratchR![i];
                }
                _sink.Send(_interleaved.AsSpan(0, total), count, 2, SampleRate);
            }
            else
            {
                _sink.Send(_scratchL.AsSpan(0, count), count, 1, SampleRate);
            }
        }

        public override void Dispose()
        {
            AudioService.RemoveSink(this);
            _sink?.Dispose();
            _sink = null;
            base.Dispose();
        }
    }

    /// <summary>
    /// Audio source that subscribes to a Link Audio channel published by another peer
    /// and emits stereo samples through its two outputs. Mono channels are duplicated
    /// to both outputs; underflow produces silence.
    /// </summary>
    public sealed class LinkAudioReceiveSignal : MultiChannelSignal
    {
        readonly SigParamDiff<AbletonLinkChannel> Channel = new("Channel");

        AbletonLinkSource? _source;
        AbletonLinkChannel _appliedChannel;
        float[]? _interleaved;

        public LinkAudioReceiveSignal()
        {
            SetOutputCount(2);
            Channel.ValueChanged = OnChannelChanged;
        }

        void OnChannelChanged(AbletonLinkChannel newChannel)
        {
            if (newChannel.Equals(_appliedChannel)) return;

            _source?.Dispose();
            _source = null;
            _appliedChannel = newChannel;

            if (newChannel.IsValid)
            {
                try { _source = new AbletonLinkSource(newChannel); }
                catch { _source = null; }
            }
        }

        protected override void FillBuffers(float[][] buffers, int offset, int count)
        {
            float[] left = buffers[0];
            float[] right = buffers[1];

            if (_source == null)
            {
                left.ReadSilence(offset, count);
                right.ReadSilence(offset, count);
                return;
            }

            // The producer's channel count is whatever we last received over the
            // network; default to stereo before any buffer has arrived.
            int producerChannels = _source.ProducerChannels;
            if (producerChannels != 1 && producerChannels != 2) producerChannels = 2;

            int wantSamples = count * producerChannels;
            _interleaved = BufferHelpers.Ensure(_interleaved, wantSamples);
            int got = _source.Read(_interleaved.AsSpan(0, wantSamples), count, producerChannels);
            int gotFrames = got / producerChannels;

            if (producerChannels == 2)
            {
                for (int i = 0; i < gotFrames; i++)
                {
                    left[offset + i] = _interleaved[i * 2];
                    right[offset + i] = _interleaved[i * 2 + 1];
                }
            }
            else
            {
                for (int i = 0; i < gotFrames; i++)
                {
                    float s = _interleaved[i];
                    left[offset + i] = s;
                    right[offset + i] = s;
                }
            }

            // Pad any remaining tail with silence (under-run).
            for (int i = gotFrames; i < count; i++)
            {
                left[offset + i] = 0f;
                right[offset + i] = 0f;
            }
        }

        public override void Dispose()
        {
            _source?.Dispose();
            _source = null;
            base.Dispose();
        }
    }
}
