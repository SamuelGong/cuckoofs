#include "test_cuckoo_store.h"

#include <future>

#include "connection/node.h"

std::shared_ptr<CuckooConfig> CuckooStoreUT::config = nullptr;
std::shared_ptr<OpenInstance> CuckooStoreUT::openInstance = nullptr;
char *CuckooStoreUT::writeBuf = nullptr;
size_t CuckooStoreUT::size = 0;
char *CuckooStoreUT::readBuf = nullptr;
size_t CuckooStoreUT::readSize = 0;
char *CuckooStoreUT::readBuf2 = nullptr;

/* ------------------------------------------- write local -------------------------------------------*/

TEST_F(CuckooStoreUT, WriteThroughLocalSame)
{
    NewOpenInstance(1000, StoreNode::GetInstance()->GetNodeId(), "/WriteLocal", O_WRONLY | O_CREAT);

    size_t size = CUCKOO_STORE_STREAM_MAX_SIZE + 1;
    char *buf = (char *)malloc(size);
    strcpy(buf, "abc");

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, 0);
    });
    EXPECT_EQ(ret1.get(), 0);
    EXPECT_EQ(ret2.get(), 0);
    auto bufferedSize = openInstance->writeStream.GetSize();
    EXPECT_EQ(bufferedSize, 0);
    EXPECT_EQ(openInstance->currentSize.load(), size);
    free(buf);
}

TEST_F(CuckooStoreUT, WriteThroughLocalDifferent)
{
    NewOpenInstance(1000, StoreNode::GetInstance()->GetNodeId(), "/WriteLocal", O_WRONLY);
    openInstance->currentSize = size;

    size_t size = CUCKOO_STORE_STREAM_MAX_SIZE + 1;
    char *buf = (char *)malloc(size);
    strcpy(buf, "abc");

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, size);
    });
    EXPECT_EQ(ret1.get(), 0);
    EXPECT_EQ(ret2.get(), 0);
    auto bufferedSize = openInstance->writeStream.GetSize();
    EXPECT_EQ(bufferedSize, 0);
    EXPECT_EQ(openInstance->currentSize.load(), size * 2);
    free(buf);
}

/* ------------------------------------------- write remote -------------------------------------------*/

TEST_F(CuckooStoreUT, WriteThroughRemoteSame)
{
    NewOpenInstance(2000, StoreNode::GetInstance()->GetNodeId() - 1, "/WriteRemote", O_WRONLY | O_CREAT);

    size_t size = CUCKOO_STORE_STREAM_MAX_SIZE + 1;
    char *buf = (char *)malloc(size);
    strcpy(buf, "abc");

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, 0);
    });
    EXPECT_EQ(ret1.get(), 0);
    EXPECT_EQ(ret2.get(), 0);
    auto bufferedSize = openInstance->writeStream.GetSize();
    EXPECT_EQ(bufferedSize, 0);
    EXPECT_EQ(openInstance->currentSize.load(), size);
    free(buf);
}

TEST_F(CuckooStoreUT, WriteThroughRemoteDifferent)
{
    NewOpenInstance(2000, StoreNode::GetInstance()->GetNodeId() - 1, "/WriteRemote", O_WRONLY);

    size_t size = CUCKOO_STORE_STREAM_MAX_SIZE + 1;
    char *buf = (char *)malloc(size);
    strcpy(buf, "abc");

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, size);
    });
    EXPECT_EQ(ret1.get(), 0);
    EXPECT_EQ(ret2.get(), 0);
    auto bufferedSize = openInstance->writeStream.GetSize();
    EXPECT_EQ(bufferedSize, 0);
    EXPECT_EQ(openInstance->currentSize.load(), size * 2);
    free(buf);
}

TEST_F(CuckooStoreUT, WriteBackRemoteSame)
{
    NewOpenInstance(2000, StoreNode::GetInstance()->GetNodeId() - 1, "/WriteRemote", O_WRONLY);

    size_t size = CUCKOO_STORE_STREAM_MAX_SIZE / 2;
    char *buf = (char *)malloc(size);
    strcpy(buf, "abc");

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, 0);
    });
    EXPECT_EQ(ret1.get(), 0);
    EXPECT_EQ(ret2.get(), 0);
    auto bufferedSize = openInstance->writeStream.GetSize();
    EXPECT_EQ(bufferedSize, size);
    EXPECT_EQ(openInstance->currentSize.load(), size);
    free(buf);
}

TEST_F(CuckooStoreUT, WriteBackLocalDifferent)
{
    NewOpenInstance(1000, StoreNode::GetInstance()->GetNodeId(), "/WriteLocal", O_WRONLY);
    openInstance->currentSize = size;

    size_t size = CUCKOO_STORE_STREAM_MAX_SIZE + 1;
    char *buf = (char *)malloc(size);
    strcpy(buf, "abc");

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->WriteFile(openInstance.get(), buf, size, size);
    });
    EXPECT_EQ(ret1.get(), 0);
    EXPECT_EQ(ret2.get(), 0);
    // not sure of buffered data size, can be size or size * 2
    // auto bufferedSize = openInstance->writeStream.GetSize();
    // EXPECT_EQ(bufferedSize, 0);
    EXPECT_EQ(openInstance->currentSize.load(), size * 2);
    free(buf);
}

/* ------------------------------------------- read local -------------------------------------------*/

TEST_F(CuckooStoreUT, ReadLocalSmallSame)
{
    NewOpenInstance(10000, StoreNode::GetInstance()->GetNodeId(), "/ReadLocalSmall", O_WRONLY | O_CREAT);
    ResetBuf(false);
    int ret = CuckooStore::GetInstance()->WriteFile(openInstance.get(), writeBuf, size, 0);
    EXPECT_EQ(ret, 0);

    NewOpenInstance(10000, StoreNode::GetInstance()->GetNodeId(), "/ReadLocalSmall", O_RDONLY);
    openInstance->originalSize = size;
    openInstance->currentSize = size;
    auto buffer = std::shared_ptr<char>((char *)malloc(size), free);
    openInstance->readBuffer = buffer;
    openInstance->readBufferSize = size;
    ret = CuckooStore::GetInstance()->ReadSmallFiles(openInstance.get());
    EXPECT_EQ(ret, 0);

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf, readSize, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf2, readSize, 0);
    });
    EXPECT_EQ(ret1.get(), readSize);
    EXPECT_EQ(ret2.get(), readSize);

    EXPECT_EQ(0, memcmp(writeBuf, readBuf, readSize));
    EXPECT_EQ(0, memcmp(writeBuf, readBuf2, readSize));
}

TEST_F(CuckooStoreUT, ReadLocalSmallDifferent)
{
    NewOpenInstance(10000, StoreNode::GetInstance()->GetNodeId(), "/ReadLocalSmall", O_WRONLY);
    ResetBuf(false);
    int ret = CuckooStore::GetInstance()->WriteFile(openInstance.get(), writeBuf, size, 0);
    EXPECT_EQ(ret, 0);

    NewOpenInstance(10000, StoreNode::GetInstance()->GetNodeId(), "/ReadLocalSmall", O_RDONLY);
    openInstance->originalSize = size;
    openInstance->currentSize = size;
    auto buffer = std::shared_ptr<char>((char *)malloc(size), free);
    openInstance->readBuffer = buffer;
    openInstance->readBufferSize = size;
    ret = CuckooStore::GetInstance()->ReadSmallFiles(openInstance.get());
    EXPECT_EQ(ret, 0);

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf, readSize, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf2, readSize, readSize);
    });
    EXPECT_EQ(ret1.get(), readSize);
    EXPECT_EQ(ret2.get(), readSize);

    EXPECT_EQ(0, memcmp(writeBuf, readBuf, readSize));
    EXPECT_EQ(0, memcmp(writeBuf + readSize, readBuf2, readSize));
}

TEST_F(CuckooStoreUT, ReadLocalLargeSame)
{
    NewOpenInstance(10001, StoreNode::GetInstance()->GetNodeId(), "/ReadLocalLarge", O_WRONLY | O_CREAT);
    ResetBuf(true);

    int ret = CuckooStore::GetInstance()->WriteFile(openInstance.get(), writeBuf, size, 0);
    EXPECT_EQ(ret, 0);

    NewOpenInstance(10001, StoreNode::GetInstance()->GetNodeId(), "/ReadLocalLarge", O_RDONLY);
    openInstance->originalSize = size;
    openInstance->currentSize = size;

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf, readSize, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf2, readSize, 0);
    });
    EXPECT_EQ(ret1.get(), readSize);
    EXPECT_EQ(ret2.get(), readSize);

    EXPECT_EQ(0, memcmp(writeBuf, readBuf, readSize));
    EXPECT_EQ(0, memcmp(writeBuf, readBuf2, readSize));
}

TEST_F(CuckooStoreUT, ReadLocalLargeDifferent)
{
    NewOpenInstance(10001, StoreNode::GetInstance()->GetNodeId(), "/ReadLocalLarge", O_RDONLY);
    openInstance->originalSize = size;
    openInstance->currentSize = size;

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf, readSize, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf2, readSize, readSize);
    });
    EXPECT_EQ(ret1.get(), readSize);
    EXPECT_EQ(ret2.get(), readSize);

    EXPECT_EQ(0, memcmp(writeBuf, readBuf, readSize));
    EXPECT_EQ(0, memcmp(writeBuf + readSize, readBuf2, readSize));
}

/* ------------------------------------------- read remote -------------------------------------------*/

TEST_F(CuckooStoreUT, ReadRemoteSmallSame)
{
    NewOpenInstance(20000, StoreNode::GetInstance()->GetNodeId() - 1, "/ReadRemoteSmall", O_WRONLY | O_CREAT);
    ResetBuf(false);
    int ret = CuckooStore::GetInstance()->WriteFile(openInstance.get(), writeBuf, size, 0);
    EXPECT_EQ(ret, 0);

    NewOpenInstance(20000, StoreNode::GetInstance()->GetNodeId() - 1, "/ReadRemoteSmall", O_RDONLY);
    openInstance->originalSize = size;
    openInstance->currentSize = size;
    auto buffer = std::shared_ptr<char>((char *)malloc(size), free);
    openInstance->readBuffer = buffer;
    openInstance->readBufferSize = size;
    ret = CuckooStore::GetInstance()->ReadSmallFiles(openInstance.get());
    EXPECT_EQ(ret, 0);

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf, readSize, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf2, readSize, 0);
    });
    EXPECT_EQ(ret1.get(), readSize);
    EXPECT_EQ(ret2.get(), readSize);

    EXPECT_EQ(0, memcmp(writeBuf, readBuf, readSize));
    EXPECT_EQ(0, memcmp(writeBuf, readBuf2, readSize));
}

TEST_F(CuckooStoreUT, ReadRemoteSmallDifferent)
{
    NewOpenInstance(20000, StoreNode::GetInstance()->GetNodeId() - 1, "/ReadRemoteSmall", O_WRONLY);
    ResetBuf(false);
    int ret = CuckooStore::GetInstance()->WriteFile(openInstance.get(), writeBuf, size, 0);
    EXPECT_EQ(ret, 0);

    NewOpenInstance(20000, StoreNode::GetInstance()->GetNodeId() - 1, "/ReadRemoteSmall", O_RDONLY);
    openInstance->originalSize = size;
    openInstance->currentSize = size;
    auto buffer = std::shared_ptr<char>((char *)malloc(size), free);
    openInstance->readBuffer = buffer;
    openInstance->readBufferSize = size;
    ret = CuckooStore::GetInstance()->ReadSmallFiles(openInstance.get());
    EXPECT_EQ(ret, 0);

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf, readSize, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf2, readSize, readSize);
    });
    EXPECT_EQ(ret1.get(), readSize);
    EXPECT_EQ(ret2.get(), readSize);

    EXPECT_EQ(0, memcmp(writeBuf, readBuf, readSize));
    EXPECT_EQ(0, memcmp(writeBuf + readSize, readBuf2, readSize));
}

TEST_F(CuckooStoreUT, ReadRemoteLargeSame)
{
    NewOpenInstance(20001, StoreNode::GetInstance()->GetNodeId() - 1, "/ReadRemoteLarge", O_WRONLY | O_CREAT);
    ResetBuf(true);

    int ret = CuckooStore::GetInstance()->WriteFile(openInstance.get(), writeBuf, size, 0);
    EXPECT_EQ(ret, 0);

    NewOpenInstance(20001, StoreNode::GetInstance()->GetNodeId() - 1, "/ReadRemoteLarge", O_RDONLY);
    openInstance->originalSize = size;
    openInstance->currentSize = size;

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf, readSize, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf2, readSize, 0);
    });
    EXPECT_EQ(ret1.get(), readSize);
    EXPECT_EQ(ret2.get(), readSize);

    EXPECT_EQ(0, memcmp(writeBuf, readBuf, readSize));
    EXPECT_EQ(0, memcmp(writeBuf, readBuf2, readSize));
}

TEST_F(CuckooStoreUT, ReadRemoteLargeDifferent)
{
    NewOpenInstance(20001, StoreNode::GetInstance()->GetNodeId() - 1, "/ReadRemoteLarge", O_RDONLY);
    openInstance->originalSize = size;
    openInstance->currentSize = size;

    std::future<int> ret1 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf, readSize, 0);
    });
    std::future<int> ret2 = std::async(std::launch::async, [&]() {
        return CuckooStore::GetInstance()->ReadFile(openInstance.get(), readBuf2, readSize, readSize);
    });
    EXPECT_EQ(ret1.get(), readSize);
    EXPECT_EQ(ret2.get(), readSize);

    EXPECT_EQ(0, memcmp(writeBuf, readBuf, readSize));
    EXPECT_EQ(0, memcmp(writeBuf + readSize, readBuf2, readSize));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
