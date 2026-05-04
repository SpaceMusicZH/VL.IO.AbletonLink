using System;

namespace VL.IO.AbletonLink
{
    public readonly record struct AbletonLinkChannel(ulong Id, string Name, ulong PeerId, string PeerName)
    {
        public override string ToString() => $"{PeerName} | {Name}";

        public string IdHex => Id.ToString("x16");
        public string PeerIdHex => PeerId.ToString("x16");

        public bool IsValid => Id != 0;

        public static AbletonLinkChannel Empty => default;
    }
}
