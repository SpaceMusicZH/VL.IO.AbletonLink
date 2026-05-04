# VL.IO.AbletonLink

VL wrapper for [Ableton Link 4.0](https://github.com/Ableton/link). Synchronize vvvv to [other applications and devices](https://www.ableton.com/en/link/products) that support Ableton Link, and — new in 2.0 — stream audio between peers using **Link Audio**.

CPP wrapper code originally taken from [UnityAbletonLink](https://github.com/comoc/UnityAbletonLink) by [Akihiro Komori](https://github.com/comoc); SPSC ring buffer modeled on [Void-LinkAudio](https://github.com/gluon/Void-LinkAudio) by Julien Bayle.


## Using the library

Install via NuGet:

    nuget install VL.IO.AbletonLink

See [Managing Nugets](https://thegraybook.vvvv.org/reference/hde/managing-nugets.html) in the VL documentation for usage details.

Try it with vvvv, the visual live-programming environment for .NET — download at https://visualprogramming.net.


## What's new in 2.0 — Link Audio

[Link Audio](https://www.ableton.com/en/blog/live-12-4-is-coming/) extends Link from a tempo / beat / phase sync layer to also stream **audio** between peers on the local network, sample-accurate and beat-aligned. It shipped with [Ableton Live 12.4](https://www.ableton.com/en/live/) on May 5 2026 and is supported by Push, Move 2.0, Note, and a growing list of third-party hosts (Max, TouchDesigner, VCV Rack, Pure Data, openFrameworks).

vvvv now joins that list. The library exposes:

- **Peer Name** — the identifier other peers see for this vvvv instance.
- **Link Audio Enabled** — toggle the audio-channel announcement layer.
- **Channels** — spread of remote channels currently visible on the network.
- **Link Audio Send** — VL.Audio sink that publishes 1 or 2 channels of audio as a Link Audio channel. Other peers (Live, Max, etc.) see and can subscribe to it.
- **Link Audio Receive** — VL.Audio source that subscribes to a remote channel and emits stereo audio (mono channels are duplicated to L+R).

For best results Ableton recommends a wired Ethernet network; USB-to-Ethernet adapters work well.


## License

[![CC0](http://i.creativecommons.org/p/zero/1.0/88x31.png "CC0")](http://creativecommons.org/publicdomain/zero/1.0/deed)

<sub>Depends on [Ableton Link](https://github.com/Ableton/link); please see their [license](https://github.com/Ableton/link/blob/master/LICENSE.md) for details. The bundled SPSC ring buffer is GPL-2.0-or-later (see [src/AbletonLinkDLL/AudioRingBuffer.h](src/AbletonLinkDLL/AudioRingBuffer.h)).</sub>
