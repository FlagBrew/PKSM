#include "TitleLoadScreen.hpp"

static bool saveFromBridge = false;
static struct in_addr lastIPAddr;

bool isLoadedSaveFromBridge(void)
{
    return saveFromBridge;
}
void setLoadedSaveFromBridge(bool v)
{
    saveFromBridge = false;
}

static char* getHostId()
{
    static sockaddr_in addr;
    addr.sin_addr.s_addr = gethostid();
    return inet_ntoa(addr.sin_addr);
}

bool receiveSaveFromBridge(void)
{
    if (!Gui::showChoiceMessage(i18n::localize("WIRELESS_WARNING") + '\n' + StringUtils::format(i18n::localize("WIRELESS_IP"), getHostId())))
    {
        return false;
    }

    int fd;
    struct sockaddr_in servaddr;
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_CREATE_FAIL"), errno);
        return false;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(PKSM_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_BIND_FAIL"), errno);
        close(fd);
        return false;
    }

    if (listen(fd, 5) < 0)
    {
        Gui::error(i18n::localize("SOCKET_LISTEN_FAIL"), errno);
        close(fd);
        return false;
    }

    int fdconn;
    int addrlen = sizeof(servaddr);
    if ((fdconn = accept(fd, (struct sockaddr*)&servaddr, (socklen_t*)&addrlen)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_ACCEPT_FAIL"), errno);
        close(fd);
        return false;
    }

    lastIPAddr = servaddr.sin_addr;

    size_t size = 0x100000;
    char* data  = new char[size];

    size_t total = 0;
    size_t chunk = 1024;
    int n;
    while (total < size)
    {
        size_t torecv = size - total > chunk ? chunk : size - total;
        n             = recv(fdconn, data + total, torecv, 0);
        total += n;
        if (n <= 0)
        {
            break;
        }
        fprintf(stderr, "Recv %u bytes, %u still missing\n", total, size - total);
    }

    close(fdconn);
    close(fd);

    if (n == 0 || total == size)
    {
        if (TitleLoader::load((u8*)data, total))
        {
            saveFromBridge = true;
            Gui::setScreen(std::make_unique<MainMenu>());
        }
    }
    else
    {
        Gui::error(i18n::localize("DATA_RECEIVE_FAIL"), errno);
    }

    delete[] data;
    return true;
}

bool sendSaveToBridge(void)
{
    bool result = false;
    // send via TCP
    int fd;
    struct sockaddr_in servaddr;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_CREATE_FAIL"), errno);
        return result;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(PKSM_PORT);
    servaddr.sin_addr   = lastIPAddr;

    if (connect(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_CONNECTION_FAIL"), errno);
        close(fd);
        return result;
    }

    size_t size  = TitleLoader::save->getLength();
    size_t total = 0;
    size_t chunk = 1024;
    int n;
    while (total < size)
    {
        size_t tosend = size - total > chunk ? chunk : size - total;
        n             = send(fd, TitleLoader::save->rawData() + total, tosend, 0);
        if (n == -1)
        {
            break;
        }
        total += n;
        fprintf(stderr, "Sent %u bytes, %u still missing\n", total, size - total);
    }
    if (total == size)
    {
        // Gui::createInfo("Success!", "Data sent back correctly.");
        result = true;
    }
    else
    {
        Gui::error(i18n::localize("DATA_SEND_FAIL"), errno);
    }

    close(fd);
    saveFromBridge = false;
    return result;
}

void backupBridgeChanges()
{
    char stringTime[15]   = {0};
    time_t unixTime       = time(NULL);
    struct tm* timeStruct = gmtime((const time_t*)&unixTime);
    std::strftime(stringTime, 14, "%Y%m%d%H%M%S", timeStruct);
    std::string path = "/3ds/PKSM/backups/bridge/" + std::string(stringTime) + ".bak";
    FSStream out     = FSStream(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_WRITE | FS_OPEN_CREATE, TitleLoader::save->getLength());
    if (out.good())
    {
        out.write(TitleLoader::save->rawData(), TitleLoader::save->getLength());
    }
    out.close();
}
