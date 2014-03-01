/** \copyright
 * Copyright (c) 2014, Balazs Racz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \file memory_config_test.cc
 *
 * Unit tests for the Memory Config protocol support.
 *
 * @author Balazs Racz
 * @date 23 Feb 2014
 */

#include "utils/async_datagram_test_helper.hxx"
#include "nmranet/NMRAnetAsyncMemoryConfig.hxx"

namespace NMRAnet
{

class MockMemorySpace : public MemorySpace
{
public:
    MOCK_METHOD0(read_only, bool());
    MOCK_METHOD0(min_address, address_t());
    MOCK_METHOD0(max_address, address_t());

    MOCK_METHOD4(write, void(address_t destination, const uint8_t* data,
                             size_t len, errorcode_t* error));
    MOCK_METHOD4(read, size_t(address_t source, uint8_t* dst, size_t len,
                              errorcode_t* error));
};

class MemoryConfigTest : public TwoNodeDatagramTest
{
protected:
    MemoryConfigTest() : memory_one(&datagram_support_, node_, 10)
    {
    }
    ~MemoryConfigTest()
    {
    }

    MemoryConfigHandler memory_one;
    std::unique_ptr<MemoryConfigHandler> memory_two;
};

void FillPayload(uint8_t* dst, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        dst[i] = '0' + (i % 10);
    }
}

using testing::DoAll;
using testing::WithArgs;

TEST_F(MemoryConfigTest, MockMemoryConfigRead)
{
    StrictMock<MockMemorySpace> space;
    memory_one.registry()->insert(node_, 0x27, &space);

    EXPECT_CALL(space, read(0x100, _, 10, _))
        .WillOnce(DoAll(WithArgs<1, 2>(Invoke(&FillPayload)), Return(10)));

    ExpectPacket(":X19A2822AN077C80;"); // received ok, response pending
    ExpectPacket(
        ":X1B77C22AN2050000001002730;");
    ExpectPacket(
        ":X1C77C22AN3132333435363738;");
    ExpectPacket(
        ":X1D77C22AN39;");

    SendPacket(":X1A22A77CN204000000100270A;");
    Wait();
}

} // namespace