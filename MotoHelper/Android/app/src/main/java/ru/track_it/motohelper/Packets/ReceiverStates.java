package ru.track_it.motohelper.Packets;

public enum ReceiverStates {
    WaitingStart,
    ReceivingSize,
    ReceivingPacket,
    ReceivingTimeout,
    Processing
}
