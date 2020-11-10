#include <IridiumSBD.h>
#include <SoftwareSerial.h>

#define SEND_BUFFER_SIZE 300
#define RECV_BUFFER_SIZE 240
#define COMMAND_SIZE 6
#define IRIDIUM_ERROR(ec) ec != ISBD_SUCCESS


// 00,00! is a no-op command
struct Command
{
    int opcode;
    int argument;

    Command() :
        opcode(0),
        argument(0)
    {
    }
};


class IridiumConnection
{

private:
    unsigned char sendBuffer[SEND_BUFFER_SIZE];
    uint32_t openSpot;
    unsigned char receiveBuffer[RECV_BUFFER_SIZE];
    IridiumSBD& modem;
    unsigned long lastTransmission;

    size_t getSendLength()
    {
        return openSpot;
    }

    void tryParseCommand(int receivedLength, Command& result)
    {
        if (receivedLength != COMMAND_SIZE)
            return;

        // initialize new string char buffer with extra space for null terminator
        char str[COMMAND_SIZE + 1];
        str[COMMAND_SIZE] = '\0';

        for (int i = 0; i < COMMAND_SIZE; i++)
        {
            str[i] = receiveBuffer[i];
        }

        String commandStr(str);
        int commaIdx = commandStr.indexOf(",");
        int exclIdx = commandStr.indexOf("!");

        if (commaIdx != -1 || exclIdx != COMMAND_SIZE - 1)
            return;

        // if toInt fails it'll return 0 by default, according to arduino docs, and 0 is no-op for us
        result.opcode = commandStr.substring(0, commaIdx).toInt();
        result.argument = commandStr.substring(commaIdx + 1, exclIdx).toInt();
    }

public:
    IridiumConnection(IridiumSBD& pModem) :
        openSpot(0),
        modem(pModem),
        lastTransmission(0)
    {
        modem.adjustSendReceiveTimeout(30);
    }

    bool isSendBufferEmpty()
    {
        return openSpot == 0;
    }

    bool isSendBufferFull()
    {
        return openSpot == SEND_BUFFER_SIZE;
    }

    IridiumConnection& writeByte(unsigned char data)
    {
        if (!isSendBufferFull())
        {
            sendBuffer[openSpot] = data;
            openSpot += 1;
        }

        return *this;
    }

    IridiumConnection& writeBytes(unsigned char bytes[], int length)
    {
        for (int i = 0; i < length; i++)
        {
            writeByte(bytes[i]);
        }

        return *this;
    }

    unsigned long timeSinceLastTransmission()
    {
        return millis() - lastTransmission;
    }

    void reset()
    {
        openSpot = 0;
    }

    Command tryTransmission()
    {
        size_t receivedLength = 0;
        Command result;

        if (IRIDIUM_ERROR(
            modem.sendReceiveSBDBinary(sendBuffer, getSendLength(), receiveBuffer, receivedLength)))
        {
            // todo do something with the error (?)
            return result;
        }

        reset();
        lastTransmission = millis();

        tryParseCommand(receivedLength, result);

        return result;
    }
};